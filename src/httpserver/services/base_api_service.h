#ifndef BASE_API_SERVICE_H
#define BASE_API_SERVICE_H

#include "httprequest.h"
#include "httpresponse.h"
#include <QVector>
#include <QString>

#include "../errors/api_error_factory.h"
#include "../utils/swagger_doc_generator.h"

#define HTTP_GET_METHOD "GET"
#define HTTP_POST_METHOD "POST"
#define HTTP_DELETE_METHOD "DELETE"
#define HTTP_PUT_METHOD "PUT"
#define HTTP_OPTIONS_METHOD "OPTIONS"

#define HTTP_HEADER_AUTHORIZATION "authorization"

// user managament
#define PERM_CATEGORY_USER_MANAGEMENT "user_management"
#define PERM_UPDATE_USER_ROLE "update_user_role"
#define PERM_MONITOR_USER_MESSAGES "monitor_user_messages"

// rag file permissions
#define PERM_CATEGORY_RAG_FILE "rag_file"
#define PERM_DOWNLOAD_RAG_FILE "rag_file_download"
#define PERM_UPLOAD_RAG_FILE "rag_file_upload"
#define PERM_VIEW_RAG_FILES "rag_file_view"
#define PERM_REMOVE_RAG_FILES "rag_file_remove"

// role mangement
#define PERM_CATEGORY_ROLE_MANAGEMENT "role_management"
#define PERM_CREATE_ROLE "create_role"
#define PERM_DELETE_ROLE "delete_role"
#define PERM_UPDATE_ROLE "update_role"
#define PERM_UPDATE_ROLE_PERMISSION "update_role_permission"
#define PERM_VIEW_ROLE_PERMISSION "view_role_permissions"
#define PERM_UPDATE_MANAGABLE_ROLES "update_managable_roles"
#define PERM_VIEW_MANAGABLE_ROLES "view_managable_roles"
#define PERM_VIEW_ROLE_HIERACHY_TREE "view_role_hierachy"

// bot mangement
#define PERM_CATEGORY_BOT_MANAGEMENT "bot_management"
#define PERM_LIST_BOT "list_bot"
#define PERM_DELETE_BOT "delete_bot"
#define PERM_UPDATE_BOT "update_bot"
#define PERM_CREATE_BOT "create_bot"
#define PERM_UPDATE_BOT_RAGS "update_bot_rags"
#define PERM_UPDATE_ROLE_MANAGABLE_BOTS "update_role_managable_bots"

// developer permissions
#define PERM_CATEGORY_DEVELOPER "developer"
#define PERM_VIEW_DOCS "view_docs"

using stefanfrings::HttpRequest;
using stefanfrings::HttpResponse;

struct ApiValRes{
    bool isValid;
    BaseApiError apiErrorObj;
    // Constructor for success result
    explicit ApiValRes(BaseApiError _apiErrorObj)
        : isValid(false), apiErrorObj(std::move(_apiErrorObj)) {}

    // Constructor for error result
    explicit ApiValRes()
        : isValid(true){}
};

class BaseApiService : public QObject
{
    Q_OBJECT  // Required for QObject inheritance

public:

    explicit BaseApiService(QObject* parent = nullptr);

    // Overridable function for handling service logic
    virtual void service(HttpRequest& request, HttpResponse& response,QByteArray lang = "fa");

    // Public validation function
    virtual ApiValRes validate(HttpRequest &request);

};



#endif
