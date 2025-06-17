#ifndef JWT_MANAGER_H
#define JWT_MANAGER_H
#include <QString>

struct JWTPayload {
    QString username;
    int     accessLevel;
    qint64 userID;
    bool is_valid_token=false;
    JWTPayload (
            QString _username="",
            int _accessLevel=-1,
            qint64 _userID=0):
        username(_username),
        accessLevel(_accessLevel),
        userID(_userID){}

};

class AccessLevel {
public:
    static constexpr int ANY_USER = 0 ;
    static constexpr int NORMAL_USER = 1;
    static constexpr int ADMIN_USER = 2 ;
    static constexpr int SUPER_ADMIN_USER =3 ;

    static const QString NORMAL_USER_TYPE ;
    static const QString ADMIN_USER_TYPE ;
    static const QString SUPER_ADMIN_USER_TYPE ;
    static const QString ANY_USER_TYPE ;

    static QString GetAccessLevelType(int accessLevelCode);
    static int GetAccessLevelCode(QString accessLevelType);
};

class JWTManager
{
public:
    JWTManager();
    QString validateTokenGetUsername(const QByteArray& token);
    qint64 ValidateTokenGetUserID(const QByteArray &token);
    int ValidateTokenGetAccessLevel(const QByteArray& token);
    JWTPayload ValidateTokenGetPayload(const QByteArray& token);
    QString GenerateToken(
            const QString& username,
            const quint64 &userID,
            const int& accessLevel=AccessLevel::NORMAL_USER);

private:
    const std::string USERNAME_KEY      = "username";
    const std::string USERID_KEY        = "userID";
    const std::string ACCESSLEVEL_KEY   = "accessLevel";

    std::string _jwt_secret;
    std::string _jwt_issuer;
    std::string _jwt_prefix;
    std::string _jwt_type;
    qint64 _jwt_lifetime;
};

#endif
