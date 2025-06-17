#include "api_response_utils.h"

#include <QJsonDocument>

void ApiResponseUtils::SendJsonResponse(
        stefanfrings::HttpResponse &response,
        QJsonObject jsonObject,
        int status_code)
{
    response.setStatus(status_code);
    response.setHeader("Content-Type","application/json");
    response.write(CreateJsonByteArray(jsonObject),true);
}

void ApiResponseUtils::SendJsonError(
        stefanfrings::HttpResponse &response,
        QString message,
        int status_code)
{
    response.setStatus(status_code);
    response.setHeader("Content-Type","application/json");
    QJsonObject jsonObject;
    jsonObject.insert("error",message);
    response.write(CreateJsonByteArray(jsonObject),true);
}

void ApiResponseUtils::SendJsonError(
        stefanfrings::HttpResponse &response,
        BaseApiError errorObject,
        QString lang)
{
    response.setStatus(
                errorObject.get_status_code(),
                errorObject.get_message_english().toUtf8());
    response.setHeader("Content-Type","application/json");
    QJsonObject jsonObject;
    jsonObject.insert("error",errorObject.get_message_by_language(lang));
    response.write(CreateJsonByteArray(jsonObject),true);
}

QByteArray ApiResponseUtils::CreateJsonByteArray(QJsonObject &object)
{
    // Create a QJsonDocument from the QJsonObject
    QJsonDocument jsonDoc(object);
    // Convert the QJsonDocument to a compact JSON and return it as QByteArray
    return jsonDoc.toJson(QJsonDocument::Compact);
}
