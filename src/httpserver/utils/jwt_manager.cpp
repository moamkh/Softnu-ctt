#include "jwt_manager.h"

#include "jwt-cpp/jwt.h"

#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include <QString>
#include "../../global.h"

const int AccessLevel::ANY_USER;
const int AccessLevel::NORMAL_USER;
const int AccessLevel::ADMIN_USER;
const int AccessLevel::SUPER_ADMIN_USER;

const QString AccessLevel::ANY_USER_TYPE         = "Any";
const QString AccessLevel::NORMAL_USER_TYPE      = "Normal";
const QString AccessLevel::ADMIN_USER_TYPE       = "Admin";
const QString AccessLevel::SUPER_ADMIN_USER_TYPE = "SuperAdmin";


QString AccessLevel::GetAccessLevelType(int accessLevelCode)
{
    if (accessLevelCode == NORMAL_USER){
        return QStringLiteral("Normal");
    }
    else if(accessLevelCode == ADMIN_USER){
        return QStringLiteral("Admin");
    }
    else if (accessLevelCode == SUPER_ADMIN_USER) {
        return QStringLiteral("SuperAdmin");
    }
    else if (accessLevelCode == ANY_USER){
        return QStringLiteral("Any");
    }
    else {
        return "";
    }
}

int AccessLevel::GetAccessLevelCode(QString accessLevelType)
{

    if (accessLevelType == NORMAL_USER_TYPE){
        return NORMAL_USER;
    }
    else if(accessLevelType == ADMIN_USER_TYPE){
        return ADMIN_USER;
    }
    else if (accessLevelType == SUPER_ADMIN_USER_TYPE) {
        return SUPER_ADMIN_USER;
    }
    else if (accessLevelType == ANY_USER_TYPE){
        return ANY_USER;
    }
    else {
        return -1;
    }
}

JWTManager::JWTManager()
{
    _jwt_secret     = myGlobalSettings->value("JWT/SECRET").toString().toStdString();
    _jwt_issuer     = myGlobalSettings->value("JWT/ISSUER").toString().toStdString();
    _jwt_prefix     = myGlobalSettings->value("JWT/STARTER_PREFIX").toString().toStdString();
    _jwt_type       = myGlobalSettings->value("JWT/TYPE").toString().toStdString();
    _jwt_lifetime   = static_cast<qint64>(myGlobalSettings->value("JWT/DEFAULT_TOKEN_LIFE_SECONDS").toInt());

    qInfo() << "(JWT Settings beeing used)";
    qInfo() << "JWT Secret : "          << QString::fromStdString(_jwt_secret);
    qInfo() << "JWT Issuer : "          << QString::fromStdString(_jwt_issuer);
    qInfo() << "JWT Prefix : "          << QString::fromStdString(_jwt_prefix);
    qInfo() << "JWT Type : "            << QString::fromStdString(_jwt_type);
    qInfo() << "JWT Token Life time : " << QString::number(_jwt_lifetime);
}

QString JWTManager::validateTokenGetUsername(const QByteArray &token)
{
    QString strToken = token;
    QString res="";

    if (token.isEmpty()){return res;}
    if (!strToken.contains("Bearer")){return res;}

    QStringList parts = strToken.split(" ");
    if(parts[0].isEmpty()){return res;}
    if(parts.length()<2){return res;}
    auto verifyier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(_jwt_secret))
            .with_issuer(_jwt_issuer);
    auto decoded = jwt::decode(parts[1].toStdString());
    try
    {

        verifyier.verify(decoded);
        for (auto& e : decoded.get_payload_json()) {
            if (e.first == USERNAME_KEY) {

                res = QString::fromStdString(e.second.serialize()).remove("\"");
            }
          }
        return res;
    }catch(const std::exception& e)
    {
        qWarning() << e.what();
        return res;
    }
}
qint64 JWTManager::ValidateTokenGetUserID(const QByteArray &token){

    QString strToken = token;

    qint64 res=0;

    if (token.isEmpty()){return 0;}

    if (!strToken.contains(QString::fromStdString(_jwt_prefix))){return 0;}

    QStringList parts = strToken.split(" ");

    if(parts[0].isEmpty()){return 0;}
    if(parts.length()<2){return res;}
    try
    {
    auto verifyier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(_jwt_secret))
            .with_issuer(_jwt_issuer);

    auto decoded = jwt::decode(parts[1].toStdString());

    verifyier.verify(decoded);

    for (auto& e : decoded.get_payload_json()) {
        if (e.first == USERID_KEY) {
            res = QString::fromStdString(e.second.serialize()).remove("\"").toInt();
        }
      }
    return res;

    }catch(const std::exception& e)
    {
        qDebug()<<e.what();
        return 0;
    }

}
int JWTManager::ValidateTokenGetAccessLevel(const QByteArray &token)
{

    QString strToken = QString::fromUtf8(token);

    int res=-1;

    if (token.isEmpty()){return res;}
    if (!strToken.contains(QString::fromStdString(_jwt_prefix))){return res;}

    QStringList parts = strToken.split(" ");

    if(parts[0].isEmpty()){return res;}
    if(parts.length()<2){return res;}
    try
    {
    auto verifyier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(_jwt_secret))
            .with_issuer(_jwt_issuer);

    auto decoded = jwt::decode(parts[1].toStdString());

    verifyier.verify(decoded);

    for (auto& e : decoded.get_payload_json()) {
        if (e.first == ACCESSLEVEL_KEY) {
            res = QString::fromStdString(e.second.serialize()).remove("\"").toInt();
        }
      }
    return res;

    }catch(const std::exception& e)
    {
        qDebug()<<e.what();
        return res;
    }
}
JWTPayload JWTManager::ValidateTokenGetPayload(const QByteArray &token)
{
    QString strToken = QString::fromUtf8(token);

    JWTPayload res;

    if (token.isEmpty()){return res;}
    if (!strToken.contains(QString::fromStdString(_jwt_prefix))){return res;}

    QStringList parts = strToken.split(" ");

    if(parts[0].isEmpty()){return res;}
    if(parts.length()<2){return res;}
    try
    {
    auto verifyier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(_jwt_secret))
            .with_issuer(_jwt_issuer);

    auto decoded = jwt::decode(parts[1].toStdString());

    verifyier.verify(decoded);

    for (auto& e : decoded.get_payload_json()) {
        if (e.first == ACCESSLEVEL_KEY) {
            res.accessLevel = QString::fromStdString(e.second.serialize()).remove("\"").toInt();
        }
        else if (e.first == USERNAME_KEY) {
            res.username = QString::fromStdString(e.second.serialize()).remove("\"");
        }
        else if (e.first == USERID_KEY) {
            res.userID = static_cast<quint64>(QString::fromStdString(e.second.serialize()).remove("\"").toInt());
        }
     }
    if (res.username.isEmpty() || res.userID == 0 || res.accessLevel == -1)
    {
        return JWTPayload();
    }
    res.is_valid_token=true;
    return res;

    }catch(const std::exception& e)
    {
        qDebug()<<e.what();
        return res;
    }
}
QString JWTManager::GenerateToken(
        const QString& username,
        const quint64& userID,
        const int& accessLevel)
{
    qint64 tokenExpirationEpoc=0;
        tokenExpirationEpoc =
                QDateTime::currentDateTime()
                .addSecs(_jwt_lifetime)
                .toSecsSinceEpoch();
    std::string token = jwt::create()
            .set_issuer(_jwt_issuer)
            .set_type(_jwt_type)
            .set_payload_claim(USERID_KEY       ,jwt::claim(std::to_string(userID)))
            .set_payload_claim(USERNAME_KEY     ,jwt::claim(username.toStdString()))
            .set_payload_claim(ACCESSLEVEL_KEY  ,jwt::claim(std::to_string(accessLevel)))
            .set_expires_at(jwt::date(std::chrono::duration<int>(tokenExpirationEpoc)))
            .sign(jwt::algorithm::hs256(_jwt_secret));

    auto decoded = jwt::decode(token);
    for(auto& e : decoded.get_payload_json())
            std::cout << e.first << " = " << e.second.serialize() << std::endl;
    return QString::fromStdString(token);
}
