#include "CallReports.h"

#include "../../../global.h"

#include "../../utils/api_response_utils.h"
#include "../../utils/json_validator.h"

#include "../../../services/CppAmi/utils/ICTRequestUtils.h"

using FileData = IctApiUtils::FileData;

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

ListUserCallSummariesApi::ListUserCallSummariesApi(QObject *parent)
{
    qInfo() << "Initializing list user call summaries";
    SwaggerGenerator::Endpoint m_endpoint;
    m_endpoint.path = "/{language}" + PATH;
    m_endpoint.method = ALLOWED_METHOD;
    m_endpoint.category = "Users";
    m_endpoint.requiresAuth=true;
    m_endpoint.description = "Lists the available users to be monitored by the logged in user";
    m_endpoint.parameters.append(SwaggerGenerator::getPathLanguageParameter());
    mySwaggerGenerator->addEndpoint(m_endpoint);
}

void ListUserCallSummariesApi::service(HttpRequest &request, HttpResponse &response, QByteArray lang)
{

}

ApiValRes ListUserCallSummariesApi::validate(HttpRequest &request)
{
    if(request.getMethod()!=ALLOWED_METHOD){
        return ApiValRes(ApiErrorFactory::InvalidMethodError());
    }

    QJsonObject _jsonReq = QJsonDocument::fromJson(request.getBody()).object();

    JsonValidator _jsonValidator(JSON_SCHEMA);
    ApiValRes _valRes = _jsonValidator.validate(_jsonReq);
    if(!_valRes.isValid){
        return ApiValRes(_valRes.apiErrorObj);
    }
    // check if conversation belongs to user
    qint64 userID = _jsonReq.value(USER_ID_KEY).toInt();

    if(!myDbController->checkUserExists(userID)){
        return ApiValRes(ApiErrorFactory::BadRequest());
    }

    return ApiValRes();
}

DownloadUserConversationApi::DownloadUserConversationApi(QObject *parent)
{
    qInfo()<<"Initializing Download user conversation audio";
    SwaggerGenerator::Endpoint m_endpoint;
    m_endpoint.path = "/{language}" + PATH;
    m_endpoint.method = ALLOWED_METHOD;
    m_endpoint.category = "Users";
    m_endpoint.requiresAuth=true;
    m_endpoint.description = "Downloads audio file using the provided call summary id.";
    m_endpoint.parameters.append(SwaggerGenerator::getPathLanguageParameter());
    mySwaggerGenerator->addEndpoint(m_endpoint);
}

void DownloadUserConversationApi::service(HttpRequest &request, HttpResponse &response, QByteArray lang)
{
    QJsonObject jsonReq = QJsonDocument::fromJson(request.getBody()).object();
    qint64 call_id = jsonReq.value(call_summary_id).toInt();
    DbResault dbRes = myDbController->getCallSummaryById(call_id);
    if(!dbRes.status){
        ApiResponseUtils::SendJsonError(response,ApiErrorFactory::InternalServerError());
        return;
    }

    QString unique_id =dbRes.data.value("unique_id").toString();
    FileData fileData = IctApiUtils::getAudioFileByUniqueId(unique_id);
    if (!fileData.status) {
        qWarning() << "[WARNING] Failed to fetch file. Error:" << fileData.error;
        ApiResponseUtils::SendJsonError(response,ApiErrorFactory::FailedToDownloadAudioFile());
        return;
    }

    ApiResponseUtils::SendAudioFile(response,fileData.binary);
}

ApiValRes DownloadUserConversationApi::validate(HttpRequest &request)
{
    if(request.getMethod()!=ALLOWED_METHOD){
        return ApiValRes(ApiErrorFactory::InvalidMethodError());
    }

    QJsonObject _jsonReq = QJsonDocument::fromJson(request.getBody()).object();

    JsonValidator _jsonValidator(JSON_SCHEMA);
    ApiValRes _valRes = _jsonValidator.validate(_jsonReq);
    if(!_valRes.isValid){
        return ApiValRes(_valRes.apiErrorObj);
    }
    // check if conversation belongs to user
    qint64 call_id = _jsonReq.value(call_summary_id).toInt();

    if(!myDbController->checkCallSummaryExists(call_id)){
        return ApiValRes(ApiErrorFactory::BadRequest());
    }

    return ApiValRes();
}
