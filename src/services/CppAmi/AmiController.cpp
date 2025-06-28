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

    if(ext_user_data.isEmpty()){qFatal("Failed to get allowed extentions from ICT API server");}
    // Check if the user exists
    for (auto user : ext_user_data) {
        QJsonObject userObj = user.toObject();
        QString username = userObj.value("userName").toString();
        QString fullname = userObj.value("fullName").toString();
        QString ext_num = QString::number(userObj.value("extesion").toInt());

        if (!myDbController->checkUserExistsByUsername(username)) {
            myDbController->createUser(fullname, ext_num, username);

#ifdef QT_DEBUG
            qWarning() << "Inserted new user:"
                       << "Username:" << username
                       << "Fullname:" << fullname
                       << "Extension:" << ext_num;
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
        QString db_username = db_user_obj.value("username").toString();
        bool is_active_in_ict = active_usernames.contains(db_username);
        bool current_is_active = db_user_obj.value("is_active").toBool();

        // Update only if the status is different
        if (current_is_active != is_active_in_ict) {
            myDbController->updateUserIsActive(db_user_obj.value("id").toInt(), is_active_in_ict);

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
