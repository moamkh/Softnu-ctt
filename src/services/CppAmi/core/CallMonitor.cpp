#include "CallMonitor.h"
#include "AmiLogger.h"
#include <QDir>
#include <QJsonDocument>
#include <QDateTime>

#include "../../../global.h"
CallMonitor::CallMonitor(const QString& targetExtension, bool logRawEvents, QObject* parent)
    : QObject(parent)
    , m_targetExtension(targetExtension)
    , m_logRawEvents(logRawEvents)
{
    // Setup call logs directory
    QDir logDir(QDir::currentPath() + "/logs");
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }

    // Create extension-specific log file
    m_callLogFile.setFileName(logDir.filePath(QString("call_logs_%1.log").arg(targetExtension)));
    m_callLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);

    // Create raw events log file if enabled
    if (m_logRawEvents) {
        m_rawLogFile.setFileName(logDir.filePath(QString("raw_call_logs_%1.log").arg(targetExtension)));
        m_rawLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }

    // Log initialization
    QJsonObject initEvent;
    initEvent["type"] = "monitor_initialized";
    initEvent["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    initEvent["extension"] = targetExtension;
    initEvent["privilege"] = "call,all";
    logCallEvent(initEvent);
}

CallMonitor::~CallMonitor()
{
    if (m_callLogFile.isOpen()) {
        m_callLogFile.close();
    }
    if (m_rawLogFile.isOpen()) {
        m_rawLogFile.close();
    }
}

void CallMonitor::processEvent(const QJsonObject& eventData)
{
    // Log raw event if enabled
    if (m_logRawEvents) {
        logRawEvent(eventData);
    }

    QString eventType = eventData["Event"].toString();
    QString channel = eventData["Channel"].toString();

    if (!isTargetChannel(channel)) {
        return;
    }

    if (eventType == "Newchannel") {
        handleNewChannel(eventData);
    }
    else if (eventType == "Newstate") {
        handleNewState(eventData);
    }
    else if (eventType == "Bridge") {
        handleBridgeEvent(eventData);
    }
    else if (eventType == "Hangup") {
        handleHangupEvent(eventData);
    }
}

void CallMonitor::logCallEvent(const QJsonObject& callData)
{
    if (!m_callLogFile.isOpen()) return;

    QJsonDocument doc(callData);
    m_callLogFile.write(doc.toJson(QJsonDocument::Indented) + "\n");
    m_callLogFile.flush();
}

void CallMonitor::logRawEvent(const QJsonObject& event)
{
    if (!m_rawLogFile.isOpen()) return;

    QJsonDocument doc(event);
    m_rawLogFile.write(doc.toJson(QJsonDocument::Indented) + "\n");
    m_rawLogFile.flush();
}

bool CallMonitor::isTargetChannel(const QString& channel)
{
    return channel.startsWith(QString("SIP/%1-").arg(m_targetExtension));
}

void CallMonitor::handleNewChannel(const QJsonObject& event)
{
    QString channel = event["Channel"].toString();
    CallSummary summary;
    summary.uniqueId = event["Uniqueid"].toString();
    summary.linkedId = event["Linkedid"].toString();
    summary.channel = channel;
    summary.callerIdNum = event["CallerIDNum"].toString();
    summary.callerIdName = event["CallerIDName"].toString();
    summary.connectedLineNum = event["ConnectedLineNum"].toString();
    summary.connectedLineName = event["ConnectedLineName"].toString();
    summary.status = "initiated";
    summary.startTime = QDateTime::currentDateTime();
    summary.extensionNumber = m_targetExtension;

    m_activeCalls[channel] = summary;

    QJsonObject logEvent = callSummaryToJson(summary);
    logEvent["type"] = "call_started";
    logEvent["timestamp"] = summary.startTime.toString(Qt::ISODate);
    logCallEvent(logEvent);

    emit callStarted(channel, logEvent);
}

void CallMonitor::handleNewState(const QJsonObject& event)
{
    QString channel = event["Channel"].toString();
    QString channelState = event["ChannelState"].toString();

    if (!m_activeCalls.contains(channel)) {
        return;
    }

    CallSummary& summary = m_activeCalls[channel];

    if (channelState == "5") { // Ringing
        summary.status = "ringing";
        summary.hasRinging = true;
        summary.direction = "in";
        summary.otherExtensionNumber = summary.connectedLineNum;
        QJsonObject logEvent;
        logEvent["type"] = "call_ringing";
        logEvent["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        logEvent["channel"] = channel;
        logEvent["uniqueid"] = summary.uniqueId;
        logEvent["privilege"] = "call,all";
        logCallEvent(logEvent);
        emit callStateChanged(channel, "ringing");
    }
    else if (channelState == "6") { // Up
        summary.status = "answered";
        summary.startTime = QDateTime::currentDateTime();
        QJsonObject logEvent;
        logEvent["type"] = "call_answered";
        logEvent["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        logEvent["channel"] = channel;
        logEvent["uniqueid"] = summary.uniqueId;
        logEvent["privilege"] = "call,all";
        logCallEvent(logEvent);
        emit callStateChanged(channel, "answered");
    }
}

void CallMonitor::handleBridgeEvent(const QJsonObject& event)
{
    QString channel = event["Channel"].toString();
    QString bridgeState = event["BridgeState"].toString();

    if (!m_activeCalls.contains(channel)) {
        return;
    }

    CallSummary& summary = m_activeCalls[channel];

    if (bridgeState == "Link") {
        summary.bridgeStartTime = QDateTime::currentDateTime();
        summary.bridgeUniqueId = event["BridgeUniqueid"].toString();
        summary.bridgeType = event["BridgeType"].toString();
        summary.bridgeTechnology = event["BridgeTechnology"].toString();

        QJsonObject logEvent;
        logEvent["type"] = "bridge_enter";
        logEvent["timestamp"] = summary.bridgeStartTime.toString(Qt::ISODate);
        logEvent["channel"] = channel;
        logEvent["uniqueid"] = summary.uniqueId;
        logEvent["bridge_uniqueid"] = summary.bridgeUniqueId;
        logEvent["bridge_type"] = summary.bridgeType;
        logEvent["privilege"] = "call,all";
        logCallEvent(logEvent);
    }
    else if (bridgeState == "Unlink") {
        summary.bridgeEndTime = QDateTime::currentDateTime();

        QJsonObject logEvent;
        logEvent["type"] = "bridge_leave";
        logEvent["timestamp"] = summary.bridgeEndTime.toString(Qt::ISODate);
        logEvent["channel"] = channel;
        logEvent["uniqueid"] = summary.uniqueId;
        logEvent["bridge_uniqueid"] = summary.bridgeUniqueId;
        logEvent["bridge_type"] = summary.bridgeType;
        logEvent["privilege"] = "call,all";
        logCallEvent(logEvent);
    }
}

void CallMonitor::handleHangupEvent(const QJsonObject& event)
{
    QString channel = event["Channel"].toString();
    if (!m_activeCalls.contains(channel)) {
        return;
    }

    CallSummary& summary = m_activeCalls[channel];
    summary.endTime = QDateTime::currentDateTime();
    summary.cause = event["Cause"].toString();
    summary.causeDescription = event["CauseDescription"].toString();
    summary.causeTxt = event["CauseTxt"].toString();

    // Set direction and other extension number based on call flow
    if (summary.hasRinging) {
        // Incoming call
        summary.direction = "in";
        summary.otherExtensionNumber = event["ConnectedLineNum"].toString();
    } else {
        // Outgoing call
        summary.direction = "out";
        summary.otherExtensionNumber = event["ConnectedLineNum"].toString();
    }

    // Determine call type based on extension numbers
    bool isInternal = summary.extensionNumber.length() < 9 && summary.otherExtensionNumber.length() < 9;
    summary.callType = isInternal ? "internal" : "external";

    // Log call ended event
    QJsonObject logEvent = callSummaryToJson(summary);
    logEvent["type"] = "call_ended";
    logEvent["timestamp"] = summary.endTime.toString(Qt::ISODate);
    logEvent["raw_event"] = event;  // Include raw event data
    logCallEvent(logEvent);

    // Log call summary
    QJsonObject summaryEvent = callSummaryToJson(summary);
    summaryEvent["type"] = "call_summary";
    summaryEvent["timestamp"] = summary.endTime.toString(Qt::ISODate);
    summaryEvent["call_duration"] = summary.startTime.secsTo(summary.endTime);
    summaryEvent["raw_event"] = event;  // Include raw event data
    
    if (summary.bridgeStartTime.isValid() && summary.bridgeEndTime.isValid()) {
        summaryEvent["bridge_duration"] = summary.bridgeStartTime.secsTo(summary.bridgeEndTime);
    }
    
    logCallEvent(summaryEvent);
    emit callEnded(channel, summaryEvent);

    // Store call summary in database
    storeCallSummary(summary);

    // Clean up call data
    m_activeCalls.remove(channel);
}

QJsonObject CallMonitor::callSummaryToJson(const CallSummary& summary)
{
    QJsonObject obj;
    obj["uniqueid"] = summary.uniqueId;
    obj["linkedid"] = summary.linkedId;
    obj["channel"] = summary.channel;
    obj["extension_number"] = summary.extensionNumber;
    obj["other_extension_number"] = summary.otherExtensionNumber;
    obj["direction"] = summary.direction;
    obj["call_type"] = summary.callType;
    obj["caller_id_num"] = summary.callerIdNum;
    obj["caller_id_name"] = summary.callerIdName;
    obj["connected_line_num"] = summary.connectedLineNum;
    obj["connected_line_name"] = summary.connectedLineName;
    obj["status"] = summary.status;
    obj["has_ringing"] = summary.hasRinging;
    obj["start_time"] = summary.startTime.toString(Qt::ISODate);
    obj["end_time"] = summary.endTime.toString(Qt::ISODate);

    if (summary.bridgeStartTime.isValid()) {
        obj["bridge_start_time"] = summary.bridgeStartTime.toString(Qt::ISODate);
    }
    if (summary.bridgeEndTime.isValid()) {
        obj["bridge_end_time"] = summary.bridgeEndTime.toString(Qt::ISODate);
    }
    if (!summary.bridgeUniqueId.isEmpty()) {
        obj["bridge_uniqueid"] = summary.bridgeUniqueId;
        obj["bridge_type"] = summary.bridgeType;
        obj["bridge_technology"] = summary.bridgeTechnology;
    }
    if (!summary.cause.isEmpty()) {
        obj["cause"] = summary.cause;
        obj["cause_description"] = summary.causeDescription;
        obj["cause_txt"] = summary.causeTxt;
    }

    return obj;
}

void CallMonitor::storeCallSummary(const CallSummary& summary)
{
    /*
    TODO: Implement database storage
    This method should store the call summary in your database.
    You can use the following data from the CallSummary struct:
    
    - uniqueId
    - linkedId
    - channel
    - extensionNumber
    - otherExtensionNumber
    - direction
    - callType
    - callerIdNum
    - callerIdName
    - connectedLineNum
    - connectedLineName
    - status
    - hasRinging
    - startTime
    - endTime
    - bridgeStartTime (if valid)
    - bridgeEndTime (if valid)
    - bridgeUniqueId (if not empty)
    - bridgeType (if not empty)
    - bridgeTechnology (if not empty)
    - cause
    - causeDescription
    - causeTxt
    */

    //check if the call summery has the status = answered
    if(summary.status == "answered"){
        myDbController->storeCallSummary(
            m_targetExtension,
            summary.startTime.toSecsSinceEpoch(),
            summary.endTime.toSecsSinceEpoch(),
            summary.startTime.secsTo(summary.endTime),
            summary.otherExtensionNumber,
            summary.direction,
            summary.callType,
            summary.status,
            summary.uniqueId
            );

        // emit signal to retrieve the file from apiserver
    }
    // TODO: suggestion to tack un awnsered calls












} 
