#ifndef SWAGGER_DOC_GENERATOR_H
#define SWAGGER_DOC_GENERATOR_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QJsonDocument>
#include <QString>

// Primitive Types
#define SWAGGER_TYPE_STRING   "string"
#define SWAGGER_TYPE_NUMBER   "number"
#define SWAGGER_TYPE_INTEGER  "integer"
#define SWAGGER_TYPE_BOOLEAN  "boolean"
#define SWAGGER_TYPE_ARRAY    "array"
#define SWAGGER_TYPE_OBJECT   "object"

// String Formats
#define SWAGGER_FORMAT_DATE       "date"
#define SWAGGER_FORMAT_DATETIME   "date-time"
#define SWAGGER_FORMAT_PASSWORD   "password"
#define SWAGGER_FORMAT_BYTE       "byte"
#define SWAGGER_FORMAT_BINARY     "binary"
#define SWAGGER_FORMAT_EMAIL      "email"
#define SWAGGER_FORMAT_UUID       "uuid"
#define SWAGGER_FORMAT_URI        "uri"
#define SWAGGER_FORMAT_HOSTNAME   "hostname"
#define SWAGGER_FORMAT_IPV4       "ipv4"
#define SWAGGER_FORMAT_IPV6       "ipv6"
#define SWAGGER_FORMAT_ISO6391 "iso639-1"

// Numeric Formats
#define SWAGGER_FORMAT_INT32      "int32"
#define SWAGGER_FORMAT_INT64      "int64"
#define SWAGGER_FORMAT_FLOAT      "float"
#define SWAGGER_FORMAT_DOUBLE     "double"

class SwaggerGenerator
{
public:
    struct Endpoint {
        QString path;
        QString method;
        QString description;
        QJsonObject requestSchema;
        QJsonObject responseSchema = QJsonObject();
        bool requiresAuth;
        QString category;
        QJsonArray parameters=QJsonArray{};
    };

    SwaggerGenerator();

    // Configuration methods
    SwaggerGenerator& setTitle(const QString& title);
    SwaggerGenerator& setVersion(const QString& version);
    SwaggerGenerator& setDescription(const QString& description);
    SwaggerGenerator& setLicense(const QString& name, const QString& url);
    SwaggerGenerator& addServer(const QString& url, const QString& description = QString()); // Added addServer method

    void addEndpoint(const Endpoint& endpoint);
    QJsonDocument generate() const;
    bool writeToFile(const QString& filePath) const;

    static QJsonObject createParameter(
        const QString& name,
        const QString& paramIn,
        const QString& description,
        bool required,
        const QJsonObject& schema,
        const QVariant& example = QVariant()
    );

    static const QJsonObject getPathLanguageParameter() {
        static const QJsonObject languageParameter =
            SwaggerGenerator::createParameter(
                "language", "path", "language identifier", true,
                createSchemaObject("string", {}, {}, SWAGGER_FORMAT_ISO6391), QString("en")
            );
        return languageParameter;
    }
    static QJsonObject createSchemaObject(const QString& type,
                                        const QJsonObject& properties = {},
                                        const QJsonArray& required = {},
                                        const QString& format = "");
private:

    QJsonObject convertSchema(const QJsonObject& inputSchema) const ;
    QJsonObject createSecuritySchemes() const;




    QString m_title = "API Documentation";
    QString m_version = "1.0.0";
    QString m_description;
    QPair<QString, QString> m_license;
    QList<QJsonObject> m_servers; // Changed to list of servers
    QList<Endpoint> m_endpoints;
    QJsonArray parameters;
};

#endif // SWAGGER_DOC_GENERATOR_H
