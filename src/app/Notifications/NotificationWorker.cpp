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
#include "NotificationWorker.h"
#include <QFile>
#include <QDateTime>
#include "Common/Paths.h"
#include "app/Accounts/AccountUtils.h"

namespace Dekko
{
namespace Notifications
{
NotificationWorker::NotificationWorker(QObject *parent) :
    QObject(parent)
{
}

void NotificationWorker::openConnections()
{
    QString msg = QLatin1String("OPEN CONNECTIONS CALLED");
    emit logMessage(msg);
    QDateTime currentTime = QDateTime::currentDateTime();
    if (m_accounts.size() > 0) {
        Q_FOREACH(MailboxWatcher *watcher, m_watchers) {
            watcher->setStartDateTime(currentTime);
        }

        Q_FOREACH(Accounts::Account *account, m_accounts) {
            static_cast<Imap::Mailbox::NetworkWatcher *>(account->network())->setNetworkOnline();
        }
        return;
    }
    QSettings m_settings;
    QStringList groups;
    m_settings.beginGroup("Accounts");
    groups = m_settings.childGroups();
    m_settings.endGroup();
    if (!groups.length()) {
        qWarning() << "No accounts configured, cannot start notification worker.";
        return;
    }
    Settings::OfflineSettings *prefs = new Settings::OfflineSettings();
    prefs->setAccountId(Dekko::Settings::Id::globalId);
    Q_FOREACH(const QString &group, groups) {
        Accounts::Account *account = new Accounts::Account(this);
        account->setAccountId(group);
        QString cacheDir = Common::writablePath(Common::LOCATION_CACHE) + QLatin1String("notifications/") + group + QLatin1Char('/');
        account->setCacheDir(cacheDir);
        if (!static_cast<Dekko::Settings::ImapSettings *>(
                    account->imapSettings())->server().isEmpty()) {
            // So for each account we need a mailbox watcher and a password manager
            MailboxWatcher *watcher = new MailboxWatcher(this);
            watcher->setStartDateTime(currentTime);
            watcher->setMailbox(QString("INBOX")); // TODO: we should make this configurable and allow more than one mailbox to be watched.
            connect(watcher, SIGNAL(newMessage()), this, SIGNAL(newMessage()));
            watcher->setAccount(account);
            Accounts::PasswordManager *pwdManager = new Accounts::PasswordManager(this);
            pwdManager->setAccount(account);
            pwdManager->setType(Accounts::PasswordManager::IMAP);
            connect(pwdManager, SIGNAL(passwordNowAvailable(QVariant)), this, SLOT(setAccountPassword(QVariant)));
            m_passwordManagers.append(pwdManager);
            account->openImapConnection();
            connect(account->imapModel(),
                    SIGNAL(authRequested()),
                    this,
                    SLOT(passwordRequested()));
            connect(account,
                    SIGNAL(checkSslPolicy()),
                    this,
                    SLOT(setSslPolicy()));
            connect(account->imapModel(),
                    SIGNAL(connectionStateChanged(QObject*, Imap::ConnectionState)),
                    watcher,
                    SLOT(onConnectionStateChanged(QObject*,Imap::ConnectionState)));
            connect(watcher, SIGNAL(newMessages(PostalMessage*)), this, SIGNAL(newMessage(PostalMessage*)));
            connect(watcher, SIGNAL(messageNowMarkedRead(PostalMessage*)), this, SIGNAL(messageNowMarkedRead(PostalMessage*)));
            connect(watcher, SIGNAL(countChanged()), this, SLOT(updateUnreadCount()));
            m_watchers.append(watcher);
            connect(account->imapModel(),
                    SIGNAL(imapError(QString)),
                    this,
                    SLOT(reconnect(QString)));
            connect(account->imapModel(),
                    SIGNAL(networkError(QString)),
                    this,
                    SLOT(reconnect(QString)));
            connect(account->imapModel(),
                    SIGNAL(alertReceived(QString)),
                    this,
                    SLOT(reconnect(QString)));

        }
        m_accounts.append(account);
    }
}

void NotificationWorker::setNetworkPolicy(const bool online)
{
    Q_FOREACH(Accounts::Account *account, m_accounts) {
        if (online) {
            static_cast<Imap::Mailbox::NetworkWatcher *>(account->network())->setNetworkOnline();
        } else {
            static_cast<Imap::Mailbox::NetworkWatcher *>(account->network())->setNetworkOffline();
        }
    }
}

void NotificationWorker::closeConnections()
{
    Q_FOREACH(Accounts::Account *account, m_accounts) {
        qDebug() << "NotificationWorker >>> closing connections.";
        static_cast<Imap::Mailbox::NetworkWatcher *>(account->network())->setNetworkOffline();
    }
}

void NotificationWorker::passwordRequested()
{
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(Accounts::Account *account, m_accounts) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            Q_FOREACH(Accounts::PasswordManager *pm, m_passwordManagers) {
                QString accountId = static_cast<Accounts::Account*>(pm->account())->accountId();
                if( accountId == account->accountId()) {
                    pm->requestPassword();
                }
            }
            return;
        }
    }
}

void NotificationWorker::setAccountPassword(QVariant pwdObject)
{
    QMap<QString, QVariant> pwd = pwdObject.toMap();
    QString accountId = pwd.value(QStringLiteral("accountId")).toString();
    QString password = pwd.value(QStringLiteral("password")).toString();
    Q_FOREACH(Accounts::Account *account, m_accounts) {
        if (accountId == account->accountId()) {
            static_cast<Imap::Mailbox::Model *>(account->imapModel())->setImapPassword(password);
            return;
        }
    }
}

void NotificationWorker::setSslPolicy()
{
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(Accounts::Account *account, m_accounts) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            account->setSslPolicy(true); // TODO: find a way for the user to accept this
            return;
        }
    }
}

void NotificationWorker::updateUnreadCount()
{
    int count = 0;
    Q_FOREACH(Accounts::Account *account, m_accounts) {
        count += static_cast<Imap::Mailbox::MsgListModel *>(account->msgListModel())->unreadCount();
    }
    emit unreadCountChanged(count);
}

void NotificationWorker::reconnect(const QString &message)
{
    qDebug() << ">>> NotificationWorker > Either an IMAP or NetworkError: " << message;
    closeConnections();
    openConnections();
}
}
}
