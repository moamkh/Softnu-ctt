#ifndef SWAGGER_DOCUMENTATION_H
#define SWAGGER_DOCUMENTATION_H
#include "base_api_service.h"
#include "../utils/jwt_manager.h"



class SwaggerDocApi : public BaseApiService{
public:
    explicit SwaggerDocApi(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    QJsonObject GenerateValidResponse();

    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;

    const QByteArray ALLOWED_METHOD = HTTP_GET_METHOD;

    const QByteArray PATH = "/docs";
};
#endif // SWAGGER_DOCUMENTATION_H
