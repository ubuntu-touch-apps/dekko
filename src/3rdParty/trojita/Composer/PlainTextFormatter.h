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

#ifndef COMPOSER_PLAINTEXTFORMATTER_H
#define COMPOSER_PLAINTEXTFORMATTER_H

#include <QStringList>

namespace Composer {
namespace Util {

/** @short Enable decoding of format=flowed, RFC 3676 */
typedef enum {
    FORMAT_PLAIN, /**< @short No value, use default */
    FORMAT_FLOWED, /**< @short format=flowed, but DelSp is not active */
    FORMAT_FLOWED_DELSP, /**< @short format=flowed; delsp=yes (see RFC 3676 for details */
} FlowedFormat;

QString plainTextToHtml(const QString &plaintext, const FlowedFormat flowed);

QRegExp signatureSeparator();

QStringList quoteText(QStringList inputLines);

QString lineWithoutTrailingCr(const QString &line);

}
}

#endif // COMPOSER_PLAINTEXTFORMATTER_H
