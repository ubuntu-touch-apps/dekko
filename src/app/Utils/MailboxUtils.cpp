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
#include "MailboxUtils.h"
#include <QDebug>
#include "Imap/Model/Model.h"
#include "Imap/Model/MailboxTree.h"
#include "Imap/Model/MsgListModel.h"
#include "Imap/Model/SubtreeModel.h"
#include "Imap/Model/CopyMoveOperation.h"

namespace Dekko
{
namespace Utils
{
MailboxUtils::MailboxUtils(QObject *parent) :
    QObject(parent), m_account(0), m_mboxSettings(0)
{
    m_mboxSettings = new Settings::MailboxSettings(this);
}

QObject *MailboxUtils::account()
{
    return m_account;
}

void MailboxUtils::setAccount(QObject *account)
{
    m_account = qobject_cast<Accounts::Account *>(account);
    if (m_account->accountId().isEmpty()) {
        return;
    }
    m_mboxSettings->setAccount(m_account);
    emit accountChanged();
}

bool MailboxUtils::markMessageAsRead(const QModelIndex &message, bool marked)
{
    Imap::Mailbox::Model *m_imapModel = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
    if (!m_imapModel)
        return false;
    m_imapModel->markMessagesRead(QModelIndexList() << message, marked ? Imap::Mailbox::FLAG_ADD : Imap::Mailbox::FLAG_REMOVE);
    emit flagChangeEventDispatched();
    return true;
}

bool MailboxUtils::markSelectedMessagesAsRead(bool marked)
{
    if (m_selectedIndexList.count()) {
        Imap::Mailbox::Model *m_imapModel = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
        if (m_imapModel) {
            m_imapModel->markMessagesRead(m_selectedIndexList, marked ? Imap::Mailbox::FLAG_ADD : Imap::Mailbox::FLAG_REMOVE);
            emit flagChangeEventDispatched();
            return true;
        }
    } else {
        qDebug() << "[MailboxUtils::markSelectedMessagesAsRead] No messages to mark";
    }
    return false;
}

bool MailboxUtils::markMessageDeleted(const QModelIndex &message, bool marked)
{
    return determineMoveSelectedMessagesToTrashOrMarkDeleted(QModelIndexList() << message, marked);;
}

bool MailboxUtils::markSelectedMessagesDeleted(bool mark)
{
    if (m_selectedIndexList.count()) {
        return determineMoveSelectedMessagesToTrashOrMarkDeleted(m_selectedIndexList, mark);
    } else {
        qDebug() << "[MailboxUtils::markSelectedMessagesDeleted] No messages to marke deleted";
    }
    return false;
}

bool MailboxUtils::markMessageAsFlagged(const QModelIndex &message, bool marked)
{
    Imap::Mailbox::Model *m_imapModel = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
    if (!m_imapModel)
        return false;
    m_imapModel->setMessageFlags(QModelIndexList() << message, "\\Flagged", marked ? Imap::Mailbox::FLAG_ADD : Imap::Mailbox::FLAG_REMOVE);
    emit flagChangeEventDispatched();
    return true;
}

bool MailboxUtils::markSelectedMessagesFlagged(bool mark)
{
    if (m_selectedIndexList.count()) {
        Imap::Mailbox::Model *m_imapModel = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
        if (m_imapModel) {
            m_imapModel->setMessageFlags(m_selectedIndexList, "\\Flagged", mark ? Imap::Mailbox::FLAG_ADD : Imap::Mailbox::FLAG_REMOVE);
            emit flagChangeEventDispatched();
            return true;
        }
    } else {
        qDebug() << "[MailboxUtils::markSelectedMessagesFlagged] No messages to mark flagged";
    }
    return false;
}

QModelIndex MailboxUtils::deproxifiedIndex(const QModelIndex index)
{
    return Imap::deproxifiedIndex(index);
}

bool MailboxUtils::moveMessageToMailbox(const QString &currentMailbox, const uint &uid, const QString &targetMailbox)
{
    if (!m_account) {
        return false;
    }
    Imap::Mailbox::Model *m_model = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
    Q_ASSERT(m_model);
    if (!m_model->isNetworkAvailable()) {
        return false; // We need to have a network connection for this to work
    }
    Imap::Mailbox::TreeItemMailbox *currentMbox = static_cast<Imap::Mailbox::TreeItemMailbox *>(m_model->findMailboxByName(currentMailbox));
    if (!currentMbox) {
        return false;
    }
    // Ok let's move it now
    QList<uint> uids;
    uids << uid;
    m_model->copyMoveMessages(currentMbox, targetMailbox, uids, Imap::Mailbox::MOVE);
    return true;
}

bool MailboxUtils::expungeMailboxByName(const QString &mboxName)
{
    if (!m_account) {
        return false;
    }
    Imap::Mailbox::Model *m_model = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
    Q_ASSERT(m_model);
    if (!m_model->isNetworkAvailable()) {
    }
    QModelIndex currentMbox = static_cast<Imap::Mailbox::TreeItemMailbox *>(m_model->findMailboxByName(mboxName))->toIndex(m_model);
    if (!currentMbox.isValid()) {
        return false;
    }
    m_model->expungeMailbox(currentMbox);
    return true;
}

bool MailboxUtils::markMailboxAsRead(const QString &mboxName)
{
    if (!m_account) {
        return false;
    }
    Imap::Mailbox::Model *m_model = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
    Q_ASSERT(m_model);
    if (!m_model->isNetworkAvailable()) {
    }
    QModelIndex currentMbox = static_cast<Imap::Mailbox::TreeItemMailbox *>(m_model->findMailboxByName(mboxName))->toIndex(m_model);
    if (!currentMbox.isValid()) {
        return false;
    }
    m_model->markMailboxAsRead(currentMbox);
    emit flagChangeEventDispatched();
    return true;
}

void MailboxUtils::appendToSelectedIndexList(const QModelIndex &index)
{
    if (index.isValid()) {
        m_selectedIndexList.append(index);
        return;
    }
    qDebug() << "[MailboxUtils::appendToSelectedIndexList] Model index not valid";
}

void MailboxUtils::appendToSelectedModelIndexList(const QList<QModelIndex> &indexList)
{
    if (!indexList.isEmpty()) {
        m_selectedIndexList.append(indexList);
        return;
    }
    qDebug() << "[MailboxUtils::appendToSelectedIndexList] Model index list not valid";
}

void MailboxUtils::removeFromSelectedIndexList(const QModelIndex &index)
{
    if (index.isValid()) {
        if (m_selectedIndexList.contains(index)) {
            m_selectedIndexList.removeAll(index);
            return;
        }
        qDebug() << "[MailboxUtils::removeFromSelectedIndexList] Model index not in selected list";
    }
    qDebug() << "[MailboxUtils::removeFromSelectedIndexList] Model index not valid";
}

void MailboxUtils::clearSelectedMessages()
{
    if (m_selectedIndexList.count()) {
        m_selectedIndexList.clear();
        return;
    }
    qDebug() << "[MailboxUtils::clearSelectedMessages] called on an empty list";
}

int MailboxUtils::getTotalForMessageFlag(const QList<QModelIndex> &indexList, MailboxUtils::Flags flag)
{
    int ret = 0;
    Q_FOREACH(const QModelIndex &idx, indexList) {
        switch (flag) {
        case MARKED_FLAGGED:
            if (idx.data(Imap::Mailbox::RoleMessageIsMarkedFlagged).toBool()) {
                ret++;
            }
            break;
        case NOT_FLAGGED:
            if (!idx.data(Imap::Mailbox::RoleMessageIsMarkedFlagged).toBool()) {
                ret++;
            }
            break;
        case MARKED_READ:
            if (idx.data(Imap::Mailbox::RoleMessageIsMarkedRead).toBool()) {
                ret++;
            }
            break;
        case MARKED_UNREAD:
            if (!idx.data(Imap::Mailbox::RoleMessageIsMarkedRead).toBool()) {
                ret++;
            }
            break;
        }
    }
    return ret;
}

bool MailboxUtils::determineMoveSelectedMessagesToTrashOrMarkDeleted(const QModelIndexList &msgs, const bool mark)
{
    if (!m_account) {
        return false;
    }
    Imap::Mailbox::Model *m_model = qobject_cast<Imap::Mailbox::Model *>(m_account->imapModel());
    Q_ASSERT(m_model);
    if (!m_model->isNetworkAvailable()) {
        return false; // We need to have a network connection for this to work
    }
    const QString currentMailbox = static_cast<Imap::Mailbox::MsgListModel *>(m_account->msgListModel())->mailboxName();
    m_mboxSettings->setMailboxName(currentMailbox);
    QVariantMap keys = m_mboxSettings->keys();
    bool moveToTrash = m_mboxSettings->read(keys.value("moveToTrash").toString()).toBool();
    bool markRead = m_mboxSettings->read(keys.value("markRead").toString()).toBool();
    QString destMbox = m_mboxSettings->read(keys.value("trashMailbox").toString()).toString();
    if (!moveToTrash) {
        qDebug() << "Move to trash not enabled, marking message(s) deleted";
        m_model->markMessagesDeleted(msgs, mark ? Imap::Mailbox::FLAG_ADD : Imap::Mailbox::FLAG_REMOVE);
        if (markRead && mark) {
            m_model->markMessagesRead(msgs, Imap::Mailbox::FLAG_ADD);
        }
        emit moveOrDeleteEventDispatched();
        return true;
    } else if (destMbox == currentMailbox) { // We could just add this to the if() above but this reads cleaner
        qDebug() << "Cant move messages in " << currentMailbox << "to" << destMbox << ", they are the same mailbox!";
        // Now if we are here then that means the user has tried deleting the message in the Trash folder.
        // So the notice suggests we can't move it here again. But this time we set the Deleted flag assuming
        // the user wants to fully delete on the next expunge run.
        m_model->markMessagesDeleted(msgs, mark ? Imap::Mailbox::FLAG_ADD : Imap::Mailbox::FLAG_REMOVE);
        if (markRead && mark) {
            m_model->markMessagesRead(msgs, Imap::Mailbox::FLAG_ADD);
        }
        emit moveOrDeleteEventDispatched();
        return true;
    }
    // Ok so the user wants it moved so let's get to it.
    Imap::Mailbox::TreeItemMailbox *currentMbox = static_cast<Imap::Mailbox::TreeItemMailbox *>(m_model->findMailboxByName(currentMailbox));
    if (!currentMbox) {
        return false;
    }
    QList<uint> uids;
    Q_FOREACH(const QModelIndex &idx, msgs) {
        if (idx.isValid()) {
            uids.append(idx.data(Imap::Mailbox::RoleMessageUid).toUInt());
        }
    }
    m_model->copyMoveMessages(currentMbox, destMbox, uids, Imap::Mailbox::MOVE);
    emit moveOrDeleteEventDispatched();
    return true;
}
}
}
