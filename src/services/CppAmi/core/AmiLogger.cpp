#include "AmiLogger.h"
#include <QDir>
#include <QDebug>

AmiLogger* AmiLogger::m_instance = nullptr;

AmiLogger& AmiLogger::instance()
{
    if (!m_instance) {
        m_instance = new AmiLogger();
    }
    return *m_instance;
}

AmiLogger::AmiLogger(QObject* parent)
    : QObject(parent)
{
    // Create logs directory if it doesn't exist
    QDir logDir(QDir::currentPath() + "/logs");
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }

    // Open log file
    m_logFile.setFileName(logDir.filePath("ami_events.log"));
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

AmiLogger::~AmiLogger()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void AmiLogger::info(const QString& message, const QJsonObject& data)
{
    log("INFO", message, data);
}

void AmiLogger::error(const QString& message, const QJsonObject& data)
{
    log("ERROR", message, data);
}

void AmiLogger::debug(const QString& message, const QJsonObject& data)
{
    log("DEBUG", message, data);
}

void AmiLogger::log(const QString& level, const QString& message, const QJsonObject& data)
{
    QJsonObject logObj;
    logObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    logObj["level"] = level;
    logObj["message"] = message;
    
    if (!data.isEmpty()) {
        logObj["data"] = data;
        // Add raw event data for terminal output
        if (data.contains("raw_event")) {
            logObj["raw_event"] = data["raw_event"];
        }
    }

    QString logMessage = QJsonDocument(logObj).toJson(QJsonDocument::Compact);
    
    // Write to file (without raw event data)
    if (m_logFile.isOpen()) {
        QJsonObject fileLogObj = logObj;
        fileLogObj.remove("raw_event");
        QString fileLogMessage = QJsonDocument(fileLogObj).toJson(QJsonDocument::Compact);
        m_logFile.write(fileLogMessage.toUtf8() + "\n");
        m_logFile.flush();
    }

    // Output to console with colors
    QString colorCode;
    if (level == "ERROR") {
        colorCode = "\033[31m"; // Red
    } else if (level == "INFO") {
        colorCode = "\033[32m"; // Green
    } else if (level == "DEBUG") {
        colorCode = "\033[36m"; // Cyan
    }

    // Print with color and reset at the end
    qDebug().noquote() << colorCode << logMessage << "\033[0m";
}

QString AmiLogger::formatLogMessage(const QString& level, const QString& message, const QJsonObject& data)
{
    QJsonObject logObj;
    logObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    logObj["level"] = level;
    logObj["message"] = message;
    
    if (!data.isEmpty()) {
        logObj["data"] = data;
    }

    return QJsonDocument(logObj).toJson(QJsonDocument::Compact);
} 
