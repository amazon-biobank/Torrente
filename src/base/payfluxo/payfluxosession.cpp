#include "payfluxosession.h"
#include "base/bittorrent/session.h"

#include <QWebSocket>
#include <QUrl>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


PayfluxoSession::PayfluxoSession( bool debug, QObject* parent)
    : QObject(parent)
    , m_debug(debug)
{
    QUrl url("ws://127.0.0.1:7933");

    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &PayfluxoSession::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &PayfluxoSession::closed);
    m_webSocket.open(url);
}

void PayfluxoSession::onConnected()
{
    if (m_debug)
        qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
        this, &PayfluxoSession::onTextMessageReceived);
    
}

void PayfluxoSession::closed() {

}

void PayfluxoSession::handlePaymentNotification(QString ip)
{
    BitTorrent::Session::instance()->unbanIP(ip);
}

void PayfluxoSession::onTextMessageReceived(QString message)
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

void PayfluxoSession::sendBlockDownloadedMessage(QString ip, QString torrentId, QString fileSize)
{
    QJsonObject dataObj;
    dataObj.insert("uploaderIp", ip);
    dataObj.insert("torrentId", torrentId);
    dataObj.insert("fileSize", fileSize);
    QJsonObject messageObj;
    messageObj.insert("type", QString("DownloadedBlock"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray ba = messageDoc.toJson();
    this->sendMessage(QString(ba));
}

void PayfluxoSession::sendMessage(QString message)
{
    m_webSocket.sendTextMessage(message);
}


