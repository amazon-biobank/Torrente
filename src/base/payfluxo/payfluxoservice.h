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
    void handleAuthNotification(QString certificate, QString orgMSPID);
    void handleAuthFailedNotification();

public:
    explicit PayfluxoService(bool debug, QObject* parent);
    void onConnected();
    void closed();
    void onWebSocketError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(QString message);
    void sendBlockDownloadedMessage(QString ip, QString torrentId, QString fileSize);
    void sendAuthenticationMessage(QString encryptedContent, QString salt, QString password);
    void sendDeauthMessage();
    void sendDownloadIntentionMessage(QString magneticLink, int piecesNumber, QString torrentId);
    void sendCloseMessage();
    void sendRedeemMessage();
    void sendRefreshWalletMessage();
};
#endif
