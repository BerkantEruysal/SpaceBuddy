#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include "socketclient.h"
#include <memory>

class Backend : public QObject
{

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool syncEnabled READ syncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)

private:
    explicit Backend(QObject *parent = nullptr);
    
public:
    static Backend *create(QQmlEngine *engine = nullptr, QJSEngine *scriptEngine = nullptr);
    Q_INVOKABLE void connectToServer(QString code);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void onSpacePressed();
    Q_INVOKABLE void sendTestSignal();
    
    QString connectionStatus() const;
    bool syncEnabled() const;
    void setSyncEnabled(bool enabled);

signals:
    void connectionStatusChanged();
    void syncEnabledChanged();
    void spaceReceivedRemote();

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketMessageReceived(const QString &message);
    void onSpaceSignalReceived();
    void onSocketError(const QString &error);

private:
    std::unique_ptr<SocketClient> m_socketClient;
    QString m_connectionStatus = "Bilinmiyor";
    bool m_syncEnabled = true;
};

#endif // BACKEND_H
