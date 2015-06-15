/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MAILBOXUTILS_H
#define MAILBOXUTILS_H

#include <QObject>
#include "app/Accounts/Account.h"
#include "app/Settings/MailboxSettings.h"

namespace Dekko
{
namespace Settings
{
class MailboxSettings;
}
namespace Accounts
{
class Account;
}
namespace Utils
{

class MailboxUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *account READ account WRITE setAccount NOTIFY accountChanged)
    Q_ENUMS(Flags)

public:
    explicit MailboxUtils(QObject *parent = 0);

    enum Flags {
        MARKED_READ,
        MARKED_UNREAD,
        MARKED_FLAGGED,
        NOT_FLAGGED
    };

    QObject *account();
    void setAccount(QObject *account);

    Q_INVOKABLE bool markMessageAsRead(const QModelIndex &message, bool marked);
    Q_INVOKABLE bool markSelectedMessagesAsRead(bool marked);
    Q_INVOKABLE bool markMessageDeleted(const QModelIndex &message, bool marked);
    Q_INVOKABLE bool markSelectedMessagesDeleted(bool mark);
    Q_INVOKABLE bool markMessageAsFlagged(const QModelIndex &message, bool marked);
    Q_INVOKABLE bool markSelectedMessagesFlagged(bool mark);
    Q_INVOKABLE QModelIndex deproxifiedIndex(const QModelIndex index);
    Q_INVOKABLE bool moveMessageToMailbox(const QString &currentMailbox, const uint &uid, const QString &targetMailbox);
    Q_INVOKABLE bool expungeMailboxByName(const QString &mboxName);
    Q_INVOKABLE bool markMailboxAsRead(const QString &mboxName);

signals:
    void accountChanged();
    void moveOrDeleteEventDispatched();
    void flagChangeEventDispatched();

public slots:
    void appendToSelectedIndexList(const QModelIndex &index);
    void appendToSelectedModelIndexList(const QList<QModelIndex> &indexList);
    void removeFromSelectedIndexList(const QModelIndex &index);
    void clearSelectedMessages();
    // gets the total number of messages with the given Flag
    int getTotalForMessageFlag(const QList<QModelIndex> &indexList, Flags flag);

private:
    bool determineMoveSelectedMessagesToTrashOrMarkDeleted(const QModelIndexList &msgs, const bool mark);

private:
    Accounts::Account *m_account;
    QModelIndexList m_selectedIndexList;
    Settings::MailboxSettings *m_mboxSettings;

};
}
}
#endif // MAILBOXUTILS_H
