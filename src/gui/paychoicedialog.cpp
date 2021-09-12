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
#include <QListWidget>
#include <QApplication>
#include <QMessageBox>

#include "base/bittorrent/session.h"
#include "base/payfluxo/payfluxo.h"
#include "base/bittorrent/torrent.h"
#include "base/bittorrent/torrentimpl.h"
#include "base/bittorrent/infohash.h"

#include "paychoicedialog.h"

#include "ui_paychoicedialog.h"
#include "utils.h"

#include <iostream>
#include <string>

PayChoiceDialog::PayChoiceDialog(QWidget* parent, QVector<BitTorrent::Torrent *> notPaidTorrents)
    : QDialog(parent)
    , m_ui(new Ui::PayChoiceDialog)
{
    m_ui->setupUi(this);

    QStringList torrentsNames;

    torrentOptions = notPaidTorrents;

    int amountOfTorrents = notPaidTorrents.count();
    for (int index = 0; index < amountOfTorrents; index++)
    {
        torrentsNames << notPaidTorrents[index]->name();
    }

    m_ui->torrentChoices->addItems(torrentsNames);

    QListWidgetItem* item = 0;
    for (int i = 0; i < m_ui->torrentChoices->count(); ++i) {
        item = m_ui->torrentChoices->item(i);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    m_ui->SelectButton->setText(tr("Select"));
    this->setWindowTitle(tr("Convert torrents to paid mode"));
}

PayChoiceDialog::~PayChoiceDialog()
{
    delete m_ui;
}


void PayChoiceDialog::onSelectButtonClicked()
{
    QListWidgetItem* item = 0;
    for (int i = 0; i < m_ui->torrentChoices->count(); ++i) {
        item = m_ui->torrentChoices->item(i);
        Qt::CheckState torrentState = item->checkState();
        if (torrentState == Qt::CheckState::Checked)
        {
            torrentOptions[i]->turnTorrentPaid();
            if (!torrentOptions[i]->isCompleted()) {
                Payfluxo::Session::instance()->declareDownloadIntention(
                    torrentOptions[i]->createMagnetURI(),
                    torrentOptions[i]->piecesCount(),
                    torrentOptions[i]->id().toString()
                );
            }
        }
    }

    this->close();
}
