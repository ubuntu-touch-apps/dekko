#include "MailboxWatcher.h"
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
#include "Imap/Parser/MailAddress.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QStringBuilder>
#include "app/Utils/Path.h"

namespace Dekko
{
namespace Notifications
{
MailboxWatcher::MailboxWatcher(QObject *parent) :
    QObject(parent), m_account(0), m_searchForMbox(new Utils::SearchForSpecialMailbox),
    m_msgListModel(0), m_previousUnreadCount(0)
{
}

void MailboxWatcher::setAccount(Accounts::Account *account)
{
    m_account = account;
}

void MailboxWatcher::setMailbox(const QString &mailbox)
{
    m_mailbox = mailbox;
}

void MailboxWatcher::setStartDateTime(const QDateTime &dateTime)
{
    m_startDateTime = dateTime;
}

void MailboxWatcher::onConnectionStateChanged(QObject *parser, Imap::ConnectionState state)
{
    Q_UNUSED(parser);
    qDebug() << "MailboxWatcher::connectionState: " << Imap::connectionStateToString(state);
    if (state == Imap::CONN_STATE_AUTHENTICATED) {
        m_msgListModel = static_cast<Imap::Mailbox::MsgListModel *>(m_account->msgListModel());
        connect(m_msgListModel, SIGNAL(unreadCountPossiblyChanged()), this, SLOT(handleUnreadCountPossiblyChanged()));
        connect(m_account->imapModel(), SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)), this, SLOT(handleDataChanged(const QModelIndex&,const QModelIndex&)));
        QTimer::singleShot(4000, this, SLOT(onAuthenticatedSetRequestedMailbox()));
        return;
    }
}

void MailboxWatcher::onAuthenticatedSetRequestedMailbox()
{
    m_searchForMbox.data()->setModel(qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel()));
    connect(m_searchForMbox.data(), &Utils::SearchForSpecialMailbox::inboxFound, [=](QString mailboxName, QModelIndex mboxIndex) {
        if (mailboxName == m_mailbox) {
            Imap::Mailbox::MsgListModel *msgList = static_cast<Imap::Mailbox::MsgListModel *>(m_account->msgListModel());
            if (msgList) {
                msgList->setMailbox(mboxIndex);
            }
        }
        return;
        disconnect(m_searchForMbox.data(), 0, this, 0);
    });
    qDebug() << ">>> MailboxWatcher: Searching for mailbox";
    m_searchForMbox.data()->start();
}

void MailboxWatcher::handleUnreadCountPossiblyChanged()
{
    int unreadCount = m_msgListModel->unreadCount();
    if (unreadCount > m_previousUnreadCount) {
        m_previousUnreadCount = unreadCount;
        Imap::Mailbox::TreeItemMsgList *msgListPtr = m_msgListModel->msgListPtr;
        for (int i = 0; i < msgListPtr->m_children.size(); ++i) {
            Imap::Mailbox::TreeItemMessage *message = static_cast<Imap::Mailbox::TreeItemMessage *>(msgListPtr->m_children[i]);
            if (!message->isMarkedAsRead()) {
                Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(m_account->imapModel());

                if (!message->fetched() && !m_uids.contains(message->uid())) {
                    qDebug() << ">>> MESSAGE NOT FETCHED >>> REQUESTING NOW!";
                    model->askForMsgMetadata(message, Imap::Mailbox::Model::PRELOAD_PER_POLICY);
                    m_uids.append(message->uid());
                    m_uidsWaiting.append(message->uid());
                }
            }
        }
        qDebug() << ">>> MailboxWatcher: UNREAD COUNT INCREASED TO: " << m_previousUnreadCount;
    } else if (unreadCount < m_previousUnreadCount) {
        m_previousUnreadCount = unreadCount;
        qDebug() << ">>> MailboxWatcher: UNREAD COUNT DECREASED: " << m_previousUnreadCount;
        Imap::Mailbox::TreeItemMsgList *msgListPtr = m_msgListModel->msgListPtr;
        for (int i = 0; i < msgListPtr->m_children.size(); ++i) {
            Imap::Mailbox::TreeItemMessage *message = static_cast<Imap::Mailbox::TreeItemMessage *>(msgListPtr->m_children[i]);
            if (message->isMarkedAsRead()) {
                Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(m_account->imapModel());
                if (m_uids.contains(message->uid())) {
                    Imap::Message::Envelope envelope = message->envelope(model);
                    PostalMessage *mail = new PostalMessage;
                    Imap::Message::MailAddress addr = envelope.from.takeAt(0);
                    mail->from = addr.prettyName(Imap::Message::MailAddress::FORMAT_JUST_NAME);
                    mail->subject = envelope.subject;
                    mail->accountId = m_account->accountId();
                    mail->count = m_msgListModel->unreadCount();
                    mail->uid = message->uid();
                    m_uids.removeAll(message->uid());
                    emit messageNowMarkedRead(mail);
                }
            }
        }
    }
    emit countChanged();
}

void MailboxWatcher::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    // we only want to look for messages here that we have previously had to call Imap::Mailbox::Model::askForMessageMetadata
    // becuase the envelope wasn't available yet.
    // These currently unknown messages are the ones we use for notifications
    // For already fetched messages we have to assume we have already seen as to not keep resending the same notification
    // because the user hasn't read it yet.
    // So basically notifications are created for envelopes hot off the wire.
    // The unread count bubble on the pinned launcher icon will represent the full total unread i.e MsgListModel::unreadCount()
    // and not the count of the messages we are waiting for here i.e m_uids.size()
    if (m_uidsWaiting.size() == 0) {
        return;
    }
    Imap::Mailbox::TreeItemMsgList *msgListPtr = m_msgListModel->msgListPtr;
    for (int i = 0; i < msgListPtr->m_children.size(); ++i) {
        Imap::Mailbox::TreeItemMessage *message = static_cast<Imap::Mailbox::TreeItemMessage *>(msgListPtr->m_children[i]);
        if (!message->isMarkedAsRead() && m_uidsWaiting.contains(message->uid())) {
            if (!message->fetched()) {
                qDebug() << "handleDataChanged >>> MESSAGE" << message->uid() << "NOT ARRIVED YET";
                continue;
            }
            Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(m_account->imapModel());
            QDateTime timestamp = message->envelope(model).date.toLocalTime();
            if (timestamp < m_startDateTime) {
                qDebug() << "Not sending notification for msg: " << timestamp
                            << "StartTime: " << m_startDateTime;
                // if the timestamp is older than when this session
                // started then we don't track it, as we end up with
                // repeated unread notifications even though we can safely say
                // either we have already sent one on a previous connection
                // or the user has already browsed the message list and seen it exists
                // BUt we are waiting on it (maybe the cache got flushed or UIDVALIDITY changed
                m_uidsWaiting.removeAll(message->uid());
            } else if (message->fetched()) {
                qDebug() << "handleDataChanged >>> MESSAGE" << message->uid() << "ENVELOPE SUBJECT: " << message->envelope(model).subject;
                Imap::Message::Envelope envelope = message->envelope(model);
                PostalMessage *mail = new PostalMessage;
                Imap::Message::MailAddress addr = envelope.from.takeAt(0);
                mail->from = addr.prettyName(Imap::Message::MailAddress::FORMAT_JUST_NAME);
                mail->subject = envelope.subject;
                mail->accountId = m_account->accountId();
                mail->count = m_msgListModel->unreadCount();
                mail->uid = message->uid();
                // Here we want to see if we have an avatar in our cache for this address
                // Now the way our image helper works this image we may have will be too small
                // for the notification tray, but we know it exists on gravatar so let's use a gravatar path
                // as the icon location.
                // FIXME: fetch the icon into our cache so we don't have to keep using the users bandwidth
                QString address = addr.mailbox + QLatin1Char('@') + addr.host;
                QDir cacheDir(QString(Dekko::cacheLocation() % "/.ImageCache"));
                if (cacheDir.entryList(QStringList() << QString(address % "*.png")).size() > 0) {
                    QString hash = QString(QCryptographicHash::hash(address.toLatin1(),QCryptographicHash::Md5).toHex());
                    mail->gravatar = ("http://www.gravatar.com/avatar/" % hash);
                }
                m_uidsWaiting.removeAll(message->uid());
                emit newMessages(mail);
            }
        }
    }
}
}
}
