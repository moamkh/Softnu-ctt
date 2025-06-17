#include "AmiTypes.h"
#include <QDebug>

NewChannelEvent::NewChannelEvent(const QJsonObject& data)
{
    m_timestamp = QDateTime::currentDateTime();
    m_channel = data["Channel"].toString();
    m_callerIdNum = data["CallerIDNum"].toString();
    m_callerIdName = data["CallerIDName"].toString();
    m_connectedLineNum = data["ConnectedLineNum"].toString();
    m_connectedLineName = data["ConnectedLineName"].toString();
}

QJsonObject NewChannelEvent::toJson() const
{
    QJsonObject obj;
    obj["Event"] = eventType();
    obj["Channel"] = m_channel;
    obj["CallerIDNum"] = m_callerIdNum;
    obj["CallerIDName"] = m_callerIdName;
    obj["ConnectedLineNum"] = m_connectedLineNum;
    obj["ConnectedLineName"] = m_connectedLineName;
    obj["Timestamp"] = m_timestamp.toString(Qt::ISODate);
    return obj;
}

NewStateEvent::NewStateEvent(const QJsonObject& data)
{
    m_timestamp = QDateTime::currentDateTime();
    m_channel = data["Channel"].toString();
    m_channelState = data["ChannelState"].toString();
}

QJsonObject NewStateEvent::toJson() const
{
    QJsonObject obj;
    obj["Event"] = eventType();
    obj["Channel"] = m_channel;
    obj["ChannelState"] = m_channelState;
    obj["Timestamp"] = m_timestamp.toString(Qt::ISODate);
    return obj;
}

BridgeEvent::BridgeEvent(const QJsonObject& data)
{
    m_timestamp = QDateTime::currentDateTime();
    m_channel = data["Channel"].toString();
    m_bridgeState = data["BridgeState"].toString();
}

QJsonObject BridgeEvent::toJson() const
{
    QJsonObject obj;
    obj["Event"] = eventType();
    obj["Channel"] = m_channel;
    obj["BridgeState"] = m_bridgeState;
    obj["Timestamp"] = m_timestamp.toString(Qt::ISODate);
    return obj;
}

HangupEvent::HangupEvent(const QJsonObject& data)
{
    m_timestamp = QDateTime::currentDateTime();
    m_channel = data["Channel"].toString();
    m_cause = data["Cause"].toString();
    m_causeDescription = data["CauseDescription"].toString();
    m_causeTxt = data["CauseTxt"].toString();
    m_callerIdNum = data["CallerIDNum"].toString();
    m_connectedLineNum = data["ConnectedLineNum"].toString();
}

QJsonObject HangupEvent::toJson() const
{
    QJsonObject obj;
    obj["Event"] = eventType();
    obj["Channel"] = m_channel;
    obj["Cause"] = m_cause;
    obj["CauseDescription"] = m_causeDescription;
    obj["CauseTxt"] = m_causeTxt;
    obj["CallerIDNum"] = m_callerIdNum;
    obj["ConnectedLineNum"] = m_connectedLineNum;
    obj["Timestamp"] = m_timestamp.toString(Qt::ISODate);
    return obj;
} 
