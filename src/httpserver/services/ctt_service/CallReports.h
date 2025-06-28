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

class ConversationMessageListAPI : public BaseApiService{
public:
    explicit ConversationMessageListAPI(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    const QString USER_CONVERSATION_ID_KEY   = QStringLiteral("conversationID");


    QJsonObject JSON_SCHEMA = {
        {JSON_TYPE,JSON_OBJECT},
        {JSON_PROPERTIES,QJsonObject{
                              {USER_CONVERSATION_ID_KEY,JSON_INTEGER_FIELD}
                          }},
        {JSON_REQUIRED,QJsonArray({USER_CONVERSATION_ID_KEY})}
    };
    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_POST_METHOD;
    const QByteArray PATH = "/conversation/messages";
};

class SearchUserConversationAPI : public BaseApiService{
public:
    explicit SearchUserConversationAPI(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    const QString USER_CONVERSATION_SEARCH_STRING_KEY   = QStringLiteral("searchString");


    QJsonObject JSON_SCHEMA = {
        {JSON_TYPE,JSON_OBJECT},
        {JSON_PROPERTIES,QJsonObject{
                              {USER_CONVERSATION_SEARCH_STRING_KEY,JSON_STRING_FIELD}
                          }},
        {JSON_REQUIRED,QJsonArray({USER_CONVERSATION_SEARCH_STRING_KEY})}
    };
    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_POST_METHOD;
    const QByteArray PATH = "/conversation/search";
};

class CreateFeedbackAPI : public BaseApiService{
public:
    explicit CreateFeedbackAPI(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    QJsonObject GenerateValidResponse();
    const QString MESSAGE_FEEDKBACK_RATE_KEY  = QStringLiteral("rate");
    const QString MESSAGE_FEEDKBACK_DESCRIPTION_KEY   = QStringLiteral("description");
    const QString MESSAGE_FEEDKBACK_aiMessageID_KEY   = QStringLiteral("message_id");


    QJsonObject JSON_SCHEMA = {
        {JSON_TYPE,JSON_OBJECT},
        {JSON_PROPERTIES,QJsonObject{
                                      {MESSAGE_FEEDKBACK_RATE_KEY,JSON_INTEGER_FIELD},
                                      {MESSAGE_FEEDKBACK_DESCRIPTION_KEY,JSON_STRING_FIELD},
                                      {MESSAGE_FEEDKBACK_aiMessageID_KEY,JSON_INTEGER_FIELD},
                                      }},
        {JSON_REQUIRED,QJsonArray({MESSAGE_FEEDKBACK_RATE_KEY,MESSAGE_FEEDKBACK_DESCRIPTION_KEY,
                                    MESSAGE_FEEDKBACK_aiMessageID_KEY})}
    };
    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_POST_METHOD;
    const QByteArray PATH = "/conversation/message/feedback";
};

class GetFeedBackAPI : public BaseApiService{
public:
    explicit GetFeedBackAPI(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    const QString MESSAGE_ID_KEY  = QStringLiteral("message_id");

    QJsonObject JSON_SCHEMA = {
        {JSON_TYPE,JSON_OBJECT},
        {JSON_PROPERTIES,QJsonObject{
                              {MESSAGE_ID_KEY,JSON_INTEGER_FIELD}
                          }},
        {JSON_REQUIRED,QJsonArray({MESSAGE_ID_KEY})}
    };
    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_POST_METHOD;
    const QByteArray PATH = "/conversation/message/feedbackInfo";
};

class GetAiMessageMetaDataAPI : public BaseApiService{
public:
    explicit GetAiMessageMetaDataAPI(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:
    const QString MESSAGE_ID_KEY  = QStringLiteral("message_id");

    QJsonObject JSON_SCHEMA = {
        {JSON_TYPE,JSON_OBJECT},
        {JSON_PROPERTIES,QJsonObject{
                              {MESSAGE_ID_KEY,JSON_INTEGER_FIELD}
                          }},
        {JSON_REQUIRED,QJsonArray({MESSAGE_ID_KEY})}
    };
    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_POST_METHOD;
    const QByteArray PATH = "/conversation/message/aiMetaData";
};

class ListUserBotsAndRagsApi : public BaseApiService{
public:
    explicit ListUserBotsAndRagsApi(QObject* parent = nullptr);

    void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa") override;

    ApiValRes validate(HttpRequest &request) override;

    QByteArray getPath(){return PATH;}
private:

    const int ACCESS_LEVEL = AccessLevel::NORMAL_USER;
    const QByteArray ALLOWED_METHOD = HTTP_GET_METHOD;
    const QByteArray PATH = "/conversation/bot-rag/list";
};



#endif // CALLREPORTS_H
