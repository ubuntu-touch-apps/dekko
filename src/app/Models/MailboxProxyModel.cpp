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
#include "MailboxProxyModel.h"
#include "Imap/Model/SubtreeModel.h"
#include <QDebug>
namespace Dekko
{
namespace Models
{
MailboxProxyModel::MailboxProxyModel(QObject *parent) :
    RowsJoinerProxy(parent)
{
}

QHash<int, QByteArray> MailboxProxyModel::roleNames() const
{
    return m_roles;
}

QList<QAbstractItemModel *> MailboxProxyModel::mailboxModels() const
{
    return m_mboxModels;
}

void MailboxProxyModel::setMailboxModels(QList<QAbstractItemModel *> mboxList)
{
    if (mboxList.size() <= 0) {
        return;
    } else if (RowsJoinerProxy::models().size() > 0) {
        // We need to remove all these and basically do a resetModel
        // Since this is bound to the modelsChanged property binding in qml this will get reevaluated
        // after every models changed signal from the accountsmanager
        Q_FOREACH(QAbstractItemModel *model, RowsJoinerProxy::models()) {
            Q_ASSERT(model);
            removeSourceModel(model);
        }
    }
    m_mboxModels = mboxList;
    Q_FOREACH(QAbstractItemModel *aim, m_mboxModels) {
        Imap::Mailbox::SubtreeModelOfMailboxModel *model = qobject_cast<Imap::Mailbox::SubtreeModelOfMailboxModel *>(aim);
        Q_ASSERT(model);
        insertSourceModel(model);
        if (m_roles.isEmpty()) {
            m_roles = model->roleNames();
        }
    }
    emit modelsChanged();
}

QObject *MailboxProxyModel::get(int index) const
{
    return m_mboxModels.at(index);
}

QObject *MailboxProxyModel::getAccountForIndex(int index) const
{
    return getAccount(index);
}

QObject *MailboxProxyModel::getAccountAndSetMailbox(int index, const QString &mbox) const
{
    Accounts::Account *account = getAccount(index);
    // Now set the mailbox
    static_cast<Imap::Mailbox::MsgListModel *>(account->msgListModel())->setMailbox(mbox);
    return account;
}

Accounts::Account *MailboxProxyModel::getAccount(int index) const
{
    QObject *mboxModel = m_mboxModels.at(index);
    Accounts::Account *account = qobject_cast<Accounts::Account *>(mboxModel->parent());
    Q_ASSERT(account);
    return account;
}

}
}
