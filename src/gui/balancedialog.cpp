/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2013  Nick Tiskov <daymansmail@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#include "base/payfluxo/payfluxo.h"

#include "balancedialog.h"

#include "ui_balancedialog.h"
#include "utils.h"

#include <iostream>

BalanceDialog::BalanceDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::BalanceDialog)
{
    m_ui->setupUi(this);
    Payfluxo::Session *payfluxoInstance = Payfluxo::Session::instance();

    payfluxoInstance->getService()->sendRefreshWalletMessage();

    QString certificateString = payfluxoInstance->getCertificate();

    m_ui->publicKey->setText(this->getSimplifiedPublicKey(certificateString));

    this->refreshWallet();

    this->setWindowTitle(tr("User Balance"));

    QObject::connect(payfluxoInstance, &Payfluxo::Session::walletUpdated, this, &BalanceDialog::refreshWallet);
}

void BalanceDialog::refreshWallet() {
    Payfluxo::Session* payfluxoInstance = Payfluxo::Session::instance();

    float redeemableCoins = payfluxoInstance->getRedeemableCoins();
    float availableCoins = payfluxoInstance->getAvailableCoins();
    float frozenCoins = payfluxoInstance->getFrozenCoins();

    this->updateWalletUI(availableCoins, frozenCoins, redeemableCoins);
}

void BalanceDialog::updateWalletUI(float newAvailable, float newFrozen, float newRedeemable){
    m_ui->redeemBalance_value->setText(QString::number(newRedeemable));
    m_ui->globalBalance_value->setText(QString::number(newAvailable));
    m_ui->downloadBalance_value->setText(QString::number(newFrozen));
}

BalanceDialog::~BalanceDialog()
{
    delete m_ui;
}

QString BalanceDialog::getSimplifiedPublicKey(QString publicKeyString){
    publicKeyString = publicKeyString.remove("\"-----BEGIN CERTIFICATE-----\\n");
    publicKeyString = publicKeyString.remove("\\n-----END CERTIFICATE-----\\n\"");

    QString firstPart = publicKeyString.left(5);
    QString lastPart = publicKeyString.right(5);

    return firstPart.append("...").append(lastPart);
}

void BalanceDialog::flushCredentials()
{
    Payfluxo::Session::instance()->logout();

    this->close();
}

void BalanceDialog::invokeRedeem()
{
    Payfluxo::Session::instance()->redeemCoins();
}
