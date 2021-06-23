#include "payfluxoservice.h"
#include "payfluxo.h"
#include "base/bittorrent/session.h"

#include <QWebSocket>
#include <QUrl>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#define PAYFLUXO_SOCKET "ws://127.0.0.1:7933"
#define RECONNECT_WAIT 100

PayfluxoService::PayfluxoService(bool debug, QObject* parent)
    : QObject(parent)
    , m_debug(debug)
{
    QUrl url(PAYFLUXO_SOCKET);

    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &PayfluxoService::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &PayfluxoService::closed);
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &PayfluxoService::onWebSocketError);

    m_webSocket.open(url);
}

void PayfluxoService::onWebSocketError(QAbstractSocket::SocketError error) {
    QUrl url(PAYFLUXO_SOCKET);
    qDebug() << "Failed to connect to Payfluxo::error" << endl;
    QThread::msleep(RECONNECT_WAIT);
    m_webSocket.open(url);
}

void PayfluxoService::onConnected()
{
    if (m_debug)
        qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
        this, &PayfluxoService::onTextMessageReceived);
}

void PayfluxoService::closed() {

}

void PayfluxoService::handlePaymentNotification(QString ip)
{
    Payfluxo::Session* session = Payfluxo::Session::instance();

    session->decreaseIpPaymentPendent(ip);
    session->setRedeemableCoins(session->getRedeemableCoins() + 1);

    if (session->getIpPendentPayment(ip) < 1)
        BitTorrent::Session::instance()->unbanIP(ip);
}

void PayfluxoService::onTextMessageReceived(QString message)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    QString type = jsonObject["type"].toString();

    QJsonObject dataJson = jsonObject["data"].toObject();

    if (QString::compare(type, "PaymentNotification", Qt::CaseSensitive) == 0) {
        this->handlePaymentNotification(dataJson["ip"].toString());
    }

    if (m_debug)
        qDebug() << "Message received:" << message;

}

void PayfluxoService::sendBlockDownloadedMessage(QString ip, QString magneticLink, QString fileSize)
{
    QJsonObject dataObj;
    dataObj.insert("uploaderIp", ip);
    dataObj.insert("magneticLink", magneticLink);
    dataObj.insert("fileSize", fileSize);
    QJsonObject messageObj;
    messageObj.insert("type", QString("DownloadedBlock"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray serializedMessage = messageDoc.toJson();
    this->sendMessage(QString(serializedMessage));
}

void PayfluxoService::sendAuthenticatedMessage(QString certificate, QString privateKey, QString orgMSP)
{
    QJsonObject dataObj;
    dataObj.insert("certificate", certificate);
    dataObj.insert("privateKey", privateKey);
    dataObj.insert("orgMSP", orgMSP);
    QJsonObject messageObj;
    messageObj.insert("type", QString("Authenticated"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray serializedMessage = messageDoc.toJson();
    this->sendMessage(QString(serializedMessage));
}

void PayfluxoService::sendDeauthMessage()
{
    QJsonObject dataObj;
    dataObj.insert("message", "Loging out");
    QJsonObject messageObj;
    messageObj.insert("type", QString("Logout"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray serializedMessage = messageDoc.toJson();
    this->sendMessage(QString(serializedMessage));
}

void PayfluxoService::sendCloseMessage()
{
    QJsonObject dataObj;
    dataObj.insert("message", "Closing Torrente");
    QJsonObject messageObj;
    messageObj.insert("type", QString("Closing"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray serializedMessage = messageDoc.toJson();
    this->sendMessage(QString(serializedMessage));
}

void PayfluxoService::sendMessage(QString message)
{
    m_webSocket.sendTextMessage(message);
}
