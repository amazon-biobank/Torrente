#ifndef PAYFLUXOSESSION_H
#define PAYFLUXOSESSION_H

#include <QWebSocket>

class PayfluxoSession : public QObject {

private:
    bool m_debug;
    QWebSocket m_webSocket;
    void sendMessage(QString message);

public:
    explicit PayfluxoSession(bool debug, QObject* parent);
    void onConnected();
    void closed();
    void handlePaymentNotification(QString ip);
    void onTextMessageReceived(QString message);
    void sendBlockDownloadedMessage(QString ip, QString torrentId, QString fileSize);

};
#endif 

