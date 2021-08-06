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
#include <QApplication>
#include <QMessageBox>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "base/encryption/fileencryption.h"

#include "decypherdialog.h"

#include "ui_decypherdialog.h"
#include "utils.h"

#define INVALID_CREDENTIALS_EXCEPTION 1
#define NO_FILE_SELECTED 2

DecypherDialog::DecypherDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::DecypherDialog)
{
    m_ui->setupUi(this);

    m_ui->cypheredFileButton->setText(tr("Choose file"));
    m_ui->importKeyButton->setText(tr("Import file key"));
    m_ui->decypherButton->setText(tr("Decrypt file"));
    this->setWindowTitle(tr("Decrypt file"));
}

DecypherDialog::~DecypherDialog()
{
    delete m_ui;
}


void DecypherDialog::onImportFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        QDir::homePath(),
        tr("Cyphered files (*.cyphered)")
    );
    if (fileName == "")
        fileName = "No file chosen";
    m_ui->choosenFileLabel->setText(fileName);
    DecypherDialog::filePath = fileName;
}

void DecypherDialog::onImportKeyButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open key"),
        QDir::homePath(),
        tr("Key Files (*.biobank)")
    );
    if (fileName == "")
        fileName = "No file chosen";
    m_ui->keyPathLabel->setText(fileName);
    DecypherDialog::keyPath = fileName;
}

void DecypherDialog::toggleWidgetsEnable() {
    m_ui->cypheredFileButton->setEnabled(!m_ui->cypheredFileButton->isEnabled());
    m_ui->importKeyButton->setEnabled(!m_ui->importKeyButton->isEnabled());
    m_ui->decypherButton->setEnabled(!m_ui->decypherButton->isEnabled());
}

void DecypherDialog::onDecypherButtonClicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->toggleWidgetsEnable();

    try {
        if (keyPath == "" || filePath == "")
            throw NO_FILE_SELECTED;
        QString jsonKey;
        QFile jsonKeyFile;
        jsonKeyFile.setFileName(keyPath);
        jsonKeyFile.open(QIODevice::ReadOnly | QIODevice::Text);
        jsonKey = jsonKeyFile.readAll();
        jsonKeyFile.close();
        QJsonDocument jsonKeyDocument = QJsonDocument::fromJson(jsonKey.toUtf8());

        QJsonObject jsonObject = jsonKeyDocument.object();
        QString keyString = jsonObject["secret_key"].toString();
        QString tagString = jsonObject["file_tag"].toString();

        Encryption::Encryption::decryptFile(filePath, keyString, tagString);

        QApplication::restoreOverrideCursor();
        this->toggleWidgetsEnable();
        QApplication::restoreOverrideCursor();
        QMessageBox fileDecryptionSuccessDialogBox;
        fileDecryptionSuccessDialogBox.setText("Decryption succeded");
        fileDecryptionSuccessDialogBox.exec();
        this->close();
    }
    catch (int i) {
        // Senha incorreta!
        QApplication::restoreOverrideCursor();
        this->toggleWidgetsEnable();
        QMessageBox fileDecryptionErrorDialogBox;
        fileDecryptionErrorDialogBox.setText("No files selected");
        fileDecryptionErrorDialogBox.exec();
    }
}
