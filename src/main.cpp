#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include "httplistener.h"

#include "database/db_controller.h"

#include "httpserver/request_router.h"
#include "httpserver/utils/jwt_manager.h"

#include "utils/utils.h"

#include "global.h"

using stefanfrings::HttpListener;



void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString formattedMessage;

    // Define ANSI color codes
    const char *debugColor = "\033[34m";     // Blue
    const char *infoColor = "\033[32m";      // Green
    const char *warningColor = "\033[33m";   // Yellow
    const char *criticalColor = "\033[31m";  // Red
    const char *fatalColor = "\033[41m";     // Red background
    const char *resetColor = "\033[0m";      // Reset

    // Format message based on type
    switch (type) {
        case QtDebugMsg:
            formattedMessage = QString("%1%2 [Debug] %3%4").arg(debugColor, timestamp, localMsg.constData(), resetColor);
            break;
        case QtInfoMsg:
            formattedMessage = QString("%1%2 [Info] %3%4").arg(infoColor, timestamp, localMsg.constData(), resetColor);
            break;
        case QtWarningMsg:
            formattedMessage = QString("%1%2 [Warning] %3%4").arg(warningColor, timestamp, localMsg.constData(), resetColor);
            break;
        case QtCriticalMsg:
            formattedMessage = QString("%1%2 [Critical] %3%4").arg(criticalColor, timestamp, localMsg.constData(), resetColor);
            break;
        case QtFatalMsg:
            formattedMessage = QString("%1%2 [Fatal] %3%4").arg(fatalColor, timestamp, localMsg.constData(), resetColor);
            break;
    }

    // Print to console
    fprintf(stderr, "%s\n", formattedMessage.toLocal8Bit().constData());
    fflush(stderr);

    // Write plain text to a log file without colors
    QFile logFile("application.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream logStream(&logFile);
        logStream << QString("[%1] [%2] %3")
                         .arg(timestamp)
                         .arg(type == QtDebugMsg ? "Debug" :
                              type == QtInfoMsg ? "Info" :
                              type == QtWarningMsg ? "Warning" :
                              type == QtCriticalMsg ? "Critical" : "Fatal")
                         .arg(localMsg.constData())
                  << "/n";
    }

    if (type == QtFatalMsg) {
        abort();
    }
}

QString searchConfigFile(QString fn="settings.ini") {
    QString binDir=QCoreApplication::applicationDirPath();
    qWarning()<<binDir;
    QString appName=QCoreApplication::applicationName();
    QString fileName(fn);

    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/..");
    searchList.append(binDir+"/etc");
    searchList.append(binDir+"/../etc");
    searchList.append(binDir+"/../"+appName+"/etc");     // for development with shadow build (Linux)
    searchList.append(binDir+"/../../"+appName+"/etc");  // for development with shadow build (Windows)
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (QString dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            fileName=QDir(file.fileName()).canonicalPath();
            qInfo("Using config file %s",qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
    }
    qFatal("Cannot find config file %s",qPrintable(fileName));
}

int main(int argc, char*argv[]){
    QCoreApplication app(argc, argv);
    // Custom message handler
    qInstallMessageHandler(customMessageHandler);

    // Global settings initialization
    QString configFileName=searchConfigFile(QString("Novinsettings.ini"));
    myGlobalSettings = new QSettings(configFileName,QSettings::IniFormat,&app);

    // Jwt manager initialization
    myJWTManager = new JWTManager();

    // Data base initialization
    myDbController = new DbController ();

    // Initialize static folder utils
    myStaticFolderUtils = new StaticFolderUtils();

    // Initialize Swagger Doc Generator
    mySwaggerGenerator = new SwaggerGenerator();

    /*  Initialize listener
     *      note: the settings must be given in a seperate
     *      QSettings pointer
    */
    QSettings* myFileListenerSettings  = new QSettings(configFileName, QSettings::IniFormat, &app);
    myFileListenerSettings->beginGroup("files");
    myStaticFileController = new StaticFileController(myFileListenerSettings);

    /*  Initialize listener settings
     *      note: the settings must be given in a seperate
     *      QSettings pointer
    */
    QSettings* myHttpListenerSettings = new QSettings(configFileName,QSettings::IniFormat,&app);
    myHttpListenerSettings->beginGroup("listener");
    new HttpListener(myHttpListenerSettings,new ApiRequestMapper());


    QString server_url = myGlobalSettings->value("swagger/server_url").toString();
    mySwaggerGenerator->setTitle(myGlobalSettings->value("swagger/title").toString());
    mySwaggerGenerator->setVersion(myGlobalSettings->value("swagger/version").toString());
    mySwaggerGenerator->addServer(server_url);
    mySwaggerGenerator->setDescription(myGlobalSettings->value("swagger/description").toString());
    mySwaggerGenerator->writeToFile(myStaticFolderUtils->GetRootStaticPath().append(myGlobalSettings->value("static/swaggerDocsJson").toString()));

    myAmiController = new AmiController();
    app.exec();
}








