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
#define DB_FAILED_TO_REGISTER_USER "User registration failed."
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

    explicit DbResault(QJsonObject _data)
        : status(true), data(std::move(_data)) {}

    explicit DbResault(QString _message)
        : status(false), data(QJsonObject()), message(std::move(_message)) {}

    explicit DbResault(int dbErrCode)
        : status(false), data(QJsonObject()), db_err_code(dbErrCode) {}

    explicit DbResault()
        : status(false), data(QJsonObject()), db_err_code(0) {}
};

class DbController : public QObject
{
    Q_OBJECT
public:
    explicit DbController(QObject *parent=nullptr);
    ~DbController();

    DbResault createUser(QString &fullname, QString &extensionNumber, QString &username);
    DbResault getUserDetails(qint64 userId);
    bool updateUser(
        qint64 userId,
        QString fullname,            // pass empty if no update
        QString extensionNumber,     // pass empty if no update
        QString username             // pass empty if no update
        );
    DbResault listAllUsers();
    DbResault listUsers(const QString& searchField, const QString& searchValue, int page, int perPage);
    bool checkUserExists(qint64 userId);
    bool checkUserExistsByUsername(QString &username);
    DbResault updateUserIsActive(qint64 userId, bool isActive);

    DbResault storeCallSummary(
        QString extensionNumber,
        int startTime,
        int endTime,
        int duration,
        QString callee,
        QString direction,
        QString type,
        QString status,
        QString uniqueId
        ) ;
    DbResault insertAiResponse(
        int callSummaryId,
        QString aiResponse
        );

    DbResault getTodaysCallSummariesWithEmptyAiResponse();

private:
    DbConnectionPool* m_DbConnectionPool;
};

#endif // DB_CONTROLLER_H
