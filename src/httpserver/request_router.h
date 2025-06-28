#ifndef REQUEST_ROUTER_H
#define REQUEST_ROUTER_H


#include "httprequesthandler.h"
#include <QMap>
#include <QObject>
#include "services/base_api_service.h"
#include "services/swagger_documentation.h"
#include "services/ctt_service/CallReports.h"

using stefanfrings::HttpRequest;
using stefanfrings::HttpResponse;
using stefanfrings::HttpRequestHandler;

class ApiRequestMapper : public HttpRequestHandler {
    Q_OBJECT
public:
    ApiRequestMapper(QObject* parent = nullptr);
    void service (HttpRequest& request,HttpResponse& response);

private:
    //function to initialize the QMap
    void MapServices();
    void SetCorsHeaders(stefanfrings::HttpResponse &response);

    // Map url to services
    QMap<QByteArray,BaseApiService*> m_service_map;

    // Service instances

    // swagger documentation apis
    SwaggerDocApi* SwaggerDocService;
    MonitorableUsersListAPI* MonitorableUsersListService;



    // supported language container initialized with ini file
    QList<QByteArray> SUPPORTED_LANGUAGES;
};



#endif
