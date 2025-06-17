#ifndef AMILOGGER_HPP
#define AMILOGGER_HPP

#include <QObject>
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>

class AmiLogger : public QObject {
    Q_OBJECT

public:
    static AmiLogger& instance();

    void info(const QString& message, const QJsonObject& data = QJsonObject());
    void error(const QString& message, const QJsonObject& data = QJsonObject());
    void debug(const QString& message, const QJsonObject& data = QJsonObject());

private:
    explicit AmiLogger(QObject* parent = nullptr);
    ~AmiLogger();

    void log(const QString& level, const QString& message, const QJsonObject& data);
    QString formatLogMessage(const QString& level, const QString& message, const QJsonObject& data);

    QFile m_logFile;
    static AmiLogger* m_instance;
};

#endif // AMILOGGER_HPP 