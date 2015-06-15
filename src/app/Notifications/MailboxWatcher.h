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
#ifndef MAILBOXWATCHER_H
#define MAILBOXWATCHER_H

#include <QObject>
#include <QDateTime>
#include "Imap/ConnectionState.h"
#include "app/Accounts/Account.h"
#include "app/Utils/SearchForSpecialMailbox.h"
#include "PostalServiceWorker.h"
#include "3rdParty/trojita/Imap/Model/MsgListModel.h"
#include "3rdParty/trojita/Imap/Model/Model.h"
#include "3rdParty/trojita/Imap/Model/ItemRoles.h"
#include "3rdParty/trojita/Imap/Model/MailboxTree.h"

namespace Imap {
namespace Mailbox {
class Model;
class TreeItemMailbox;
class TreeItemMsgList;
class TreeItem;
class MsgListModel;
}
}

namespace Dekko
{
namespace Accounts {
class Account;
}
namespace Notifications
{
class PostalMessage;
class MailboxWatcher : public QObject
{
    Q_OBJECT
public:
    explicit MailboxWatcher(QObject *parent = 0);

    void setAccount(Accounts::Account *account);
    void setMailbox(const QString &mailbox);
    void setStartDateTime(const QDateTime &dateTime);

signals:
    void newMessages(PostalMessage *messages);
    void messageNowMarkedRead(PostalMessage *message);
    void countChanged();

public slots:

private slots:
    void onConnectionStateChanged(QObject *parser, Imap::ConnectionState state); // set mailbox when connection is ready
    void onAuthenticatedSetRequestedMailbox();
    void handleUnreadCountPossiblyChanged();
    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
private:
    Accounts::Account *m_account;
    QPointer<Utils::SearchForSpecialMailbox> m_searchForMbox;
    QString m_mailbox;
    Imap::Mailbox::MsgListModel *m_msgListModel;
    int m_previousUnreadCount;
    QList<uint> m_uids;
    QList<uint> m_uidsWaiting;
    QDateTime m_startDateTime;

};
}
}
#endif // MAILBOXWATCHER_H
