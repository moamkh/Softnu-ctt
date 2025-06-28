#include "CallReports.h"

#include "../../../global.h"

#include "../../utils/api_response_utils.h"
#include "../../utils/json_validator.h"

MonitorableUsersListAPI::MonitorableUsersListAPI(QObject *parent):BaseApiService(parent)
{
    qInfo() << "Initializing monitorable user list";
    SwaggerGenerator::Endpoint m_endpoint;
    m_endpoint.path = "/{language}" + PATH;
    m_endpoint.method = ALLOWED_METHOD;
    m_endpoint.category = "Users";
    m_endpoint.requiresAuth=true;
    m_endpoint.description = "Lists the available users to be monitored by the logged in user";
    m_endpoint.parameters.append(SwaggerGenerator::getPathLanguageParameter());
    mySwaggerGenerator->addEndpoint(m_endpoint);
}

void MonitorableUsersListAPI::service(HttpRequest &request, HttpResponse &response, QByteArray lang)
{
    QMap <QByteArray,QByteArray> params = request.getParameterMap();
    qWarning() << params;

    QList<QByteArray> paramList = params.keys();
    int page ,pageSize;
    if(paramList.contains("page")){
        page = params.value("page").toInt();
        pageSize = params.value("page_size","20").toInt();
        if(page <1 || pageSize<1){
            ApiResponseUtils::SendJsonError(response,ApiErrorFactory::BadRequest());
            return;
        }
    }else{
        page = 1;
        pageSize = 20;
    }
    bool is_search_field_valid = ALLOWED_QUERY_PARAMS.contains(request.getParameter("search_field"));

    QByteArray searchField = is_search_field_valid ? request.getParameter("search_field"):"";
    QByteArray searchValue = is_search_field_valid ? params.value("search_value"):"";

    DbResault dbRes = myDbController->listUsers(searchField,searchValue,page,pageSize);

    if(!dbRes.status){
        ApiResponseUtils::SendJsonError(response,ApiErrorFactory::InternalServerError());
        return;
    }

    ApiResponseUtils::SendJsonResponse(response,dbRes.data);
    return;

}

ApiValRes MonitorableUsersListAPI::validate(HttpRequest &request)
{
    if(request.getMethod() != ALLOWED_METHOD){
        return ApiValRes(ApiErrorFactory::InvalidMethodError());
    }

    // if(!request.getHeaderMap().contains(HTTP_HEADER_AUTHORIZATION)){
    //     return ApiValRes(ApiErrorFactory::AuthorizationHeaderRequired());
    // }

    return ApiValRes();
}
