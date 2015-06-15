/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the Dekko

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

#include "Recipient.h"
#include <QtQml/QQmlEngine>
#include <QDebug>

namespace Dekko
{
namespace Models
{
Recipient::Recipient(QObject *parent) :
    QObject(parent), m_recipientAddress(QString())
{
//    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Recipient::~Recipient()
{
    this->deleteLater();
}

QString Recipient::recipientAddress() const
{
    return m_recipientAddress;
}

void Recipient::setRecipientAddress(const QString &address)
{
    if (address == m_recipientAddress) {
        return;
    }
    m_recipientAddress = address;
    emit dataChanged();
}

Recipient::RecipientKind Recipient::recipientKind() const
{
    return m_recipientKind;
}

void Recipient::setRecipientKind(const RecipientKind &recipientKind)
{
    if (recipientKind == m_recipientKind ) {
        return;
    }
    m_recipientKind = recipientKind;
    emit dataChanged();
}

}
}
