#ifndef UTILS_H
#define UTILS_H


#include "httprequesthandler.h"
#include "default_settings.h"
#include <QCryptographicHash>

using namespace stefanfrings;





QString searchConfigFile(QString);

QString hashString(const QString& input, QCryptographicHash::Algorithm algorithm=QCryptographicHash::Sha256);
QByteArray generateRandomSalt(int length);
QString hashStringWithSalt(const QString& input, QCryptographicHash::Algorithm algorithm);

QString validateTokenGetUsername(const QByteArray& token);
qint64 validateTokenGetUserID(const QByteArray &token);
QString generateToken(const QString& username,const qint32 &userID);

void setDefaultCorsHeaders(HttpResponse& response);

void staticFolderSetup(const QString &baseDir);
QString getStaticFilePath();


























#endif // UTILS_H
