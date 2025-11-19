#include "socketclient.h"
#include <QtCore/QDebug>
#include "json.hpp"

SocketClient::SocketClient(const QUrl &url, bool debug, QObject *parent)
    : QObject(parent),
    m_url(url),
    m_debug(debug)
{
    if (m_debug){
        qDebug() << "WebSocket server:" << url;
    }
    connect(&m_webSocket, &QWebSocket::connected, this, &SocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &SocketClient::closed);
    
    // Error handling
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            [this](QAbstractSocket::SocketError error){
        qWarning() << "WebSocket error:" << error << m_webSocket.errorString();
        emit connectionError(m_webSocket.errorString());
    });
}

void SocketClient::setCode(const QString &c) {
    code = c;
}

void SocketClient::connectToServer() {
    if (m_webSocket.state() == QAbstractSocket::ConnectedState || 
        m_webSocket.state() == QAbstractSocket::ConnectingState) {
        if (m_debug) {
            qDebug() << "Already connected or connecting. Ignoring request.";
        }
        return;
    }

    if (m_debug) {
        qDebug() << "Connecting to WebSocket server:" << m_url;
    }
    m_webSocket.open(m_url);
}

void SocketClient::disconnectFromServer() {
    m_webSocket.close();
}

void SocketClient::onConnected(){
    if(m_debug){
        qDebug() << "Websocket connected";
    }
    emit connected();
    connect (&m_webSocket, &QWebSocket::textMessageReceived,
            this, &SocketClient::onTextMessageReceived);
}

void SocketClient::onTextMessageReceived(QString message){
    if(m_debug){
        qDebug() << "Message received:" << message;
    }
    emit messageReceived(message);

    try {
        auto j = nlohmann::json::parse(message.toStdString());
        if (j.contains("action") && j["action"] == "SPACEPUSHED") {
            emit spaceSignalReceived();
        }
    } catch (const std::exception &e) {
        qWarning() << "JSON parse error:" << e.what();
    }
}

void SocketClient::sendSpaceSignal(){
    if (m_debug)
        qDebug() << "Message sent: SPACEPUSHED";

    nlohmann::json j;
    j["action"] = "SPACEPUSHED";
    j["code"] = code.toStdString();
    
    if (m_webSocket.isValid()) {
        m_webSocket.sendTextMessage(QString::fromStdString(j.dump()));
    } else {
        qWarning() << "WebSocket is not valid, cannot send message";
    }
}

void SocketClient::sendTestSignal(){
    if (m_debug)
        qDebug() << "Message sent: TEST";

    nlohmann::json j;
    j["action"] = "TEST";
    j["code"] = code.toStdString();
    
    if (m_webSocket.isValid()) {
        m_webSocket.sendTextMessage(QString::fromStdString(j.dump()));
    } else {
        qWarning() << "WebSocket is not valid, cannot send test message";
    }
}
