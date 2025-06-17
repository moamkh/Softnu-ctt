#ifndef SCOPED_DB_CONNECTION_H
#define SCOPED_DB_CONNECTION_H

#include "db_connection_pool.h"
#include <QSqlDatabase>
#include <QSqlQuery>

class ScopedDbConnection {
public:
    // Constructor automatically acquires a connection from the pool
    explicit ScopedDbConnection(DbConnectionPool* pool);

    // Destructor automatically releases the connection
    ~ScopedDbConnection();

    // Get the database to execute queries
    QSqlDatabase& database();

    // Prevent copying to avoid double-release issues
    ScopedDbConnection(const ScopedDbConnection&) = delete;
    ScopedDbConnection& operator=(const ScopedDbConnection&) = delete;

private:
    DbConnectionPool* m_pool;
    DbConnection m_connection;
    bool m_valid;
};

#endif // SCOPED_DB_CONNECTION_H
