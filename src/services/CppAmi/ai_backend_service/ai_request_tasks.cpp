#include "ai_request_tasks.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QEventLoop>
#include <QJsonDocument>
#include <QTimer>
#include <QBuffer>
#include <QHttpPart>
#include <QElapsedTimer>  // [ENHANCED LOGGING]
#include "../../../global.h"
#include "../utils/ICTRequestUtils.h"

AiBackendService::CttPanelRequestTask::CttPanelRequestTask(AiBackendService::RequestPayload reqPayload, QObject *parent)
{
    m_request = reqPayload;
}

AiBackendService::CttPanelRequestTask::~CttPanelRequestTask() {}

void AiBackendService::CttPanelRequestTask::run()
{
    const int maxRetries = 3;
    const int retryDelayMs = 2000; // Base delay before retry (can be made exponential)

    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        qDebug() << QString("[DEBUG] Attempt %1 of %2").arg(attempt).arg(maxRetries);

        QNetworkAccessManager manager;
        QUrl url(myGlobalSettings->value("aibackend/requestUrl").toString());
        QNetworkRequest request(url);
        request.setRawHeader("accept", "application/json");

        IctApiUtils::FileData fileData = IctApiUtils::getAudioFileByUniqueId(m_request.unique_id);
        if (!fileData.status) {
            qWarning() << "[WARNING] Failed to fetch file. Error:" << fileData.error;
            myDbController->insertAiResponse(m_request.call_summary_id, QString::fromUtf8(QJsonDocument(fileData.error).toJson()));
            return;
        }

        QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QBuffer* buffer = new QBuffer();
        buffer->setData(fileData.binary);
        if (!buffer->open(QIODevice::ReadOnly)) {
            qCritical() << "[ERROR] Failed to open audio buffer.";
            delete buffer;
            delete multiPart;
            return;
        }

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant(QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileData.filename)));
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("audio/wav"));
        filePart.setBodyDevice(buffer);
        buffer->setParent(multiPart);
        multiPart->append(filePart);

        QNetworkReply* reply = manager.post(request, multiPart);
        multiPart->setParent(reply);

        QEventLoop loop;
        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        bool timedOut = false;
        int timeoutMillis = myGlobalSettings->value("aibackend/requestTimeout").toInt() * 60000;

        QElapsedTimer elapsedTimer;
        elapsedTimer.start();

        QObject::connect(&timeoutTimer, &QTimer::timeout, [&]() {
            timedOut = true;
            if (reply->isRunning()) {
                qWarning() << "[WARNING] Request timed out. Aborting...";
                reply->abort();
            }
            loop.quit();
        });

        timeoutTimer.start(timeoutMillis);
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        QObject::connect(reply, &QNetworkReply::errorOccurred, [&](QNetworkReply::NetworkError code) {
            qCritical() << "[ERROR] Network error occurred:" << code << "-" << reply->errorString();
        });

        loop.exec();  // Waits here for reply or timeout
        QByteArray aiResponse = reply->readAll();
        qint64 elapsed = elapsedTimer.elapsed();
        qDebug() << "[DEBUG] Request completed in" << elapsed << "ms.";

        // Handle timeout
        if (timedOut) {
            qCritical() << "[ERROR] Timed out. Retrying if attempts left...";
            reply->deleteLater();
            if (attempt < maxRetries)
                QThread::msleep(retryDelayMs * attempt);  // exponential backoff
            continue;
        }

        // Parse and validate response
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(aiResponse, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qCritical() << "[ERROR] Failed to parse JSON. Retrying...";
            reply->deleteLater();
            if (attempt < maxRetries)
                QThread::msleep(retryDelayMs * attempt);
            continue;
        }

        QJsonObject jsonResponse = jsonDoc.object();
        if (jsonResponse.contains("detail")) {
            QJsonObject errorObj;
            errorObj.insert("error", jsonResponse.value("detail").toString());
            myDbController->insertAiResponse(m_request.call_summary_id, QString::fromUtf8(QJsonDocument(errorObj).toJson()));
            qWarning() << "[WARNING] Server returned error: detail key present. Retrying...";
            reply->deleteLater();
            if (attempt < maxRetries)
                QThread::msleep(retryDelayMs * attempt);
            continue;
        }

        // Success
        myDbController->insertAiResponse(m_request.call_summary_id, QString::fromUtf8(jsonDoc.toJson()));
        qDebug() << "[DEBUG] Successful response inserted into DB.";
        reply->deleteLater();
        break; // Exit retry loop
    }

    qDebug() << "[DEBUG] Finished run() method.";
}




