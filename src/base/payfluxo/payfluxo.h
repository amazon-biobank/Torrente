#include "payfluxoservice.h"

namespace Payfluxo {
    class Session {
    public:
        static void initInstance(PayfluxoService* session);
        static void deleteInstance();
        static Session* instance();

        PayfluxoService* getService();
        bool authenticate(QString password, QString certificatePath);
        bool isAuthenticated();

        void increaseIpPaymentPendent(QString ip);
        void decreaseIpPaymentPendent(QString ip);
        void clearIpPaymentPendency(QString ip);
        int  getIpPendentPayment(QString ip);
        bool ipExceededPendentPayment(QString ip);

        QString getCertificate();

        void logout();

        static Session* m_instance;

    private:
        QString m_userDecryptedCertificateString;
        QString m_userDecryptedPrivateKeyString;
        QString m_userMSPIdString;

        PayfluxoService* m_payfluxoService;
        QHash<QString, int> m_ipPaymentPendencies;
        bool m_authenticated;

        explicit Session();
    };
}
