#ifndef DEFAULT_SETTINGS_H
#define DEFAULT_SETTINGS_H


#include <QSet>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QSettings>
#include <QDebug>
#include <iostream>
//HTTP METHODS DEFINES
#define HTTP_METHOD_OPTIONS "OPTIONS"
#define HTTP_METHOD_POST "POST"
#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_DELETE "DELETE"
#define HTTP_METHOD_PUT "PUT"

class JWTDefaults{

public:
    JWTDefaults(QSettings* settings):
        JWT_DEFAULT_TOKEN_LIFE_SECONDS(settings->value("DEFAULT_TOKEN_LIFE_SECONDS").toInt()),
        JWT_TYPE(settings->value("TYPE").toString().toStdString()),
        JWT_ISSUER(settings->value("ISSUER").toString().toStdString()),
        JWT_SECRET(settings->value("SECRET").toString().toStdString()),
        JWT_USER_ID_KEY(settings->value("USER_ID_KEY").toString().toStdString()),
        JWT_ACCESS_CONTROL_LEVEL_KEY(settings->value("ACCESS_CONTROL_LEVEL_KEY").toString().toStdString()),
        JWT_USERNAME_KEY(settings->value("USERNAME_KEY").toString().toStdString()),
        JWT_STARTER_PREFIX(settings->value("STARTER_PREFIX").toString().toStdString())
    {
#ifdef QT_DEBUG
        std::cout << "Initializing default jwt values"   << std::endl;
        std::cout << "JWT_DEFAULT_TOKEN_LIFE_SECONDS:"   << JWT_DEFAULT_TOKEN_LIFE_SECONDS<<std::endl;
        std::cout << "JWT_TYPE:"                         << JWT_TYPE<<std::endl;
        std::cout << "JWT_ISSUER:"                       << JWT_ISSUER<<std::endl;
        std::cout << "JWT_SECRET:"                       << JWT_SECRET<<std::endl;
        std::cout << "JWT_USER_ID_KEY:"                  << JWT_USER_ID_KEY<<std::endl;
        std::cout << "JWT_ACCESS_CONTROL_LEVEL_KEY:"     << JWT_ACCESS_CONTROL_LEVEL_KEY<<std::endl;
        std::cout << "JWT_USERNAME_KEY:"                 << JWT_ACCESS_CONTROL_LEVEL_KEY<<std::endl;
        std::cout << "JWT_STARTER_PREFIX:"               << JWT_USERNAME_KEY<<std::endl;


#endif
    }
    const qint64      JWT_DEFAULT_TOKEN_LIFE_SECONDS ;
    const std::string JWT_TYPE                       ;
    const std::string JWT_ISSUER                     ;
    const std::string JWT_SECRET                     ;
    const std::string JWT_USER_ID_KEY                ;
    const std::string JWT_ACCESS_CONTROL_LEVEL_KEY   ;
    const std::string JWT_USERNAME_KEY               ;
    const std::string JWT_STARTER_PREFIX             ;
};


#endif // DEFAULT_SETTINGS_H
