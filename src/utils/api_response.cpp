#include "api_response.h"
#include <QJsonDocument>




QByteArray FalseResponse(QString errorMessage){
    QJsonObject jsonResponse;
    jsonResponse["error"]=errorMessage;
    QJsonDocument jsonDoc(jsonResponse);
    return jsonDoc.toJson();
}

QByteArray JsonResponse(QJsonObject jsonResponse){
    QJsonDocument jsonDoc(jsonResponse);
    return jsonDoc.toJson();
}

QJsonObject doneWsResponse(
        const qint64&       userMessageID,
        const qint64&       aiMessageID,
        const QJsonObject&  aiMetaData){
    QJsonObject conversation;
    QJsonObject doneResponse;
    conversation["body"]="";
    conversation["type"]=0;
    conversation["done"]=true;
    conversation["userMessageID"]=userMessageID;
    conversation["aiMessageID"]=aiMessageID;
    doneResponse["conversation"]=conversation;
    doneResponse["aiMetaData"]=aiMetaData;
    return doneResponse;
}







