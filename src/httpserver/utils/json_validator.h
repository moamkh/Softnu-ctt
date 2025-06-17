#ifndef JSON_VALIDATOR_H
#define JSON_VALIDATOR_H

#define JSON_PROPERTIES "properties"
#define JSON_TYPE "type"
#define JSON_REQUIRED "required"
#define JSON_OBJECT "object"
#define JSON_ARRAY "array"
#define JSON_ITEMS "items"
#define JSON_STRING "string"
#define JSON_NUMBER "number"
#define JSON_INTEGER "integer"
#define JSON_BOOLEAN "boolean"

#define JSON_STRING_FIELD   QJsonObject{{JSON_TYPE,JSON_STRING}}
#define JSON_NUMBER_FIELD   QJsonObject{{JSON_TYPE,JSON_NUMBER}}
#define JSON_INTEGER_FIELD  QJsonObject{{JSON_TYPE,JSON_INTEGER}}
#define JSON_BOOLEAN_FIELD  QJsonObject{{JSON_TYPE,JSON_BOOLEAN}}
#define JSON_ARRAY_FIELD    QJsonObject{{JSON_TYPE,JSON_ARRAY}}


#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

#include "../services/base_api_service.h"



class JsonValidator {
public:
    explicit JsonValidator(const QJsonObject &schema);

    // Validate a JSON object against the schema
    ApiValRes validate(const QJsonObject &json) const;

private:
    QJsonObject m_schema;

    // Check if a JSON value matches the expected type
    bool checkType(const QJsonValue &value, const QString &type) const;

    // Recursive function to validate an object based on the schema
    ApiValRes validateObject(const QJsonObject &json, const QJsonObject &schema) const;
};


#endif
