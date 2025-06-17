#include "scoped_db_connection.h"

ScopedDbConnection::ScopedDbConnection(DbConnectionPool* pool)
    : m_pool(pool), m_connection(pool->getConnection()), m_valid(m_connection.index != -1)
{
}

ScopedDbConnection::~ScopedDbConnection()
{
    if (m_valid) {
        m_pool->releaseConnection(m_connection.index);
    }
}

QSqlDatabase& ScopedDbConnection::database()
{
    return m_connection.database;
}
