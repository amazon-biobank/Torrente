#include "payfluxoservice.h"
#include "payfluxo.h"
#include "base/bittorrent/session.h"
#include "base/bittorrent/infohash.h"

#include <QWebSocket>
#include <QUrl>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#define PAYFLUXO_SOCKET "ws://127.0.0.1:7933"
#define RECONNECT_WAIT 100

#define RECOVERY_FROM_BLACK_LIST_TOLERANCE 1024000

#define INTENTION_DECLARATION_SUCCESS 0
#define INTENTION_DECLARATION_NO_FUNDS 1

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

void PayfluxoService::handlePaymentNotification(QString ip, int blocksPaid)
{
    Payfluxo::Session* session = Payfluxo::Session::instance();

    session->setIpMadePayment(ip, blocksPaid);
    session->setRedeemableCoins(session->getRedeemableCoins() + 1);

    if (session->getIpPendentPayment(ip) < RECOVERY_FROM_BLACK_LIST_TOLERANCE)
        BitTorrent::Session::instance()->unbanIP(ip);
}

void PayfluxoService::handleIntentionDeclaredNotification(QString torrentIdString, int status)
{
    Payfluxo::Session* session = Payfluxo::Session::instance();

    BitTorrent::TorrentID torrentId = BitTorrent::TorrentID::fromString(torrentIdString);
    auto* const torrent = BitTorrent::Session::instance()->findTorrent(torrentId);

    switch (status)
    {
        case INTENTION_DECLARATION_SUCCESS:
            torrent->resume();
            break;
        case INTENTION_DECLARATION_NO_FUNDS:
            break;
    }
}

void PayfluxoService::handleWalletNotification(float newAvailable, float newFrozen, float newRedeemable)
{
    Payfluxo::Session* session = Payfluxo::Session::instance();

    session->updateWallet(newAvailable, newFrozen, newRedeemable);
}

void PayfluxoService::handleAuthFailedNotification()
{
    Payfluxo::Session::instance()->NotifyAuthFailed();
}

void PayfluxoService::handleAuthNotification(QString certificate, QString orgMSPID)
{
    Payfluxo::Session* session = Payfluxo::Session::instance();

    session->NotifyAuthentication(certificate, orgMSPID);
}

void PayfluxoService::onTextMessageReceived(QString message)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    QString type = jsonObject["type"].toString();

    QJsonObject dataJson = jsonObject["data"].toObject();

    if (QString::compare(type, "PaymentNotification", Qt::CaseSensitive) == 0) {
        this->handlePaymentNotification(dataJson["payerIp"].toString(), dataJson["blocksPaid"].toInt());
    }

    else if (QString::compare(type, "IntentionDeclaredNotification", Qt::CaseSensitive) == 0) {
        this->handleIntentionDeclaredNotification(
            dataJson["torrentId"].toString(),
            dataJson["status"].toInt()
        );
    }

    else if (QString::compare(type, "WalletNotification", Qt::CaseSensitive) == 0) {
        QJsonObject walletObject = dataJson["wallet"].toObject();
        this->handleWalletNotification(
            walletObject["available"].toDouble(),
            walletObject["frozen"].toDouble(),
            walletObject["redeemable"].toDouble()
        );
    }

    else if (QString::compare(type, "NATNotifcation", Qt::CaseSensitive) == 0) {
        Payfluxo::Session::instance()->NotifyFailed();
    }

    else if (QString::compare(type, "AuthenticationNotification", Qt::CaseSensitive) == 0) {
        this->handleAuthNotification(
            dataJson["certificate"].toString(),
            dataJson["orgMSPID"].toString()
        );
    }

    else if (QString::compare(type, "AuthenticationFailedNotification", Qt::CaseSensitive) == 0) {
        this->handleAuthFailedNotification();
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

void PayfluxoService::sendAuthenticationMessage(QString encryptedContent, QString salt, QString password)
{
    QJsonObject dataObj;
    dataObj.insert("encrypted_content", encryptedContent);
    dataObj.insert("salt", salt);
    dataObj.insert("password", password);
    QJsonObject messageObj;
    messageObj.insert("type", QString("Authentication"));
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

void PayfluxoService::sendDownloadIntentionMessage(QString magneticLink, int piecesNumber, QString torrentId)
{
    QJsonObject dataObj;
    dataObj.insert("magneticLink", magneticLink);
    dataObj.insert("piecesNumber", piecesNumber);
    dataObj.insert("torrentId", torrentId);
    QJsonObject messageObj;
    messageObj.insert("type", QString("DownloadIntention"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray serializedMessage = messageDoc.toJson();
    this->sendMessage(QString(serializedMessage));
}

void PayfluxoService::sendRedeemMessage()
{
    QJsonObject dataObj;
    dataObj.insert("message", "Manual redeem request");
    QJsonObject messageObj;
    messageObj.insert("type", QString("RedeemValues"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray serializedMessage = messageDoc.toJson();
    this->sendMessage(QString(serializedMessage));
}

void PayfluxoService::sendRefreshWalletMessage()
{
    QJsonObject dataObj;
    dataObj.insert("message", "Refresh wallet content");
    QJsonObject messageObj;
    messageObj.insert("type", QString("RefreshWallet"));
    messageObj.insert("data", dataObj);
    QJsonDocument messageDoc(messageObj);
    QByteArray serializedMessage = messageDoc.toJson();
    this->sendMessage(QString(serializedMessage));
}

void PayfluxoService::sendMessage(QString message)
{
    m_webSocket.sendTextMessage(message);
}
