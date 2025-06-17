#ifndef AMITYPES_HPP
#define AMITYPES_HPP

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class AmiEvent {
public:
    virtual ~AmiEvent() = default;
    virtual QString eventType() const = 0;
    virtual QString privilege() const = 0;
    virtual QDateTime timestamp() const = 0;
    virtual QString channel() const = 0;
    virtual QJsonObject toJson() const = 0;
};

class NewChannelEvent : public AmiEvent {
public:
    NewChannelEvent(const QJsonObject& data);
    QString eventType() const override { return "Newchannel"; }
    QString privilege() const override { return "call,all"; }
    QDateTime timestamp() const override { return m_timestamp; }
    QString channel() const override { return m_channel; }
    QString callerIdNum() const { return m_callerIdNum; }
    QString callerIdName() const { return m_callerIdName; }
    QString connectedLineNum() const { return m_connectedLineNum; }
    QString connectedLineName() const { return m_connectedLineName; }
    QJsonObject toJson() const override;

private:
    QDateTime m_timestamp;
    QString m_channel;
    QString m_callerIdNum;
    QString m_callerIdName;
    QString m_connectedLineNum;
    QString m_connectedLineName;
};

class NewStateEvent : public AmiEvent {
public:
    NewStateEvent(const QJsonObject& data);
    QString eventType() const override { return "Newstate"; }
    QString privilege() const override { return "call,all"; }
    QDateTime timestamp() const override { return m_timestamp; }
    QString channel() const override { return m_channel; }
    QString channelState() const { return m_channelState; }
    QJsonObject toJson() const override;

private:
    QDateTime m_timestamp;
    QString m_channel;
    QString m_channelState;
};

class BridgeEvent : public AmiEvent {
public:
    BridgeEvent(const QJsonObject& data);
    QString eventType() const override { return "Bridge"; }
    QString privilege() const override { return "call,all"; }
    QDateTime timestamp() const override { return m_timestamp; }
    QString channel() const override { return m_channel; }
    QString bridgeState() const { return m_bridgeState; }
    QJsonObject toJson() const override;

private:
    QDateTime m_timestamp;
    QString m_channel;
    QString m_bridgeState;
};

class HangupEvent : public AmiEvent {
public:
    HangupEvent(const QJsonObject& data);
    QString eventType() const override { return "Hangup"; }
    QString privilege() const override { return "call,all"; }
    QDateTime timestamp() const override { return m_timestamp; }
    QString channel() const override { return m_channel; }
    QString cause() const { return m_cause; }
    QString causeDescription() const { return m_causeDescription; }
    QString causeTxt() const { return m_causeTxt; }
    QString callerIdNum() const { return m_callerIdNum; }
    QString connectedLineNum() const { return m_connectedLineNum; }
    QJsonObject toJson() const override;

private:
    QDateTime m_timestamp;
    QString m_channel;
    QString m_cause;
    QString m_causeDescription;
    QString m_causeTxt;
    QString m_callerIdNum;
    QString m_connectedLineNum;
};

#endif // AMITYPES_HPP 