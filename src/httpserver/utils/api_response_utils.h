#ifndef API_RESPONSE_UTILS_H
#define API_RESPONSE_UTILS_H

#include "httpresponse.h"
#include "../errors/base_api_error.h"
#include <QJsonObject>

using stefanfrings::HttpResponse;

class ApiResponseUtils
{
public:
    static void SendJsonResponse(
            HttpResponse& response,
            QJsonObject  jsonObject,
            int status_code = 200
            );

    static void SendJsonError(
            HttpResponse& response,
            QString message = "Internal server error.",
            int status_code = 500
            );
    static void SendJsonError(
            HttpResponse& response,
            BaseApiError errorObject,
            QString lang = "fa"
            );
    static void SendAudioFile(
            HttpResponse& response,
            QByteArray& audioBinary,
            int status_code =200
            );
private:
    static QByteArray CreateJsonByteArray(QJsonObject& object);

};



#endif
