/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2006  Christophe Dumez <chris@torrente.org>
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

#include "downloadedpiecesbar.h"

#include <cmath>

#include <QDebug>
#include <QVector>

namespace
{
    QColor dlPieceColor(const QColor &pieceColor)
    {
        const QColor green {Qt::green};
        return QColor::fromHsl(green.hslHue(), pieceColor.hslSaturation(), pieceColor.lightness());
    }
}

DownloadedPiecesBar::DownloadedPiecesBar(QWidget *parent)
    : base {parent}
    , m_dlPieceColor {dlPieceColor(pieceColor())}
{
}

QVector<float> DownloadedPiecesBar::bitfieldToFloatVector(const QBitArray &vecin, int reqSize)
{
    QVector<float> result(reqSize, 0.0);
    if (vecin.isEmpty()) return result;

    const float ratio = vecin.size() / static_cast<float>(reqSize);

    // simple linear transformation algorithm
    // for example:
    // image.x(0) = pieces.x(0.0 >= x < 1.7)
    // image.x(1) = pieces.x(1.7 >= x < 3.4)

    for (int x = 0; x < reqSize; ++x)
    {
        // R - real
        const float fromR = x * ratio;
        const float toR = (x + 1) * ratio;

        // C - integer
        int fromC = fromR; // std::floor not needed
        int toC = std::ceil(toR);
        if (toC > vecin.size())
            --toC;

        // position in pieces table
        int x2 = fromC;

        // little speed up for really big pieces table, 10K+ size
        const int toCMinusOne = toC - 1;

        // value in returned vector
        float value = 0;

        // case when calculated range is (15.2 >= x < 15.7)
        if (x2 == toCMinusOne)
        {
            if (vecin[x2])
                value += ratio;
            ++x2;
        }
        // case when (15.2 >= x < 17.8)
        else
        {
            // subcase (15.2 >= x < 16)
            if (x2 != fromR)
            {
                if (vecin[x2])
                    value += 1.0 - (fromR - fromC);
                ++x2;
            }

            // subcase (16 >= x < 17)
            for (; x2 < toCMinusOne; ++x2)
                if (vecin[x2])
                    value += 1.0;

            // subcase (17 >= x < 17.8)
            if (x2 == toCMinusOne)
            {
                if (vecin[x2])
                    value += 1.0 - (toC - toR);
                ++x2;
            }
        }

        // normalization <0, 1>
        value /= ratio;

        // float precision sometimes gives > 1, because it's not possible to store irrational numbers
        value = qMin(value, 1.0f);

        result[x] = value;
    }

    return result;
}

bool DownloadedPiecesBar::updateImage(QImage &image)
{
    //  qDebug() << "updateImage";
    QImage image2(width() - 2 * borderWidth, 1, QImage::Format_RGB888);
    if (image2.isNull())
    {
        qDebug() << "QImage image2() allocation failed, width():" << width();
        return false;
    }

    if (m_pieces.isEmpty())
    {
        image2.fill(backgroundColor());
        image = image2;
        return true;
    }

    QVector<float> scaledPieces = bitfieldToFloatVector(m_pieces, image2.width());
    QVector<float> scaledPiecesDl = bitfieldToFloatVector(m_downloadedPieces, image2.width());

    // filling image
    for (int x = 0; x < scaledPieces.size(); ++x)
    {
        float piecesToValue = scaledPieces.at(x);
        float piecesToValueDl = scaledPiecesDl.at(x);
        if (piecesToValueDl != 0)
        {
            float fillRatio = piecesToValue + piecesToValueDl;
            float ratio = piecesToValueDl / fillRatio;

            QRgb mixedColor = mixTwoColors(pieceColor().rgb(), m_dlPieceColor.rgb(), ratio);
            mixedColor = mixTwoColors(backgroundColor().rgb(), mixedColor, fillRatio);

            image2.setPixel(x, 0, mixedColor);
        }
        else
        {
            image2.setPixel(x, 0, pieceColors()[piecesToValue * 255]);
        }
    }
    image = image2;
    return true;
}

void DownloadedPiecesBar::setProgress(const QBitArray &pieces, const QBitArray &downloadedPieces)
{
    m_pieces = pieces;
    m_downloadedPieces = downloadedPieces;

    requestImageUpdate();
}

void DownloadedPiecesBar::clear()
{
    m_pieces.clear();
    m_downloadedPieces.clear();
    base::clear();
}

QString DownloadedPiecesBar::simpleToolTipText() const
{
    const QString borderColor = colorBoxBorderColor().name();
    const QString rowHTML = QString::fromLatin1("<tr><td width=20 bgcolor='%1' style='border: 1px solid \"%2\";'></td><td>%3</td></tr>");
    return QLatin1String("<table cellspacing=4>")
           + rowHTML.arg(backgroundColor().name(), borderColor, tr("Missing pieces"))
           + rowHTML.arg(m_dlPieceColor.name(), borderColor, tr("Partial pieces"))
           + rowHTML.arg(pieceColor().name(), borderColor, tr("Completed pieces"))
           + QLatin1String("</table>");
}
