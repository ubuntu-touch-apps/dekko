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
#ifndef MAILBOXPROXYMODEL_H
#define MAILBOXPROXYMODEL_H

#include "3rdParty/RowsJoinerProxy.h"
#include <QObject>
#include <QAbstractItemModel>
#include "app/Accounts/Account.h"
#include "app/Accounts/AccountsManager.h"

namespace Dekko
{
namespace Accounts
{
class Account;
class AccountsManager;
}
namespace Models
{
class MailboxProxyModel : public RowsJoinerProxy
{
    Q_OBJECT
    Q_PROPERTY(QList<QAbstractItemModel *> mailboxModels READ mailboxModels WRITE setMailboxModels NOTIFY modelsChanged)

public:
    explicit MailboxProxyModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    QList<QAbstractItemModel *> mailboxModels() const;
    void setMailboxModels(QList<QAbstractItemModel *> mboxList);
    /* @short returns model at index */
    Q_INVOKABLE QObject *get(int index) const;
    /* @short returns the Accounts::Account the model at index belongs to */
    Q_INVOKABLE QObject *getAccountForIndex(int index) const;
    /*@short returns the Accounts::Account object and set the current mailbox name */
    Q_INVOKABLE QObject *getAccountAndSetMailbox(int index, const QString &mbox) const;

signals:
    void modelsChanged();

private:
    QList<QAbstractItemModel *> m_mboxModels;

    Accounts::Account *getAccount(int index) const;
    QHash<int, QByteArray> m_roles;

};
}
}
#endif // MAILBOXPROXYMODEL_H
