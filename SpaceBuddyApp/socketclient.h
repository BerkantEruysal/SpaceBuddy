#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(const QUrl &url, bool debug = false, QObject *parent = nullptr);
    void sendSpaceSignal();
    void sendTestSignal();
    void setCode(const QString &c);
    void connectToServer();
    void disconnectFromServer();

Q_SIGNALS:
    void closed();
    void connected();
    void messageReceived(QString message);
    void spaceSignalReceived();
    void connectionError(QString error);

private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString message);


private:
    QWebSocket m_webSocket;
    QUrl m_url;
    bool m_debug;
    QString code;
    bool isConnected {false};

};

#endif // SOCKETCLIENT_H
