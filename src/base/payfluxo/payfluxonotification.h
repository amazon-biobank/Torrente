#ifndef PAYFLUXONOTIFICATION_H
#define PAYFLUXONOTIFICATION_H

#include <QWebSocket>

class PayfluxoNotification : public QObject {

private:
    bool m_debug;
    QWebSocket m_webSocket;

public:
    explicit PayfluxoNotification(bool debug, QObject* parent);
    void onConnected();
    void closed();
    void handlePaymentNotification(QString ip);
    void onTextMessageReceived(QString message);
    
};
#endif 

