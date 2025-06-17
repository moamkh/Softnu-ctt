#ifndef CALLMONITORMANAGER_HPP
#define CALLMONITORMANAGER_HPP

#include <QObject>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include "CallMonitor.h"

class CallMonitorManager : public QObject {
    Q_OBJECT

public:
    explicit CallMonitorManager(const QStringList& extensions, bool logRawEvents = true, QObject* parent = nullptr);
    ~CallMonitorManager();

    void processEvent(const QJsonObject& eventData);

signals:
    void callStarted(const QString& extension, const QString& channel, const QJsonObject& callData);
    void callEnded(const QString& extension, const QString& channel, const QJsonObject& callData);
    void callStateChanged(const QString& extension, const QString& channel, const QString& state);

private:
    QMap<QString, CallMonitor*> m_callMonitors;
    bool m_logRawEvents;
};

#endif // CALLMONITORMANAGER_HPP 
