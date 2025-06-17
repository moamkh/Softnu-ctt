#include "base_api_service.h"

#include "../utils/api_response_utils.h"
#include "../errors/api_error_factory.h"
BaseApiService::BaseApiService(QObject *parent)
{

}

void BaseApiService::service(
        stefanfrings::HttpRequest &request,
        stefanfrings::HttpResponse &response,
        QByteArray lang
        )
{
    ApiResponseUtils::SendJsonError(response,ApiErrorFactory::NotImplementedError());
}

ApiValRes BaseApiService::validate(stefanfrings::HttpRequest &request)
{
    return ApiValRes();
}
