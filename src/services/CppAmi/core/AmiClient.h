#ifndef AMICLIENT_HPP
#define AMICLIENT_HPP

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QJsonObject>
#include <QTimer>

class AmiClient : public QObject {
    Q_OBJECT

public:
    explicit AmiClient(QObject* parent = nullptr);
    ~AmiClient();

    bool connectToServer(const QString& host, quint16 port, const QString& username, const QString& password);
    void disconnectFromServer();

signals:
    void connected();
    void disconnected();
    void error(const QString& errorMessage);
    void eventReceived(const QJsonObject& eventData);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();
    void onReconnectTimeout();

private:
    QTcpSocket* m_socket;
    QString m_username;
    QString m_password;
    QTimer* m_reconnectTimer;
    bool m_authenticated;
    QByteArray m_buffer;

    void authenticate();
    void parseEvent(const QByteArray& eventData);
    void handleResponse(const QByteArray& response);
    void startReconnectTimer();
    void stopReconnectTimer();
};

#endif // AMICLIENT_HPP 