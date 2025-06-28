#include "db_connection_pool.h"
#include "../global.h"
#include <QDebug>
#include <QThread>
#include <QSqlQuery>
#include <chrono>
#include <thread>
#include <QUuid>
DbConnectionPool::DbConnectionPool(QObject* parent,int pool_size)
    : QObject(parent),
      m_connectionSemaphore(myGlobalSettings->value("database/poolsize").toInt()) {
#ifdef QT_DEBUG
    qDebug() << "Initializing DB connection pool";
#endif

    m_database_username = myGlobalSettings->value("database/username").toString();
    m_database_password = myGlobalSettings->value("database/password").toString();
    m_database_name = myGlobalSettings->value("database/databasename").toString();
    m_database_port = myGlobalSettings->value("database/port").toInt();
    m_database_ip = myGlobalSettings->value("database/ip").toString();
    m_database_driver = myGlobalSettings->value("database/dbdriver").toString();
    m_max_pool_size = pool_size == 0 ? myGlobalSettings->value("database/poolsize").toInt():pool_size;
    m_max_con_retry = myGlobalSettings->value("database/reconnectRetries").toInt();

    try {
        for (int i = 0; i < m_max_pool_size; ++i) {
            QSqlDatabase db = createConnection(i);
            if (db.open()) {
                m_connections.emplace(i, db);
                m_available_con_index.push_back(i);
            }
        }
    } catch (const std::exception& e) {
        qCritical() << "Database connection pool exception:" << e.what();
        throw;
    }
}

DbConnectionPool::~DbConnectionPool() {
    QMutexLocker locker(&m_mutex);
    for (auto& [index, db] : m_connections) {
        if (db.isOpen()) {
            db.close();
        }
        QSqlDatabase::removeDatabase(db.connectionName());
    }
}

QSqlDatabase DbConnectionPool::createConnection(int index) {
    // Generate a unique connection name using UUID
    QString connectionName = QString("softnou_db_%1")
            .arg(QUuid::createUuid().toString().remove("{").remove("}"));

    // If the connection already exists, remove it
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    // Create and configure the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase(m_database_driver, connectionName);
    db.setHostName(m_database_ip);
    db.setPort(m_database_port);
    db.setDatabaseName(m_database_name);
    db.setUserName(m_database_username);
    db.setPassword(m_database_password);
    qWarning() << "Added new connection : "<< connectionName;
    return db;
}

bool DbConnectionPool::validateConnection(QSqlDatabase& db) {
    QSqlQuery testQuery(db);
    return db.isOpen() && db.isValid() && testQuery.exec("SELECT 1;");
}

void DbConnectionPool::reinitConnection(int index) {
    QSqlDatabase oldDb = m_connections.at(index);
    if (oldDb.isOpen()) {
        oldDb.close();
    }
    QSqlDatabase::removeDatabase(oldDb.connectionName());
    QSqlDatabase newDb = createConnection(index);
    newDb.open();
    m_connections[index] = newDb;
}

DbConnection DbConnectionPool::getConnection() {
    if (!m_connectionSemaphore.tryAcquire(1, 30000)) {
        return {-1, QSqlDatabase()};
    }

    QMutexLocker locker(&m_mutex);
    if (m_available_con_index.empty()) {
        m_connectionSemaphore.release();
        return {-1, QSqlDatabase()};
    }

    int index = m_available_con_index.back();
    m_available_con_index.pop_back();
    QSqlDatabase db = m_connections.at(index);

    if (!db.isOpen() || !validateConnection(db)) {
        reinitConnection(index);
        db = m_connections.at(index);
        if (!db.isOpen() || !validateConnection(db)) {
            m_connectionSemaphore.release();
            return {index, QSqlDatabase()};
        }
    }

    m_connections[index] = db;
    return {index, db};
}

void DbConnectionPool::releaseConnection(int index) {
    QMutexLocker locker(&m_mutex);
    if (index == -1 || m_connections.find(index) == m_connections.end()) {
        return;
    }

    QSqlDatabase db = m_connections.at(index);
    if (!db.isOpen() || !validateConnection(db)) {
        reinitConnection(index);
    }

    m_available_con_index.push_back(index);
    m_connectionSemaphore.release();
}

