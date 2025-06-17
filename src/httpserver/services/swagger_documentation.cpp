#include "swagger_documentation.h"

#include "../../global.h"
#include "../utils/api_response_utils.h"
#include "QDebug"

SwaggerDocApi::SwaggerDocApi(QObject *parent)
{
    qInfo() << "Initializing swagger docs api";
}

void SwaggerDocApi::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response, QByteArray lang)
{
    QByteArray fileType = request.getParameter("file");

    if(fileType.isEmpty()){
        QString path = myGlobalSettings->value("static/swaggerDocsHtml").toString();
        myStaticFileController->service(request,response,path);
        return;
    }
    else if(fileType =="css"){
        QString path = myGlobalSettings->value("static/swaggerDocsCss").toString();
        myStaticFileController->service(request,response,path);
        return;
    }
    else if(fileType =="json"){
        QString path = myGlobalSettings->value("static/swaggerDocsJson").toString();
        myStaticFileController->service(request,response,path);
        return;
    }
    else if(fileType =="js"){
        QString path = myGlobalSettings->value("static/swaggerDocsJs").toString();
        myStaticFileController->service(request,response,path);
        return;
    }
    else{
        ApiResponseUtils::SendJsonError(response,ApiErrorFactory::BadRequest());
        return;
    }


}

ApiValRes SwaggerDocApi::validate(stefanfrings::HttpRequest &request)
{
    if(request.getMethod()!=ALLOWED_METHOD){
        return ApiValRes(ApiErrorFactory::InvalidMethodError());
    }

    return ApiValRes();
}
