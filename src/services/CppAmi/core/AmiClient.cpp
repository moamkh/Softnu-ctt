#include "AmiClient.h"
#include "AmiLogger.h"
#include <QDebug>

AmiClient::AmiClient(QObject* parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_reconnectTimer(new QTimer(this))
    , m_authenticated(false)
{
    // Connect socket signals
    connect(m_socket, &QTcpSocket::connected, this, &AmiClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &AmiClient::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred,this, &AmiClient::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &AmiClient::onReadyRead);

    // Setup reconnect timer
    m_reconnectTimer->setInterval(5000); // 5 seconds
    connect(m_reconnectTimer, &QTimer::timeout, this, &AmiClient::onReconnectTimeout);
}

AmiClient::~AmiClient()
{
    disconnectFromServer();
}

bool AmiClient::connectToServer(const QString& host, quint16 port, const QString& username, const QString& password)
{
    m_username = username;
    m_password = password;
    m_authenticated = false;

    m_socket->connectToHost(host, port);
    return m_socket->waitForConnected(5000);
}

void AmiClient::disconnectFromServer()
{
    stopReconnectTimer();
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }
}

void AmiClient::onConnected()
{
    AmiLogger::instance().info("Connected to AMI server");
    authenticate();
    emit connected();
}

void AmiClient::onDisconnected()
{
    m_authenticated = false;
    AmiLogger::instance().info("Disconnected from AMI server");
    startReconnectTimer();
    emit disconnected();
}

void AmiClient::onError(QAbstractSocket::SocketError socketError)
{
    QString errorMessage = QString("Socket error: %1").arg(m_socket->errorString());
    AmiLogger::instance().error(errorMessage);
    emit error(errorMessage);
}

void AmiClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    // Process complete events
    while (true) {
        int endPos = m_buffer.indexOf("\r\n\r\n");
        if (endPos == -1) break;

        QByteArray eventData = m_buffer.left(endPos);
        m_buffer.remove(0, endPos + 4);

        if (!eventData.isEmpty()) {
            if (eventData.startsWith("Asterisk Call Manager")) {
                handleResponse(eventData);
            } else {
                parseEvent(eventData);
            }
        }
    }
}

void AmiClient::onReconnectTimeout()
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        AmiLogger::instance().info("Attempting to reconnect to AMI server...");
        m_socket->connectToHost(m_socket->peerName(), m_socket->peerPort());
    }
}

void AmiClient::authenticate()
{
    QString authCommand = QString("Action: Login\r\n"
                                "Username: %1\r\n"
                                "Secret: %2\r\n"
                                "Events: on\r\n"
                                "\r\n")
                                .arg(m_username)
                                .arg(m_password);
    m_socket->write(authCommand.toUtf8());
}

void AmiClient::parseEvent(const QByteArray& eventData)
{
    if (!m_authenticated) return;

    QJsonObject event;
    QStringList lines = QString::fromUtf8(eventData).split("\r\n", Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        int colonPos = line.indexOf(':');
        if (colonPos != -1) {
            QString key = line.left(colonPos).trimmed();
            QString value = line.mid(colonPos + 1).trimmed();
            event[key] = value;
        }
    }

    if (!event.isEmpty()) {
        emit eventReceived(event);
    }
}

void AmiClient::handleResponse(const QByteArray& response)
{
    QString responseStr = QString::fromUtf8(response);
    if (responseStr.contains("Authentication accepted")) {
        m_authenticated = true;
        AmiLogger::instance().info("Successfully authenticated with AMI server");
    } else if (responseStr.contains("Authentication failed")) {
        m_authenticated = false;
        AmiLogger::instance().error("Failed to authenticate with AMI server");
        emit error("Authentication failed");
    }
}

void AmiClient::startReconnectTimer()
{
    if (!m_reconnectTimer->isActive()) {
        m_reconnectTimer->start();
    }
}

void AmiClient::stopReconnectTimer()
{
    m_reconnectTimer->stop();
} 
