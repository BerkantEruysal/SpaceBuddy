#include "backend.h"
#include "config.h"
#include <QCoreApplication>

Backend::Backend(QObject *parent) :
    QObject(parent),
    m_socketClient(std::make_unique<SocketClient>(QUrl(Config::getServerUrl()), Config::isDebugMode()))
{
    // Removed auto-quit on socket close to prevent app from closing on connection failure
    // QObject::connect(m_socketClient.get(), &SocketClient::closed,
    //                  QCoreApplication::instance(), &QCoreApplication::quit);

    connect(m_socketClient.get(), &SocketClient::connected, this, &Backend::onSocketConnected);
    connect(m_socketClient.get(), &SocketClient::closed, this, &Backend::onSocketDisconnected);
    connect(m_socketClient.get(), &SocketClient::messageReceived, this, &Backend::onSocketMessageReceived);
    connect(m_socketClient.get(), &SocketClient::spaceSignalReceived, this, &Backend::onSpaceSignalReceived);
    connect(m_socketClient.get(), &SocketClient::connectionError, this, &Backend::onSocketError);
}

Backend *Backend::create(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    static Backend instance;
    QJSEngine::setObjectOwnership(&instance, QJSEngine::CppOwnership);
    return &instance;
}

void Backend::connectToServer(QString code){
    qDebug() << "Connecting with code:" << code;
    m_connectionStatus = "Bağlanıyor...";
    emit connectionStatusChanged();
    m_socketClient->setCode(code);
    m_socketClient->connectToServer();
}

void Backend::disconnectFromServer() {
    m_socketClient->disconnectFromServer();
}

void Backend::onSpacePressed(){
    if (m_syncEnabled) {
        m_socketClient->sendSpaceSignal();
    }
}

QString Backend::connectionStatus() const {
    return m_connectionStatus;
}

bool Backend::syncEnabled() const {
    return m_syncEnabled;
}

void Backend::setSyncEnabled(bool enabled) {
    if (m_syncEnabled != enabled) {
        m_syncEnabled = enabled;
        emit syncEnabledChanged();
    }
}

void Backend::onSocketConnected() {
    m_connectionStatus = "Bağlandı";
    emit connectionStatusChanged();
}

void Backend::onSocketDisconnected() {
    m_connectionStatus = "Bağlantı Kesildi";
    emit connectionStatusChanged();
}

void Backend::onSocketMessageReceived(const QString &message) {
    Q_UNUSED(message);
}

void Backend::onSpaceSignalReceived() {
    if (m_syncEnabled) {
        emit spaceReceivedRemote();
    }
}

void Backend::sendTestSignal() {
    m_socketClient->sendTestSignal();
}

void Backend::onSocketError(const QString &error) {
    m_connectionStatus = "Hata: " + error;
    emit connectionStatusChanged();
    qWarning() << "Connection error:" << error;
}
