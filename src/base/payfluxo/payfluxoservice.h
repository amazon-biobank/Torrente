#ifndef PAYFLUXOSESSION_H
#define PAYFLUXOSESSION_H

#include <QWebSocket>

class PayfluxoService : public QObject {

private:
    bool m_debug;
    QWebSocket m_webSocket;
    void sendMessage(QString message);

public:
    explicit PayfluxoService(bool debug, QObject* parent);
    void onConnected();
    void closed();
    void onWebSocketError(QAbstractSocket::SocketError error);
    void handlePaymentNotification(QString ip);
    void onTextMessageReceived(QString message);
    void sendBlockDownloadedMessage(QString ip, QString torrentId, QString fileSize);
    void sendAuthenticatedMessage(QString certificate, QString privateKey, QString orgMSP);
    void sendDeauthMessage();
    void sendCloseMessage();
};
#endif
