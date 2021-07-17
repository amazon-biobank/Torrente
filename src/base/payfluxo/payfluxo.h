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

        void declareDownloadIntention(QString magneticLink, int piecesNumber, QString torrentId);

        float getAvailableCoins();
        float getFrozenCoins();
        float getRedeemableCoins();

        void setAvailableCoins(float newAmount);
        void setFrozenCoins(float newAmount);
        void setRedeemableCoins(float newAmount);

        QString getCertificate();

        void logout();
        void closePayfluxo();

        static Session* m_instance;

    private:
        QString m_userDecryptedCertificateString;
        QString m_userDecryptedPrivateKeyString;
        QString m_userMSPIdString;

        float m_availableCoins;
        float m_frozenCoins;
        float m_redeemableCoins;

        PayfluxoService* m_payfluxoService;
        QHash<QString, int> m_ipPaymentPendencies;
        bool m_authenticated;

        explicit Session();
    };
}
