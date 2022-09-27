#include "payfluxo.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>

using namespace Payfluxo;

#define PENDENCE_TOLERANCE 1024000

Session::Session() {
    m_authenticated = false;

    m_userDecryptedCertificateString = QString("");
    m_userMSPIdString = QString("");
}

void Session::initInstance(PayfluxoService* service) {
    if (!m_instance)
        m_instance = new Session;
    m_instance->m_payfluxoService = service;
}

void Session::deleteInstance() {
    delete m_instance;
    m_instance = nullptr;
}

Session* Session::instance() {
    return m_instance;
}

bool Session::authenticate(QString password, QString certificatePath) {
    QFile encryptedCredentialsFile = QFile(certificatePath);
    if (!encryptedCredentialsFile.open(QFile::ReadOnly | QFile::Text))
        return true;
    QTextStream in(&encryptedCredentialsFile);
    QString encryptedCredentialsContent = in.readAll();
    QJsonDocument encryptedCredentials = QJsonDocument::fromJson(encryptedCredentialsContent.toUtf8());

    QString encryptedContent = encryptedCredentials["encrypted_content"].toString();
    QString salt = encryptedCredentials["salt"].toString();

    m_payfluxoService->sendAuthenticationMessage(encryptedContent, salt, password);

    // TODO: Recover from safe persistence
    m_availableCoins = 0;
    m_frozenCoins = 0;
    m_redeemableCoins = 0;

    return false;
}

void Session::logout() {
    m_userDecryptedCertificateString = nullptr;
    m_userMSPIdString = nullptr;

    m_authenticated = false;

    m_payfluxoService->sendDeauthMessage();
}

void Session::closePayfluxo() {
    m_payfluxoService->sendCloseMessage();
}

bool Session::isAuthenticated() {
    return m_authenticated;
}

PayfluxoService* Session::getService() {
    return m_payfluxoService;
}

void Session::setIpMadePayment(QString ip, int madePayments) {
    this->m_ipPaymentsMade[ip] = madePayments;
}

void Session::increaseIpBlocksDownloaded(QString ip) {
    this->m_ipBlocksDownloaded[ip]++;
}

void Session::clearIpPaymentRegistry(QString ip) {
    this->m_ipPaymentsMade.remove(ip);
    this->m_ipBlocksDownloaded.remove(ip);
}

bool Session::ipExceededPendentPayment(QString ip) {
    int pendentPayments = this->m_ipBlocksDownloaded[ip] - this->m_ipPaymentsMade[ip];
    return pendentPayments >= PENDENCE_TOLERANCE;
}

int Session::getIpPendentPayment(QString ip) {
    return this->m_ipBlocksDownloaded[ip] - this->m_ipPaymentsMade[ip];
}

QString Session::getCertificate() {
    return m_userDecryptedCertificateString;
}

float Session::getAvailableCoins() {
    return m_availableCoins;
}

float Session::getFrozenCoins() {
    return m_frozenCoins;
}

float Session::getRedeemableCoins() {
    return m_redeemableCoins;
}

void Session::setAvailableCoins(float newAmount) {
    m_availableCoins = newAmount;
}

void Session::setFrozenCoins(float newAmount) {
    m_frozenCoins = newAmount;
}

void Session::setRedeemableCoins(float newAmount) {
    m_redeemableCoins = newAmount;
}

void Session::declareDownloadIntention(QString magneticLink, int piecesNumber, QString torrentId) {
    m_payfluxoService->sendDownloadIntentionMessage(magneticLink, piecesNumber, torrentId);
}

void Session::redeemCoins() {
    m_payfluxoService->sendRedeemMessage();
}

void Session::updateWallet(float newAvailable, float newFrozen, float newRedeemable) {
    this->setAvailableCoins(newAvailable);
    this->setFrozenCoins(newFrozen);
    this->setRedeemableCoins(newRedeemable);

    emit walletUpdated();
}

void Session::NotifyFailed(){
    emit NATFailed();
}

void Session::NotifyAuthFailed() {
    emit authenticationFailed();
}

void Session::NotifyAuthentication(QString certificate, QString orgMSPid) {
    m_userDecryptedCertificateString = certificate;
    m_userMSPIdString = orgMSPid;

    m_authenticated = true;

    emit authenticationSucceeded();
}

Session* Session::m_instance = nullptr;
