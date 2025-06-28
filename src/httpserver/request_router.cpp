#include "request_router.h"

#include <QList>
#include "utils/api_response_utils.h"
#include "errors/api_error_factory.h"
#include "../global.h"


ApiRequestMapper::ApiRequestMapper(QObject *parent):
    HttpRequestHandler (parent)
{
    // get the supported languages

    SUPPORTED_LANGUAGES = myGlobalSettings->value("config/langs").toByteArray().split(',');
    SUPPORTED_LANGUAGES.removeAll("");
    qInfo() << "Initializing services.";

    // swagger doc apis
    SwaggerDocService = new SwaggerDocApi();
    MonitorableUsersListService = new MonitorableUsersListAPI();
    qInfo() << "Initializing services Done.";

    MapServices();
}

void ApiRequestMapper::MapServices()
{
    qInfo() << "Mapping services.";


    // swagger documentation service
    m_service_map.insert(SwaggerDocService->getPath(),SwaggerDocService);
    m_service_map.insert(MonitorableUsersListService->getPath(),MonitorableUsersListService);


    qInfo() << "Done. Mapping services.";
}

void ApiRequestMapper::SetCorsHeaders(stefanfrings::HttpResponse &response)
{
#ifdef QT_DEBUG
    qDebug() << "Setting Default Cors Headers.";
#endif
    response.setHeader("Access-Control-Allow-Origin","*");
    response.setHeader("Access-Control-Allow-Methods","GET, POST, OPTIONS, PUT, DELETE");
    response.setHeader("Access-Control-Allow-Headers","*");
}

void ApiRequestMapper::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray path = request.getPath();

    const QByteArray method = request.getMethod();

    SetCorsHeaders(response);
    if (method==HTTP_OPTIONS_METHOD){
        response.setStatus(204,"No Content");
        response.write("",true);
        return;
    }

    // Split the path into parts
    QList<QByteArray> urlWords = path.split('/');
    urlWords.removeAll(""); // Remove empty elements caused by leading/trailing slashes

    qWarning() << "URL words:" << urlWords;
    qWarning() << "Suported langs: " << SUPPORTED_LANGUAGES;
    // Extract the language
    QByteArray lang = urlWords.isEmpty() ? "" : urlWords.takeFirst(); // Take and remove the first element

    // Check if the language is supported
    if (!SUPPORTED_LANGUAGES.contains(lang)) {
        lang = "fa"; // Default to "fa" if unsupported
    }

    // Reconstruct the path without the language segment
    path = "/" + urlWords.join('/');

    qDebug() << "Language    :" << lang;
    qDebug() << "Updated path:" << path;
    qDebug() << "Method      :" << method;

    if(m_service_map.contains(path))
    {
        BaseApiService* matched_service = m_service_map[path];
        QString errorString;
        ApiValRes _valRes = matched_service->validate(request);
        if (!_valRes.isValid){
            ApiResponseUtils::SendJsonError(response,_valRes.apiErrorObj,lang);
            return;
        }
        matched_service->service(request,response,lang);
        return;
    }
    else
    {
        ApiResponseUtils::SendJsonError(response,ApiErrorFactory::PathNotFoundError());
        return;
    }
}


