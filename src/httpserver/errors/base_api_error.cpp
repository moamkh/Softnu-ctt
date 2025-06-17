#include "base_api_error.h"

BaseApiError::BaseApiError(QString message_english, QString message_persian, QString message_french, QString message_arabic, int statusCode, int errorCode)
    :_message_english(message_english),
    _message_persian(message_persian),
    _message_french(message_french),
    _message_arabic(message_arabic),
    _status_code(statusCode),
    _error_code(errorCode) {}

QJsonObject BaseApiError::toJson() {
    QJsonObject responseObject;
    responseObject.insert("error", _message_persian);
    responseObject.insert("errorCode", _error_code);
    return responseObject;
}

QJsonObject BaseApiError::toJson(QString lang) {
    QJsonObject responseObject;
    responseObject.insert("error", get_message_by_language(lang));
    responseObject.insert("errorCode", _error_code);
    return responseObject;
}

QString BaseApiError::get_message_english() { return _message_english; }
QString BaseApiError::get_message_persian() { return _message_persian; }
QString BaseApiError::get_message_french() { return _message_french; }
QString BaseApiError::get_message_arabic() { return _message_arabic; }
int BaseApiError::get_status_code() { return _status_code; }
int BaseApiError::get_error_code() {return _error_code; }

QString BaseApiError::get_message_by_language(QString lang)
{
     if (lang.toLower() == "en") {
        return _message_english;
    } else if (lang.toLower() == "fa") {
        return _message_persian;
    } else if (lang.toLower() == "fr") {
        return _message_french;
    } else if (lang.toLower() == "ar") {
        return _message_arabic;
    } else {
        // Default to Persian if the language code is not recognized
        return _message_persian;
    }
}
