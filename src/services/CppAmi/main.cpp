#include <QCoreApplication>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include "CallMonitorManager.h"
#include "AmiClient.h"
#include "AmiLogger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // List of extensions to monitor (same as in Python version)
    QStringList extensions = {
        "255", "256", "250", "260", "259", "262", "265",
        "251", "253", "280", "264", "416", "413"
    };

    // Create call monitor manager
    CallMonitorManager manager(extensions, true);

    // Create AMI client
    AmiClient amiClient;

    // Connect AMI client signals

    /*
     * Connect generic loggers
    */
    QObject::connect(&amiClient, &AmiClient::connected, []() {
        AmiLogger::instance().info("Connected to AMI server");
    });

    QObject::connect(&amiClient, &AmiClient::disconnected, []() {
        AmiLogger::instance().info("Disconnected from AMI server");
    });

    QObject::connect(&amiClient, &AmiClient::error, [](const QString& errorMessage) {
        AmiLogger::instance().error("AMI Error: " + errorMessage);
    });

    // Connect AMI events to monitor manager
    /*
     * Connect AMI events to CallMonitorManager to map
     * to the correct CallMonitor Instance.
    */
    QObject::connect(&amiClient, &AmiClient::eventReceived,
                    &manager, &CallMonitorManager::processEvent);

    // Connect to manager signals
    QObject::connect(&manager, &CallMonitorManager::callStarted,
        [](const QString& extension, const QString& channel, const QJsonObject& callData) {
            AmiLogger::instance().info(
                QString("Call started on extension %1").arg(extension),
                QJsonObject{
                    {"channel", channel},
                    {"caller", callData["caller_id"].toString()}
                }
            );
        });

    QObject::connect(&manager, &CallMonitorManager::callEnded,
        [](const QString& extension, const QString& channel, const QJsonObject& callData) {
            AmiLogger::instance().info(
                QString("Call ended on extension %1").arg(extension),
                QJsonObject{
                    {"channel", channel},
                    {"duration", callData["call_duration"].toInt()}
                }
            );
        });

    QObject::connect(&manager, &CallMonitorManager::callStateChanged,
        [](const QString& extension, const QString& channel, const QString& state) {
            AmiLogger::instance().info(
                QString("Call state changed on extension %1").arg(extension),
                QJsonObject{
                    {"channel", channel},
                    {"state", state}
                }
            );
        });

    // Connect to AMI server
    if (!amiClient.connectToServer("192.168.20.32", 5038, "softnuami", "Novin22!")) {
        AmiLogger::instance().error("Failed to connect to AMI server");
        return 1;
    }

    return app.exec();
} 
