#include "AmiController.h"
#include "../../global.h"
#include <QDebug>
AmiController::AmiController() {
    /*
     * checks ICT API and inserts newly added users and updates the is_active status of db_users
     */
    check_users();

    /*
     * Extract extensions to be monitored from ICT API and initialize
     * Call monitor manager which is for monitoring each extension's
     * events, logging them, and inserting the summary for each into
     * the database.
     */
    initialize_ami_monitor_instances();
}

void AmiController::check_users() {
    /*
     * Check if any new user was added
     */

    // Fetch extensions data from ICT API
    QJsonArray ext_user_data = IctApiUtils::getAllowedExtentions();

    struct IctUser{
        QString username;
        QString fullname;
        QString extentionNo;
    };
    // EXAMPLE RESPONSE:
    // {
    //     "fullName": "زهرا رباني",
    //     "userName": "rabani",
    //     "extesion": 265
    // }

    if(ext_user_data.isEmpty()){
        qFatal("Failed to get allowed extentions from ICT API server");
    }
    // Check if the user exists
    QMap<QString ,IctUser> IctAllowedUsers;

    for (auto user : ext_user_data) {
        QJsonObject userObj = user.toObject();

        IctUser ict_user;
        ict_user.username = userObj.value("userName").toString();
        ict_user.fullname = userObj.value("fullName").toString();
        ict_user.extentionNo = QString::number(userObj.value("extesion").toInt());


        IctAllowedUsers.insert(ict_user.username,ict_user);

        if (!myDbController->checkUserExistsByUsername(ict_user.username)) {
            myDbController->createUser(ict_user.fullname, ict_user.extentionNo, ict_user.username);

            #ifdef QT_DEBUG
            qWarning() << "Inserted new user:"
                       << "Username:" << ict_user.username
                       << "Fullname:" << ict_user.fullname
                       << "Extension:" << ict_user.extentionNo;
            #endif
        }
    }

    /*
     * Check if any user got deactivated
     */

    // Extract usernames from ICT API
    QStringList active_usernames;
    for (auto user : ext_user_data) {
        QJsonObject userObj = user.toObject();
        QString username = userObj.value("userName").toString();
        active_usernames.append(username);
    }

    // Fetch all users from the database
    DbResault dbRes = myDbController->listAllUsers();
    if (!dbRes.status) {
        qFatal("Failed to check for new users: Database failed to fetch the list of users");
    }

    QJsonArray all_users = dbRes.data.value("users").toArray();

    // Update is_active status for deactivated users
    for (auto db_user : all_users) {
        QJsonObject db_user_obj = db_user.toObject();

        qint64 db_id = db_user_obj.value("id").toInt();
        QString db_username = db_user_obj.value("username").toString();
        QString db_fullname = db_user_obj.value("fullname").toString();
        int db_ext_number = db_user_obj.value("extension_number").toInt();

        // determine active status in ict and db
        bool is_active_in_ict = active_usernames.contains(db_username);
        bool current_is_active = db_user_obj.value("is_active").toBool();

        if(is_active_in_ict){
            IctUser ict_user = IctAllowedUsers.value(db_username);
            myDbController->updateUserInfo(db_id,ict_user.fullname,"",ict_user.extentionNo);
        }

        // Update only if the status is different
        if (current_is_active != is_active_in_ict) {
            myDbController->updateUserIsActive(db_user_obj.value("id").toInt(), is_active_in_ict);

            // Update other credentials username and extention number
            #ifdef QT_DEBUG
            if (is_active_in_ict) {
                qWarning() << "User reactivated:"
                           << "Username:" << db_username;
            } else {
                qWarning() << "User deactivated:"
                           << "Username:" << db_username;
            }
            #endif
        }
    }
}

void AmiController::initialize_ami_monitor_instances() {
    // Fetch extensions data from ICT API and store it in a string list
    QJsonArray ext_user_data = IctApiUtils::getAllowedExtentions();
    QStringList ext_to_monitor;
    for (auto user : ext_user_data) {
        QJsonObject user_obj = user.toObject();
        ext_to_monitor.append(QString::number(user_obj.value("extesion").toInt()));
    }

    m_callMonitorManager = new CallMonitorManager(ext_to_monitor, true);
    m_amiClient = new AmiClient();

    /*
     * Connect AMI events to monitor manager. The CallMonitorManager maps the incoming
     * AMI event to the correct CallMonitor handler.
     */
    QObject::connect(m_amiClient, &AmiClient::eventReceived,
                     m_callMonitorManager, &CallMonitorManager::processEvent);

    /*
     * Setup logging functionality based on AMI/terminalLog
     */
    if (myGlobalSettings->value("AMI/terminalLog").toBool()) {
        QObject::connect(m_amiClient, &AmiClient::connected, []() {
            AmiLogger::instance().info("Connected to AMI server");
        });

        QObject::connect(m_amiClient, &AmiClient::disconnected, []() {
            AmiLogger::instance().info("Disconnected from AMI server");
        });

        QObject::connect(m_amiClient, &AmiClient::error, [](const QString& errorMessage) {
            AmiLogger::instance().error("AMI Error: " + errorMessage);
        });

        QObject::connect(m_callMonitorManager, &CallMonitorManager::callStarted,
                         [](const QString& extension, const QString& channel, const QJsonObject& callData) {
                             AmiLogger::instance().info(
                                 QString("Call started on extension %1").arg(extension),
                                 QJsonObject{
                                     {"channel", channel},
                                     {"caller", callData["caller_id"].toString()}
                                 });
                         });

        QObject::connect(m_callMonitorManager, &CallMonitorManager::callEnded,
                         [](const QString& extension, const QString& channel, const QJsonObject& callData) {
                             AmiLogger::instance().info(
                                 QString("Call ended on extension %1").arg(extension),
                                 QJsonObject{
                                     {"channel", channel},
                                     {"duration", callData["call_duration"].toInt()}
                                 });
                         });

        QObject::connect(m_callMonitorManager, &CallMonitorManager::callStateChanged,
                         [](const QString& extension, const QString& channel, const QString& state) {
                             AmiLogger::instance().info(
                                 QString("Call state changed on extension %1").arg(extension),
                                 QJsonObject{
                                     {"channel", channel},
                                     {"state", state}
                                 });
                         });
    }

    if (!m_amiClient->connectToServer("192.168.20.32", 5038, "softnuami", "Novin22!")) {
        AmiLogger::instance().error("Failed to connect to AMI server");
        qFatal("Failed to connect to AMI server") ;
    }
}
