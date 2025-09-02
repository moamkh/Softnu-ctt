#include "db_controller.h"
#include "utils/string_utils.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QJsonArray>
#include <QDateTime>
#include <QJsonDocument>

DbController::DbController(QObject* parent)
{
    m_DbConnectionPool = new DbConnectionPool(parent,10);
}

DbController::~DbController()
{
    delete m_DbConnectionPool;
}

DbResault DbController::createUser(QString &fullname, QString &extensionNumber, QString &username) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT user_func.create_user(:fullname, :extensionNumber, :username);");
    query.bindValue(":fullname", fullname);
    query.bindValue(":extensionNumber", extensionNumber);
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Create User Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    return DbResault(QJsonObject());  // No return data needed, just success
}

DbResault DbController::updateUserInfo(qint64 userId, const QString &fullname, const QString &username, const QString &extensionNumber) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT user_func.update_user_info(:userId, :fullname, :username, :extensionNumber);");
    query.bindValue(":userId", userId);
    query.bindValue(":fullname", fullname);
    query.bindValue(":username", username);
    query.bindValue(":extensionNumber", extensionNumber);

    if (!query.exec()) {
        qDebug() << "Update User Info Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    return DbResault(QJsonObject());  // Success
}

DbResault DbController::getUserDetails(qint64 userId) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT * FROM user_func.get_user_details(:userId);");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Get User Details Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    if (query.first()) {
        QJsonObject userObj;
        userObj.insert("fullname", query.value("fullname").toString());
        userObj.insert("extension_number", query.value("extension_number").toString()); // Changed to toString()
        userObj.insert("username", query.value("username").toString());
        return DbResault(userObj);
    }

    return DbResault(DB_NO_SUCH_USER);
}

bool DbController::updateUser(qint64 userId, QString fullname, QString extensionNumber, QString username) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT user_func.update_user(:userId, :fullname, :extensionNumber, :username);");
    query.bindValue(":userId", userId);

    if (fullname.isEmpty())
        query.bindValue(":fullname", QVariant(QVariant::String));
    else
        query.bindValue(":fullname", fullname);

    if (extensionNumber.isEmpty())
        query.bindValue(":extensionNumber", QVariant(QVariant::String));
    else
        query.bindValue(":extensionNumber", extensionNumber);

    if (username.isEmpty())
        query.bindValue(":username", QVariant(QVariant::String));
    else
        query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Update User Query Error: " << query.lastError().text();
        return false;
    }
    return true;
}

DbResault DbController::listAllUsers() {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT * FROM user_func.list_all_users();");

    if (!query.exec()) {
        qDebug() << "List All Users Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    QJsonArray usersArray;
    while (query.next()) {
        QJsonObject userObj;
        userObj.insert("id", query.value("id").toInt());
        userObj.insert("fullname", query.value("fullname").toString());
        userObj.insert("extension_number", query.value("extension_number").toString()); // Changed to toString()
        userObj.insert("username", query.value("username").toString());
        userObj.insert("is_active", query.value("is_active").toBool());
        usersArray.append(userObj);
    }

    QJsonObject resultObj;
    resultObj.insert("users", usersArray);
    return DbResault(resultObj);
}

bool DbController::checkUserExists(qint64 userId) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT user_func.user_exists(:userId);");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Check User Exists Query Error: " << query.lastError().text();
        return false;
    }

    if (query.first()) {
        return query.value(0).toBool();
    }
    return false;
}
bool DbController::checkCallSummaryExists(qint64 callSummaryId) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT call_func.checkCallSummaryExists(:callSummaryId);");
    query.bindValue(":callSummaryId", callSummaryId);

    if (!query.exec()) {
        qDebug() << "Check Call Summary Exists Query Error: " << query.lastError().text();
        return false;
    }

    if (query.first()) {
        return query.value(0).toBool();
    }

    return false;
}
bool DbController::checkUserExistsByUsername(QString &username) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT user_func.user_exists_by_username(:username);");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Check User Exists By Username Query Error: " << query.lastError().text();
        return false;
    }

    if (query.first()) {
        return query.value(0).toBool();
    }
    return false;
}

DbResault DbController::updateUserIsActive(qint64 userId, bool isActive) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare("SELECT user_func.update_user_is_active(:userId, :isActive);");
    query.bindValue(":userId", userId);
    query.bindValue(":isActive", isActive);

    if (!query.exec()) {
        qDebug() << "Update User is_active Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    return DbResault(QJsonObject());  // Success, no data to return
}

DbResault DbController::listUsers(const QString& searchField, const QString& searchValue, int page, int perPage) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    // Log incoming values
    qDebug() << "listUsers called with parameters:";
    qDebug() << "  searchField:" << searchField;
    qDebug() << "  searchValue:" << searchValue;
    qDebug() << "  page:" << page;
    qDebug() << "  perPage:" << perPage;

    // Query to count total rows
    QSqlQuery countQuery(scopedConnection.database());
    countQuery.prepare("SELECT user_func.count_users(:search_field, :search_value);");
    countQuery.bindValue(":search_field", searchField.isEmpty() ? QVariant(QVariant::String) : searchField);
    countQuery.bindValue(":search_value", searchValue.isEmpty() ? QVariant(QVariant::String) : searchValue);

    // Log count query details
    qDebug() << "Executing count query with:";
    qDebug() << "  searchField:" << (searchField.isEmpty() ? "NULL" : searchField);
    qDebug() << "  searchValue:" << (searchValue.isEmpty() ? "NULL" : searchValue);

    int totalCount = 0;
    if (countQuery.exec() && countQuery.next()) {
        totalCount = countQuery.value(0).toInt();
    } else {
        qDebug() << "Count Users Query Error: " << countQuery.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    // Calculate total pages
    int totalPages = (totalCount + perPage - 1) / perPage;

    // Adjust page number if out of range
    if (page < 1) {
        qDebug() << "Page number too low, resetting to first page.";
        page = 1;
    } else if (page > totalPages) {
        qDebug() << "Page number too high, resetting to last page.";
        page = totalPages;
    }

    // Log adjusted values
    qDebug() << "Adjusted parameters:";
    qDebug() << "  page:" << page;
    qDebug() << "  totalPages:" << totalPages;

    // Query to list users
    query.prepare("SELECT * FROM user_func.list_users(:search_field, :search_value, :page, :per_page);");
    query.bindValue(":search_field", searchField.isEmpty() ? QVariant(QVariant::String) : searchField);
    query.bindValue(":search_value", searchValue.isEmpty() ? QVariant(QVariant::String) : searchValue);
    query.bindValue(":page", page);
    query.bindValue(":per_page", perPage);

    // Log the prepared query
    qDebug() << "Executing user listing query with:";
    qDebug() << "  searchField:" << (searchField.isEmpty() ? "NULL" : searchField);
    qDebug() << "  searchValue:" << (searchValue.isEmpty() ? "NULL" : searchValue);
    qDebug() << "  page:" << page;
    qDebug() << "  perPage:" << perPage;

    if (!query.exec()) {
        qDebug() << "List Users Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    QJsonArray usersArray;
    while (query.next()) {
        QJsonObject userObj;
        userObj.insert("id", query.value("id").toInt());
        userObj.insert("fullname", query.value("fullname").toString());
        userObj.insert("extension_number", query.value("extension_number").toString());
        userObj.insert("is_active", query.value("is_active").toBool());
        userObj.insert("last_active", query.value("last_activity").toInt()); // Include last_activity
        userObj.insert("role", "CTT");
        usersArray.append(userObj);
    }

    // Build the result object
    QJsonObject resultObj;
    resultObj.insert("results", usersArray);
    resultObj.insert("total_pages", totalPages);
    resultObj.insert("page", page);
    resultObj.insert("per_page", perPage);
    resultObj.insert("total", totalCount);

    qDebug() << "Query results constructed. Total users:" << usersArray.count();

    return DbResault(resultObj);
}

DbResault DbController::ountCallSummaries(int userId, const QString &searchField, const QString &searchValue, int startDate, int endDate) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    // Log input parameters
    qDebug() << "countCallSummaries called with parameters:";
    qDebug() << "  userId:" << userId;
    qDebug() << "  searchField:" << searchField;
    qDebug() << "  searchValue:" << searchValue;
    qDebug() << "  startDate:" << startDate;
    qDebug() << "  endDate:" << endDate;

    // Prepare the query
    query.prepare("SELECT call_func.count_call_summaries(:user_id, :search_field, :search_value, :start_date, :end_date);");
    query.bindValue(":user_id", userId);
    query.bindValue(":search_field", searchField.isEmpty() ? QVariant(QVariant::String) : searchField);
    query.bindValue(":search_value", searchValue.isEmpty() ? QVariant(QVariant::String) : searchValue);
    query.bindValue(":start_date", startDate > 0 ? startDate : QVariant(QVariant::Int));
    query.bindValue(":end_date", endDate > 0 ? endDate : QVariant(QVariant::Int));

    // Log prepared query
    qDebug() << "Executing countCallSummaries query with:";
    qDebug() << "  userId:" << userId;
    qDebug() << "  searchField:" << (searchField.isEmpty() ? "NULL" : searchField);
    qDebug() << "  searchValue:" << (searchValue.isEmpty() ? "NULL" : searchValue);
    qDebug() << "  startDate:" << (startDate > 0 ? QString::number(startDate) : "NULL");
    qDebug() << "  endDate:" << (endDate > 0 ? QString::number(endDate) : "NULL");

    int totalCount = 0;
    if (query.exec() && query.next()) {
        totalCount = query.value(0).toInt();
    } else {
        qDebug() << "Count Call Summaries Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    qDebug() << "Total count of call summaries: " << totalCount;

    return DbResault(totalCount);
}





DbResault DbController::storeCallSummary(
    QString extensionNumber,
    int startTime,
    int endTime,
    int duration,
    QString callee,
    QString direction,
    QString type,
    QString status,
    QString uniqueId
    ) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare(R"(
        SELECT call_func.store_call_summary(
            :extensionNumber,
            :startTime,
            :endTime,
            :duration,
            :callee,
            :direction,
            :type,
            :status,
            :uniqueId
        );
    )");

    query.bindValue(":extensionNumber", extensionNumber);
    query.bindValue(":startTime", startTime);
    query.bindValue(":endTime", endTime);
    query.bindValue(":duration", duration);
    query.bindValue(":callee", callee);
    query.bindValue(":direction", direction);
    query.bindValue(":type", type);
    query.bindValue(":status", status);
    query.bindValue(":uniqueId", uniqueId);

    if (!query.exec()) {
        qDebug() << "Store Call Summary Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    if (query.first()) {
        QJsonObject resultObj;
        resultObj.insert("call_summary_id", query.value(0).toInt()); // Retrieve call_summary_id
        return DbResault(resultObj);
    }

    return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
}

DbResault DbController::insertAiResponse(int callSummaryId, QString aiResponse) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    query.prepare(R"(
        SELECT call_func.insert_ai_response(
            :callSummaryId,
            :aiResponse
        );
    )");

    query.bindValue(":callSummaryId", callSummaryId);
    query.bindValue(":aiResponse", aiResponse);

    if (!query.exec()) {
        qDebug() << "Insert AI Response Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    return DbResault();
}

DbResault DbController::getTodaysCallSummariesWithEmptyAiResponse() {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    // Call the PostgreSQL function
    query.prepare(R"(
        SELECT * FROM public.get_todays_call_summaries_with_empty_ai_response();
    )");

    if (!query.exec()) {
        qDebug() << "Get Today's Call Summaries Query Error: " << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    // Prepare the result as a JSON array
    QJsonArray callSummaries;
    while (query.next()) {
        QJsonObject callSummaryObj;
        callSummaryObj.insert("call_summary_id", query.value("call_summary_id").toInt());
        callSummaryObj.insert("extension_number", query.value("extension_number").toString());
        callSummaryObj.insert("unique_id", query.value("unique_id").toString());
        callSummaries.append(callSummaryObj);
    }

    QJsonObject resultObj;
    resultObj.insert("call_summaries", callSummaries);

    return DbResault(resultObj);
}

DbResault DbController::getCallSummaryById(qint64 callSummaryId) {
    ScopedDbConnection scopedConnection(m_DbConnectionPool);
    QSqlQuery query(scopedConnection.database());

    qDebug() << "getCallSummaryById called with callSummaryId:" << callSummaryId;

    query.prepare("SELECT row_to_json(cs) FROM call_func.getCallSummaryById(:callSummaryId) AS cs;");
    query.bindValue(":callSummaryId", callSummaryId);

    if (!query.exec()) {
        qDebug() << "Get Call Summary Query Error:" << query.lastError().text();
        return DbResault(DB_FAILED_TO_EXECUTE_QUERY);
    }

    if (query.next()) {
        QJsonDocument doc = QJsonDocument::fromJson(query.value(0).toString().toUtf8());
        if (doc.isObject()) {
            qDebug() << "Call summary found:" << doc.toJson(QJsonDocument::Compact);
            return DbResault(doc.object());
        }
    }

    qDebug() << "No call summary found for ID:" << callSummaryId;
    return DbResault(DB_NOT_FOUND);
}
