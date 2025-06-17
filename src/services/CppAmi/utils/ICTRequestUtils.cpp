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
    QNetworkRequest request(QUrl("https://apiserver.novinmed.com/SoftNo/getvalidextensions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ..."); // Replace with the actual token
    
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
