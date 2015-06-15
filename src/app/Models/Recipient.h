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

#ifndef RECIPIENT_H
#define RECIPIENT_H

#include <QObject>
#include "Imap/Parser/MailAddress.h"

namespace Dekko
{
namespace Models
{

class Recipient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString recipientAddress READ recipientAddress WRITE setRecipientAddress NOTIFY dataChanged)
    Q_PROPERTY(RecipientKind recipientKind READ recipientKind WRITE setRecipientKind NOTIFY dataChanged)
    Q_ENUMS(RecipientKind)
public:
    explicit Recipient(QObject *parent = 0);
    ~Recipient();

    enum RecipientKind {
        ADDRESS_TO,
        ADDRESS_CC,
        ADDRESS_BCC,
        ADDRESS_FROM,
        ADDRESS_SENDER,
        ADDRESS_REPLY_TO
    };

    QString recipientAddress() const;
    void setRecipientAddress(const QString &address);

    RecipientKind recipientKind() const;
    void setRecipientKind(const RecipientKind &recipientKind);

signals:
    void dataChanged();

public slots:

private:
    QString m_recipientAddress;
    RecipientKind m_recipientKind;
};
}
}
#endif // RECIPIENT_H
