#ifndef PAYFLUXOSESSION_H
#define PAYFLUXOSESSION_H

#include <QWebSocket>

class PayfluxoService : public QObject {
    Q_OBJECT
private:
    bool m_debug;
    QWebSocket m_webSocket;
    void sendMessage(QString message);

    void handlePaymentNotification(QString ip, int blocksPaid);
    void handleIntentionDeclaredNotification(QString torrentIdString, int status);
    void handleWalletNotification(float newAvailable, float newFrozen, float newRedeemable);

public:
    explicit PayfluxoService(bool debug, QObject* parent);
    void onConnected();
    void closed();
    void onWebSocketError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(QString message);
    void sendBlockDownloadedMessage(QString ip, QString torrentId, QString fileSize);
    void sendAuthenticatedMessage(QString certificate, QString privateKey, QString orgMSP);
    void sendDeauthMessage();
    void sendDownloadIntentionMessage(QString magneticLink, int piecesNumber, QString torrentId);
    void sendCloseMessage();
    void sendRedeemMessage();
    void sendRefreshWalletMessage();
};
#endif
