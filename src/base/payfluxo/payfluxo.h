#include <QObject>

#include "payfluxoservice.h"

namespace Payfluxo {
    class Session : public QObject {
        Q_OBJECT
        Q_DISABLE_COPY(Session)

    public:
        static void initInstance(PayfluxoService* session);
        static void deleteInstance();
        static Session* instance();

        PayfluxoService* getService();
        bool authenticate(QString password, QString certificatePath);
        bool isAuthenticated();

        void increaseIpBlocksDownloaded(QString ip);
        void setIpMadePayment(QString ip, int madePayments);
        void clearIpPaymentRegistry(QString ip);
        int  getIpPendentPayment(QString ip);
        bool ipExceededPendentPayment(QString ip);

        void declareDownloadIntention(QString magneticLink, int piecesNumber, QString torrentId);

        float getAvailableCoins();
        float getFrozenCoins();
        float getRedeemableCoins();

        void redeemCoins();

        void setAvailableCoins(float newAmount);
        void setFrozenCoins(float newAmount);
        void setRedeemableCoins(float newAmount);

        void updateWallet(float newAvailable, float newFrozen, float newRedeemable);
        //void updateUser(QString certificate, QString orgMSPId);

        QString getCertificate();

        void logout();
        void closePayfluxo();

        void NotifyFailed();
        void NotifyAuthFailed();
        void NotifyAuthentication(QString certificate, QString orgMSPid);

        static Session* m_instance;

    signals:
        void walletUpdated();
        void NATFailed();
        void authenticationFailed();
        void authenticationSucceeded();

    private:
        QString m_userDecryptedCertificateString;
        QString m_userMSPIdString;

        float m_availableCoins;
        float m_frozenCoins;
        float m_redeemableCoins;

        PayfluxoService* m_payfluxoService;
        QHash<QString, int> m_ipPaymentsMade;
        QHash<QString, int> m_ipBlocksDownloaded;
        bool m_authenticated;

        explicit Session();
    };
}
