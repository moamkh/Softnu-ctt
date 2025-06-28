#include "ICTRequestUtils.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QByteArray>
#include <QDebug>

#include "../../../global.h"

QJsonArray IctApiUtils::getAllowedExtentions(){
    /*
     * example date return
     * [
        {
          "fullName": "سارا محمدي",
          "userName": "s.mohammadi",
          "extesion": 264
        },
        {
          "fullName": "زهرا رباني",
          "userName": "rabani",
          "extesion": 265
        }
       ]
     */
    ApiServer apiServer = getApiServerCreds();
    QByteArray responseData;
    QEventLoop loop;
    
    // Setup QNetworkAccessManager
    QNetworkAccessManager manager;
    
#ifdef QT_DEBUG
    qDebug() << "Initializing QNetworkAccessManager.";
#endif
    
    QObject::connect(&manager, &QNetworkAccessManager::finished, [&loop, &responseData](QNetworkReply *reply) {
#ifdef QT_DEBUG
        qDebug() << "Network request finished.";
#endif
        
        if (reply->error() == QNetworkReply::NoError) {
#ifdef QT_DEBUG
            qDebug() << "Request successful. Reading response data.";
#endif
            responseData = reply->readAll();
        } else {
#ifdef QT_DEBUG
            qDebug() << "Error occurred: " << reply->errorString();
#endif
        }
        reply->deleteLater();
        loop.quit();
    });
    
    // Create the request
    QNetworkRequest request(QUrl(apiServer.url+"/getvalidextensions"));
    QByteArray tokenValue = apiServer.tokenPrefix.toUtf8() +" "+apiServer.token.toUtf8();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization",tokenValue ); // Replace with the actual token
    
#ifdef QT_DEBUG
    qDebug() << "Sending GET request to URL: " << request.url();
#endif
    
    // Send the request
    manager.get(request);
    
    // Wait for the response
    loop.exec();
    
#ifdef QT_DEBUG
    qDebug() << "Event loop exited. Returning response data.";
#endif
    
    QJsonObject valid_extentions = QJsonDocument::fromJson(responseData).object();
    
    if(valid_extentions.isEmpty() || valid_extentions.value("statusCode").toInt()!=200){
        return QJsonArray{};
    }
    
    return valid_extentions.value("data").toArray();
    
}

QByteArray IctApiUtils::getLastAudioFile(QString extNum, qint64 epocDate) {
    QNetworkAccessManager networkManager;

    // Convert epoch to ISO8601 format
    QString isoDate = QDateTime::fromSecsSinceEpoch(epocDate).toUTC().toString(Qt::ISODate) + ".000Z";

    // API URL and payload
    ApiServer apiServer = getApiServerCreds();
    QJsonObject payload;
    payload["extension"] = extNum;
    payload["baseDate"] = isoDate;

    // Prepare the request
    QNetworkRequest request(QUrl(apiServer.url+"/getvalidextensions"));
    QByteArray tokenValue = apiServer.tokenPrefix.toUtf8() + " " + apiServer.token.toUtf8();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("Authorization", tokenValue);

    // Logging the payload
#ifdef QT_DEBUG
    qWarning() << "Sending request to:" << apiServer.url+"/getvalidextensions";
    qWarning() << "Payload:" << QJsonDocument(payload).toJson(QJsonDocument::Indented);
#endif

    // Create an event loop to wait for the response
    QEventLoop eventLoop;
    QNetworkReply *reply = networkManager.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(); // Wait for the request to complete

    QByteArray responseData;
    if (reply->error() == QNetworkReply::NoError) {
        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

        if (!contentType.contains("application/json", Qt::CaseInsensitive)) {
            responseData = reply->readAll(); // Binary data (e.g., audio file)

            // Extract filename from content disposition
            QString filename = "unknown.wav";
            QString contentDisposition = reply->rawHeader("Content-Disposition");
            if (contentDisposition.contains("filename=")) {
                QRegularExpression regex("filename=\"?([^\"\;]+)\"?)");
                QRegularExpressionMatch match = regex.match(contentDisposition);
                if (match.hasMatch()) {
                    filename = match.captured(1);
                }
            }

#ifdef QT_DEBUG
            qWarning() << "Received audio file:";
            qWarning() << "File type: .wav";
            qWarning() << "Filename:" << filename;
#endif
        } else {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            responseData = jsonResponse.toJson(QJsonDocument::Compact);

#ifdef QT_DEBUG
            qWarning() << "Received JSON response:";
            qWarning() << jsonResponse.toJson(QJsonDocument::Indented);
#endif
        }
    } else {
#ifdef QT_DEBUG
        qWarning() << "Error occurred while making the request:";
        qWarning() << reply->errorString();
#endif
    }

    reply->deleteLater();
    return responseData;
}

QByteArray IctApiUtils::getAudioFileByUniqueId(QString unique_id)
{
    ApiServer apiServer = getApiServerCreds();
    QByteArray responseData;
    QEventLoop loop;

    // Setup QNetworkAccessManager
    QNetworkAccessManager manager;

#ifdef QT_DEBUG
    qDebug() << "Initializing QNetworkAccessManager.";
#endif

    QObject::connect(&manager, &QNetworkAccessManager::finished, [&loop, &responseData](QNetworkReply *reply) {
#ifdef QT_DEBUG
        qDebug() << "Network request finished.";
#endif

        if (reply->error() == QNetworkReply::NoError) {
#ifdef QT_DEBUG
            qDebug() << "Request successful. Reading response data.";
#endif
            responseData = reply->readAll();
        } else {
#ifdef QT_DEBUG
            qDebug() << "Error occurred: " << reply->errorString();
#endif
        }
        reply->deleteLater();
        loop.quit();
    });

    // Create the request
    QNetworkRequest request(QUrl(apiServer.url + "/downloadbyuniqueid"));
    QByteArray tokenValue = apiServer.tokenPrefix.toUtf8() + " " + apiServer.token.toUtf8();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", tokenValue);

    // Prepare the payload
    QJsonObject payload;
    payload.insert("unique_id", unique_id);

    // Convert payload to JSON
    QByteArray jsonData = QJsonDocument(payload).toJson();

#ifdef QT_DEBUG
    qDebug() << "Sending POST request to URL: " << request.url();
    qDebug() << "Payload: " << jsonData;
#endif

    // Send the request
    manager.post(request, jsonData);

    // Wait for the response
    loop.exec();

#ifdef QT_DEBUG
    qDebug() << "Event loop exited. Processing response data.";
#endif

    // Check if the response data is valid
    if (responseData.isEmpty()) {
#ifdef QT_DEBUG
        qDebug() << "No response data received.";
#endif
        return QByteArray();
    }

    // Check if the response is JSON
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError) {
        // If it's JSON, log the error and return an empty QByteArray
        QJsonObject responseObject = jsonDoc.object();
#ifdef QT_DEBUG
        qDebug() << "Error response received: " << responseObject;
#endif
        qWarning() << "Request failed. Response: " << QString(responseData);
        return QByteArray();
    }

// If the response is not JSON, assume it's the WAV file
#ifdef QT_DEBUG
    qDebug() << "Valid WAV file received.";
#endif

    return responseData;
}



QJsonArray IctApiUtils::getTodaysUserAudioFiles(QString extNum)
{
    ApiServer apiServer = getApiServerCreds();
    QByteArray responseData;
    QEventLoop loop;

    // Setup QNetworkAccessManager
    QNetworkAccessManager manager;

#ifdef QT_DEBUG
    qDebug() << "Initializing QNetworkAccessManager.";
#endif

    QObject::connect(&manager, &QNetworkAccessManager::finished, [&loop, &responseData](QNetworkReply *reply) {
#ifdef QT_DEBUG
        qDebug() << "Network request finished.";
#endif

        if (reply->error() == QNetworkReply::NoError) {
#ifdef QT_DEBUG
            qDebug() << "Request successful. Reading response data.";
#endif
            responseData = reply->readAll();
        } else {
#ifdef QT_DEBUG
            qDebug() << "Error occurred: " << reply->errorString();
#endif
        }
        reply->deleteLater();
        loop.quit();
    });

    // Create the request
    QNetworkRequest request(QUrl(apiServer.url + "/getvoicefiles"));
    QByteArray tokenValue = apiServer.tokenPrefix.toUtf8() + " " + apiServer.token.toUtf8();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", tokenValue);

    // Prepare the payload
    QJsonObject payload;
    QString currentDate = QDate::currentDate().toString("yyyy/MM/dd");
    payload.insert("extension", extNum);
    payload.insert("startDate", currentDate);
    payload.insert("endDate", currentDate);

    // Convert payload to JSON
    QByteArray jsonData = QJsonDocument(payload).toJson();

#ifdef QT_DEBUG
    qDebug() << "Sending POST request to URL: " << request.url();
    qDebug() << "Payload: " << jsonData;
#endif

    // Send the request
    manager.post(request, jsonData);

    // Wait for the response
    loop.exec();

#ifdef QT_DEBUG
    qDebug() << "Event loop exited. Processing response data.";
#endif

    // Parse the response
    QJsonObject responseJson = QJsonDocument::fromJson(responseData).object();

    if (responseJson.isEmpty() || responseJson.value("statusCode").toInt() != 200) {
#ifdef QT_DEBUG
        qDebug() << "Invalid response or error status code.";
#endif
        return QJsonArray{};
    }

    QJsonArray files = responseJson.value("data").toObject().value("files").toArray();
    QJsonArray fileNames;

    for (const QJsonValue &file : files) {
        QJsonObject data;
        QJsonObject fileObj = file.toObject();

        QString filename = fileObj.value("fileName").toString();
        QString uniqueId = filename.split('-').last().remove(".wav");

        data.insert("file_name", filename);
        data.insert("unique_id", uniqueId);

        fileNames.append(data);
    }

#ifdef QT_DEBUG
    qDebug() << "Extracted file data: " << QJsonDocument(fileNames).toJson();
#endif

    return fileNames;
}



IctApiUtils::ApiServer IctApiUtils::getApiServerCreds(){

    QString apiServerUrl = myGlobalSettings->value("ICT/base_url").toString();
    QString apiServerToken = myGlobalSettings->value("ICT/token").toString();
    QString apiServerTokenPrefix = myGlobalSettings->value("ICT/token_prefix").toString();
    if(apiServerUrl.isEmpty() || apiServerToken.isEmpty() || apiServerTokenPrefix.isEmpty()){
        qWarning() <<myGlobalSettings->allKeys();
        qFatal("ICT AMI server credentials are empty");
    }
    return ApiServer(apiServerUrl,apiServerToken,apiServerTokenPrefix);
}

IctApiUtils::ApiServer::ApiServer(QString apiServerUrl, QString apiServerToken, QString apiServerTokenPrefix):
    url(apiServerUrl),token(apiServerToken)
    ,tokenPrefix(apiServerTokenPrefix){}
