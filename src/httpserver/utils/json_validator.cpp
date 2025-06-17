#include "json_validator.h"

#include "../errors/api_error_factory.h"

JsonValidator::JsonValidator(const QJsonObject &schema) {
    m_schema = schema;
}


ApiValRes JsonValidator::validate(const QJsonObject &json) const {
    if (m_schema.isEmpty()){
        return ApiValRes(ApiErrorFactory::NoJsonSchemaError());
    }
    return validateObject(json, m_schema);
}

bool JsonValidator::checkType(const QJsonValue &value, const QString &type) const {
#ifdef QT_DEBUG
    qDebug() << "INPUTED VALUE :" << value;
    qDebug() << "INPUTED TYPE  :" << type;
#endif
    if (type == "string") return value.isString();
    if (type == "integer") return value.isDouble() && value.toDouble() == static_cast<int>(value.toDouble());
    if (type == "number") return value.isDouble();
    if (type == "boolean") return value.isBool();
    if (type == "object") return value.isObject();
    if (type == "array") return value.isArray();
    return false;
}

ApiValRes JsonValidator::validateObject(const QJsonObject &json, const QJsonObject &schema) const {
    if (!schema.contains("type") || schema["type"].toString() != "object"){
        return (ApiValRes(ApiErrorFactory::InvalidJsonSchemaFormatError()));
    }
    if (!schema.contains("properties")){
        return ApiValRes(ApiErrorFactory::InvalidJsonSchemaFormatError());
    };

    QJsonObject properties = schema["properties"].toObject();
    QJsonArray requiredFields = schema["required"].toArray();
    #ifdef QT_DEBUG
    qDebug() <<"PROPERTIES: " << properties;
    qDebug() <<"REQUIRED  : " << requiredFields;
    #endif
    if (properties.isEmpty() && requiredFields.isEmpty()){
        return ApiValRes();
    }

    // Check required fields
    for (const QJsonValue &field : requiredFields) {
        if (!json.contains(field.toString())) {
            return ApiValRes(ApiErrorFactory::MissingJsonFieldError(field.toString()));
        }
    }

    // Validate each field in the properties
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        QString key = it.key();
        QJsonObject propertySchema = it.value().toObject();

        if (!json.contains(key)) continue; // Optional field, skip if not present
        QJsonValue value = json[key];

        // Check the field type
        QString expectedType = propertySchema["type"].toString();
        if (!checkType(value, expectedType)) {
            return ApiValRes(ApiErrorFactory::JsonFieldMismatchError(key));
        }

        // Recursive check for nested objects
        if (expectedType == "object" && value.isObject()) {
            ApiValRes _objVal = validateObject(value.toObject(), propertySchema);
            if (!_objVal.isValid) {
                return ApiValRes(_objVal);
            }
        }

        // Recursive check for arrays with items type validation
        if (expectedType == "array" && value.isArray() && propertySchema.contains(JSON_ITEMS)) {
            QJsonObject itemSchema = propertySchema[JSON_ITEMS].toObject();
            QString itemType = itemSchema[JSON_TYPE].toString();

            for (const QJsonValue &item : value.toArray()) {
                if (itemType == JSON_OBJECT) {
                    // Validate nested objects
                    if (!item.isObject()) {
                        return ApiValRes(ApiErrorFactory::JsonFieldMismatchError(key + " array item"));
                    }
                    ApiValRes itemResult = validateObject(item.toObject(), itemSchema);
                    if (!itemResult.isValid) return itemResult;
                } else {
                    // Validate primitive types
                    if (!checkType(item, itemType)) {
                        return ApiValRes(ApiErrorFactory::JsonFieldMismatchError(
                            key + " array item (expected " + itemType + ")"));
                    }
                }
            }
        }
    }
    return ApiValRes();
}
