#include "CallMonitorManager.h"
#include "AmiLogger.h"

#include <QDebug>

CallMonitorManager::CallMonitorManager(const QStringList& extensions, bool logRawEvents, QObject* parent)
    : QObject(parent)
    , m_logRawEvents(logRawEvents)
{
    for (const QString& ext : extensions) {
        CallMonitor* monitor = new CallMonitor(ext, logRawEvents, this);
        m_callMonitors[ext] = monitor;

        // Connect monitor signals to manager signals
        connect(monitor, &CallMonitor::callStarted,
                [this, ext](const QString& channel, const QJsonObject& callData) {
                    emit callStarted(ext, channel, callData);
                });

        connect(monitor, &CallMonitor::callEnded,
                [this, ext](const QString& channel, const QJsonObject& callData) {
                    emit callEnded(ext, channel, callData);
                });

        connect(monitor, &CallMonitor::callStateChanged,
                [this, ext](const QString& channel, const QString& state) {
                    emit callStateChanged(ext, channel, state);
                });

        AmiLogger::instance().info(QString("Initialized CallMonitor for extension %1").arg(ext));
    }
}

CallMonitorManager::~CallMonitorManager()
{
    // Qt's parent-child relationship will handle deletion of CallMonitor objects
}

void CallMonitorManager::processEvent(const QJsonObject& eventData)
{
    QString channel = eventData["Channel"].toString();
    
    // Extract extension from channel name (e.g., "SIP/255-00000001" -> "255")
    QString extension;
    if (channel.startsWith("SIP/")) {
        int dashPos = channel.indexOf('-', 4);
        if (dashPos != -1) {
            extension = channel.mid(4, dashPos - 4);
        }
    }

    // Process event in the appropriate monitor
    if (!extension.isEmpty() && m_callMonitors.contains(extension)) {
        m_callMonitors[extension]->processEvent(eventData);
    }
} 
