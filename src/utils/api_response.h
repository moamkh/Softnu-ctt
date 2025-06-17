#ifndef API_RESPONSE_H
#define API_RESPONSE_H

// this file is for creating specefic responses for the api


#include <QByteArray>
#include <QJsonObject>
#include <QByteArray>

#include "httprequesthandler.h"


QByteArray FalseResponse(QString errorMessage);
QByteArray JsonResponse(QJsonObject jsonResponse);
QJsonObject doneWsResponse(const qint64& userMessageID,const qint64& aiMessageID,const QJsonObject& aiMetaData);

#endif // API_RESPONSE_H
