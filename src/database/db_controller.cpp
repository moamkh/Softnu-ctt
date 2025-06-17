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
    m_DbConnectionPool = new DbConnectionPool (parent);
}

DbController::~DbController()
{
    delete m_DbConnectionPool;
}

