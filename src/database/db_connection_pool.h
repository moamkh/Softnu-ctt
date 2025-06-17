#ifndef DB_CONNECTION_POOL_H
#define DB_CONNECTION_POOL_H
#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QSemaphore>
#include <QString>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#include <stdexcept>
#include <QUuid>

struct DbConnection {
    int index;
    QSqlDatabase database;
};

class DbConnectionPool : public QObject {
    Q_OBJECT
public:
    explicit DbConnectionPool(QObject* parent = nullptr,int pool_size = 0);
    ~DbConnectionPool();

    DbConnection getConnection();
    void releaseConnection(int index);

private:
    int m_max_pool_size;
    int m_max_con_retry;
    int m_database_port;
    QString m_database_ip;
    QString m_database_name;
    QString m_database_username;
    QString m_database_password;
    QString m_database_driver;

    std::unordered_map<int, QSqlDatabase> m_connections;
    std::vector<int> m_available_con_index;
    QMutex m_mutex;
    QSemaphore m_connectionSemaphore;

    QSqlDatabase createConnection(int index); // renamed for clarity
    bool validateConnection(QSqlDatabase& db);
    void reinitConnection(int index); // new helper
};

#endif

