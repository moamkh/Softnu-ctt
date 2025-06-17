#include "swagger_doc_generator.h"

#include <QJsonDocument>
#include <QDir>
#include <QDebug>
#include <QSaveFile>
#include <QSet>

#include "../../global.h"

SwaggerGenerator::SwaggerGenerator()
{

}

SwaggerGenerator& SwaggerGenerator::setTitle(const QString& title) {
    m_title = title;
    return *this;
}

SwaggerGenerator& SwaggerGenerator::setVersion(const QString& version) {
    m_version = version;
    return *this;
}

SwaggerGenerator& SwaggerGenerator::setDescription(const QString& description) {
    m_description = description;
    return *this;
}

SwaggerGenerator& SwaggerGenerator::setLicense(const QString& name, const QString& url) {
    m_license = qMakePair(name, url);
    return *this;
}

SwaggerGenerator& SwaggerGenerator::addServer(const QString& url, const QString& description) {
    QJsonObject serverObj;
    serverObj["url"] = url;
    if (!description.isEmpty()) {
        serverObj["description"] = description;
    }
    m_servers.append(serverObj);
    return *this;
}

void SwaggerGenerator::addEndpoint(const Endpoint& endpoint) {
    m_endpoints.append(endpoint);
}

QJsonDocument SwaggerGenerator::generate() const
{
    QJsonObject root;
    QJsonObject paths;

    root["openapi"] = "3.0.0";

    // Info section
    QJsonObject info;
    info["title"] = m_title;
    info["version"] = m_version;

    if (!m_description.isEmpty()) {
        info["description"] = m_description;
    }

    if (!m_license.first.isEmpty()) {
        info["license"] = QJsonObject{
            {"name", m_license.first},
            {"url", m_license.second}
        };
    }
    root["info"] = info;

    // Server configuration
    if (!m_servers.isEmpty()) {
        QJsonArray serversArray;
        for (const QJsonObject& server : m_servers) {
            serversArray.append(server);
        }
        root["servers"] = serversArray;
    }

    // Components including security schemes
    QJsonObject components;
    components["securitySchemes"] = createSecuritySchemes();
    root["components"] = components;

    // Collect unique categories for tags
    QSet<QString> categories;
    for (const Endpoint& endpoint : m_endpoints) {
        if (!endpoint.category.isEmpty()) {
            categories.insert(endpoint.category);
        }
    }
    if (!categories.isEmpty()) {
        QJsonArray tagsArray;
        for (const QString& category : categories) {
            tagsArray.append(QJsonObject{{"name", category}});
        }
        root["tags"] = tagsArray;
    }

    // Process endpoints
    for (const Endpoint& endpoint : m_endpoints) {
        QJsonObject pathItem;
        QJsonObject operation;

        operation["summary"] = endpoint.description;

        // Add tags
        if (!endpoint.category.isEmpty()) {
            operation["tags"] = QJsonArray{endpoint.category};
        }

        // Add parameters
        if (!endpoint.parameters.isEmpty()) {
            operation["parameters"] = endpoint.parameters;
        }

        // Request body
        if (!endpoint.requestSchema.isEmpty()) {
            operation["requestBody"] = QJsonObject{
                {"required", true},
                {"content", QJsonObject{
                    {"application/json", QJsonObject{
                        {"schema", convertSchema(endpoint.requestSchema)}
                    }}
                }}
            };
        }

        // Responses
        QJsonObject responses;
        responses["200"] = QJsonObject{
            {"description", "Successful operation"},
            {"content", QJsonObject{
                {"application/json", QJsonObject{
                    {"schema", convertSchema(endpoint.responseSchema)}
                }}
            }}
        };
        operation["responses"] = responses;

        // Security requirements
        if (endpoint.requiresAuth) {
            operation["security"] = QJsonArray{
                QJsonObject{{"bearerAuth", QJsonArray()}}
            };
        }

        QString method = endpoint.method.toLower();
        if (paths.contains(endpoint.path)) {
            pathItem = paths[endpoint.path].toObject();
        }
        pathItem[method] = operation;
        paths[endpoint.path] = pathItem;
    }

    root["paths"] = paths;
    return QJsonDocument(root);
}

QJsonObject SwaggerGenerator::createSecuritySchemes() const
{
    return QJsonObject{
        {"bearerAuth", QJsonObject{
            {"type", "http"},
            {"scheme", "bearer"},
            {"bearerFormat", "JWT"}
        }}
    };
}

QJsonObject SwaggerGenerator::convertSchema(const QJsonObject& inputSchema) const
{
    if (inputSchema.isEmpty()) return QJsonObject();

    QJsonObject output;
    const QString type = inputSchema["type"].toString();

    // Handle common schema types
    if (type == "object") {
        output["type"] = "object";

        // Convert properties recursively
        if (inputSchema.contains("properties")) {
            QJsonObject properties;
            const QJsonObject inputProps = inputSchema["properties"].toObject();
            for (auto it = inputProps.begin(); it != inputProps.end(); ++it) {
                properties[it.key()] = convertSchema(it.value().toObject());
            }
            output["properties"] = properties;
        }

        // Add required array if present
        if (inputSchema.contains("required")) {
            output["required"] = inputSchema["required"];
        }
    }
    else if (type == "array") {
        output["type"] = "array";
        if (inputSchema.contains("items")) {
            output["items"] = convertSchema(inputSchema["items"].toObject());
        }
    }
    else if (!type.isEmpty()) {
        output["type"] = type;
        if (inputSchema.contains("format")) {
            output["format"] = inputSchema["format"].toString();
        }
    }

    // Handle parameter-specific schema elements
    if (inputSchema.contains("enum")) {
        output["enum"] = inputSchema["enum"];
    }
    if (inputSchema.contains("example")) {
        output["example"] = inputSchema["example"];
    }
    if (inputSchema.contains("default")) {
        output["default"] = inputSchema["default"];
    }

    return output;
}
QJsonObject SwaggerGenerator::createSchemaObject(const QString& type,
                                                const QJsonObject& properties,
                                                const QJsonArray& required,
                                                const QString& format)
{
    QJsonObject schema;
    schema["type"] = type;

    if (!properties.isEmpty()) {
        schema["properties"] = properties;
    }

    if (!required.isEmpty()) {
        schema["required"] = required;
    }

    if (!format.isEmpty()) {
        schema["format"] = format;
    }

    return schema;
}
bool SwaggerGenerator::writeToFile(const QString& filePath) const
{
    QJsonDocument doc = generate();

    QFileInfo fileInfo(filePath);
    QDir().mkpath(fileInfo.absolutePath());

    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    if (file.write(jsonData) == -1) {
        qWarning() << "Failed to write to file:" << filePath;
        file.cancelWriting();
        return false;
    }

    if (!file.commit()) {
        qWarning() << "Failed to commit file changes:" << filePath;
        return false;
    }

    return true;
}

QJsonObject SwaggerGenerator::createParameter(
    const QString& name,
    const QString& paramIn,
    const QString& description,
    bool required,
    const QJsonObject& schema,
    const QVariant& example)
{
    QJsonObject param;
    param["name"] = name;
    param["in"] = paramIn;
    param["description"] = description;
    param["required"] = required;
    param["schema"] = schema;

    if (!example.isNull()) {
        param["example"] = QJsonValue::fromVariant(example);
    }

    return param;
}
