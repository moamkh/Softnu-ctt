#ifndef CALLREPORTS_H
#define CALLREPORTS_H

#include "../base_api_service.h"

#include "../../utils/json_validator.h"
#include "../../utils/jwt_manager.h"

#include <QList>

class MonitorableUsersListAPI : public BaseApiService{
public:
    explicit MonitorableUsersListAPI(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:

    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_GET_METHOD;
    const QByteArray PATH = "/users";
    const QList<QByteArray> ALLOWED_QUERY_PARAMS = {"all","fullname","extension_number"};
};

class ListUserCallSummariesApi : public BaseApiService{
public:
    explicit ListUserCallSummariesApi(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    const QString USER_ID_KEY  = QStringLiteral("user_id");

    QJsonObject JSON_SCHEMA = {
        {JSON_TYPE,JSON_OBJECT},
        {JSON_PROPERTIES,QJsonObject{
                              {USER_ID_KEY,JSON_STRING_FIELD}
                          }},
        {JSON_REQUIRED,QJsonArray({USER_ID_KEY})}
    };
    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_POST_METHOD;
    const QByteArray PATH = "/api/users/call-summaries";
};

class DownloadUserConversationApi : public BaseApiService{
public:
    explicit DownloadUserConversationApi(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    const QString call_summary_id  = QStringLiteral("call_summary_id");

    QJsonObject JSON_SCHEMA = {
        {JSON_TYPE,JSON_OBJECT},
        {JSON_PROPERTIES,QJsonObject{
                              {call_summary_id,JSON_INTEGER_FIELD}
                          }},
        {JSON_REQUIRED,QJsonArray({call_summary_id})}
    };
    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_POST_METHOD;
    const QByteArray PATH = "/api/users/call-summaries/download";
};





#endif // CALLREPORTS_H
