#ifndef DB_CONTROLLER_H
#define DB_CONTROLLER_H

#include "db_connection_pool.h"
#include "scoped_db_connection.h"
#include <QJsonObject>
#include <QObject>
#include <QVector>
#include <QJsonArray>
#define DB_FAILED_TO_EXECUTE_QUERY "Database failed to execute the query."
#define DB_NO_SUCH_USER "No such user was found."
#define DB_FAILED_TO_REGISTER_USER "User registeration failed."
#define DB_FAILED_TO_FETCH_USER_INFO "User info fetching failed."
#define DB_FAILED_TO_UPDATE_USER_PASSWORD "Failed to update user password."
#define DB_FAILED_TO_FETCH_USER_URL "Failed to fetch user url"

#define AI_METADATAKEY_INFO "info"
#define AI_METADATAKEY_PROMPT "prompt"
#define AI_METADATAKEY_RETRIVAL_CONTEXT "retrival_context"
#define AI_METADATAKEY_TPS "tps"
#define AI_METADATAKEY_RESPONSE_DURATION "response_duration"


struct DbResault {
    bool status;
    QJsonObject data;
    QString message;
    int db_err_code;

    // Constructor for success result
    explicit DbResault(QJsonObject _data)
        : status(true), data(std::move(_data)) {}

    explicit DbResault(QString _message)
        : status(false),data(QJsonObject()), message(std::move(_message)) {}

    explicit DbResault(int dbErrCode)
        : status(false),data(QJsonObject()), db_err_code(std::move(dbErrCode)) {}

    explicit DbResault()
        : status(false),data(QJsonObject()), db_err_code(0) {}
};

class DbController : public QObject
{
    Q_OBJECT
public:
    explicit DbController(QObject *parent=nullptr);
    ~DbController();




private:
    DbConnectionPool* m_DbConnectionPool;
};

#endif
