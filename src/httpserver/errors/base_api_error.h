#ifndef BASE_API_ERROR_H
#define BASE_API_ERROR_H

#include <QString>
#include <QJsonObject>

class BaseApiError
{
public:
    BaseApiError(
        QString message_english = "Server Internal Error",
        QString message_persian = "خطای داخلی سرور",
        QString message_french = "Erreur interne du serveur",
        QString message_arabic = "خطأ داخلي في الخادم",
        int statusCode = 500,
        int errorCode = 0
            );

    // Function to generate a JSON response
    QJsonObject toJson();
    QJsonObject toJson(QString lang = "fa");
    QString get_message_english();
    QString get_message_persian();
    QString get_message_french();
    QString get_message_arabic();
    int get_status_code();
    int get_error_code();

    //Function to get the appropiate language error message
    QString get_message_by_language(QString lang = "fa");
private:
    QString _message_english;
    QString _message_persian;
    QString _message_french;
    QString _message_arabic;
    int _status_code;
    int _error_code;

};

#endif
