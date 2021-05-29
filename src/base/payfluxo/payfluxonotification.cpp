#include "payfluxonotification.h"
#include "base/bittorrent/session.h"

#include <QWebSocket>
#include <QUrl>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>


PayfluxoNotification::PayfluxoNotification( bool debug, QObject* parent)
    : QObject(parent)
    , m_debug(debug)
{
    QUrl url("ws://127.0.0.1:7933");

    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &PayfluxoNotification::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &PayfluxoNotification::closed);
    m_webSocket.open(url);
}

void PayfluxoNotification::onConnected()
{
    if (m_debug)
        qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
        this, &PayfluxoNotification::onTextMessageReceived);
    
}

void PayfluxoNotification::closed() {

}

void PayfluxoNotification::handlePaymentNotification(QString ip)
{
    BitTorrent::Session::instance()->unbanIP(ip);
}

void PayfluxoNotification::onTextMessageReceived(QString message)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    QString type = jsonObject["type"].toString();    
    int typeComparison = QString::compare(type, "PaymentNotification", Qt::CaseSensitive);
    QJsonObject dataJson = jsonObject["data"].toObject();

    if (typeComparison == 0) {
        this->handlePaymentNotification(dataJson["ip"].toString());
    }
    
    if (m_debug)
        qDebug() << "Message received:" << message;
    
}


