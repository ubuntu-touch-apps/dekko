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
#ifndef NOTIFICATIONWORKER_H
#define NOTIFICATIONWORKER_H

#include <QObject>
#include <QDebug>
#include "PostalServiceWorker.h"
#include "Common/ConnectionMethod.h"
#include "Imap/Model/Model.h"
#include "Imap/Model/MsgListModel.h"
#include "Imap/Model/NetworkWatcher.h"
#include "app/Settings/Settings.h"
#include "app/Accounts/Account.h"
#include "app/Accounts/PasswordManager.h"
#include "app/Components/QmlObjectListModel.h"
#include "MailboxWatcher.h"

namespace Dekko
{
namespace Notifications
{
class MailboxWatcher;
class PostalMessage;
class NotificationWorker : public QObject
{
    Q_OBJECT
public:
    explicit NotificationWorker(QObject *parent = 0);

signals:
    void newMessage(PostalMessage *message);
    void messageNowMarkedRead(PostalMessage *message);
    void unreadCountChanged(int count);
    void logMessage(QString message);

public slots:
    void openConnections();
    void setNetworkPolicy(const bool online);
    void closeConnections();

private slots:
    void passwordRequested();
    void setAccountPassword(QVariant pwdObject);
    void setSslPolicy();
    void updateUnreadCount();
    void reconnect(const QString &message);

private:
    QList<Accounts::PasswordManager *> m_passwordManagers;
    QList<Accounts::Account *> m_accounts;
    QList<MailboxWatcher *> m_watchers;

};
}
}
#endif // NOTIFICATIONWORKER_H
