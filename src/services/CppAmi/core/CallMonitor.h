#ifndef CALLMONITOR_HPP
#define CALLMONITOR_HPP

#include <QObject>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QFile>

#include "AmiTypes.h"

struct CallSummary {
    QString uniqueId;
    QString linkedId;
    QString channel;
    QString extensionNumber;      // The extension being tracked
    QString otherExtensionNumber; // The other party in the call
    QString direction;           // "in" for incoming, "out" for outgoing
    QString callType;           // "internal" or "external"
    QString callerIdNum;
    QString callerIdName;
    QString connectedLineNum;
    QString connectedLineName;
    QString status;
    bool hasRinging;
    QDateTime startTime;
    QDateTime endTime;
    QDateTime bridgeStartTime;
    QDateTime bridgeEndTime;
    QString bridgeUniqueId;
    QString bridgeType;
    QString bridgeTechnology;
    QString cause;
    QString causeDescription;
    QString causeTxt;

    CallSummary() : hasRinging(false) {}
};

class CallMonitor : public QObject {
    Q_OBJECT

public:
    explicit CallMonitor(const QString& targetExtension, bool logRawEvents = false, QObject* parent = nullptr);
    ~CallMonitor();

    void processEvent(const QJsonObject& eventData);

signals:
    void callStarted(const QString& channel, const QJsonObject& callData);
    void callEnded(const QString& channel, const QJsonObject& callData);
    void callStateChanged(const QString& channel, const QString& state);

private:
    QString m_targetExtension;
    bool m_logRawEvents;
    QMap<QString, CallSummary> m_activeCalls;
    QFile m_callLogFile;
    QFile m_rawLogFile;

    void logCallEvent(const QJsonObject& callData);
    void logRawEvent(const QJsonObject& event);
    bool isTargetChannel(const QString& channel);
    void storeCallSummary(const CallSummary& summary);
    void handleNewChannel(const QJsonObject& event);
    void handleNewState(const QJsonObject& event);
    void handleBridgeEvent(const QJsonObject& event);
    void handleHangupEvent(const QJsonObject& event);
    QJsonObject callSummaryToJson(const CallSummary& summary);
};

#endif // CALLMONITOR_HPP 
