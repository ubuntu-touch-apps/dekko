/* Copyright (C) 2006 - 2014 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TROJITA_UIUTILS_FORMATTING_H
#define TROJITA_UIUTILS_FORMATTING_H

#include <QDateTime>
#include <QModelIndex>
#include <QObject>
#include "Composer/PlainTextFormatter.h"

class QSettings;
class QSslCertificate;
class QSslError;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
class QQmlEngine;
class QJSEngine;
#endif

namespace UiUtils {

class Formatting: public QObject
{
    Q_OBJECT
public:
    enum class BytesSuffix {
        COMPACT_FORM, /**< @short Do not append "B" when the size is less than 1kB */
        WITH_BYTES_SUFFIX /**< @short Always prepend the units, even if it's just in bytes */
    };

    /** @short Shamelessly stolen from QMessageBox */
    enum class IconType {
        NoIcon = 0,
        Information = 1,
        Warning = 2,
        Critical = 3,
        Question = 4
    };

    Q_INVOKABLE static QString prettySize(uint bytes, const BytesSuffix compactUnitFormat = BytesSuffix::COMPACT_FORM);
    Q_INVOKABLE static QString prettyDate(const QDateTime &dateTime);

    Q_INVOKABLE static QString markUpPlainTextInQML(QString plainText);

    static QString sslChainToHtml(const QList<QSslCertificate> &sslChain);
    static QString sslErrorsToHtml(const QList<QSslError> &sslErrors);

    static void formatSslState(const QList<QSslCertificate> &sslChain, const QByteArray &oldPubKey,
                               const QList<QSslError> &sslErrors, QString *title, QString *message, IconType *icon);

    static QByteArray htmlHexifyByteArray(const QByteArray &rawInput);

    static QString markUpPlainText(QString plainText, Composer::Util::FlowedFormat flowedFormat);
    static QFont systemMonospaceFont();
    static QColor tintColor(const QColor &color, const QColor &tintColor);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);
#endif

private:
    Formatting(QObject *parent);
};

}

#endif
