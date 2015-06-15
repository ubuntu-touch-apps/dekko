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
#include "MessageListModel.h"
#include "3rdParty/trojita/Imap/Model/ItemRoles.h"
#include "3rdParty/trojita/Imap/Model/MailboxModel.h"
#include "3rdParty/trojita/Imap/Model/MailboxTree.h"
#include "3rdParty/trojita/Imap/Model/QAIM_reset.h"

namespace Dekko
{
namespace Models
{
MessageListModel::MessageListModel(QObject *parent) :
    QAbstractProxyModel(parent), m_waitingForMessages(false)
{
}

QHash<int, QByteArray> MessageListModel::roleNames() const
{
    using namespace Imap::Mailbox;
    QHash<int, QByteArray> roles;
    roles[Imap::Mailbox::RoleIsFetched] = "isFetched";
    roles[Imap::Mailbox::RoleIsUnavailable] = "isUnavailable";
    roles[Imap::Mailbox::RoleMessageUid] = "messageUid";
    roles[Imap::Mailbox::RoleMessageIsMarkedDeleted] = "isMarkedDeleted";
    roles[Imap::Mailbox::RoleMessageIsMarkedRead] = "isMarkedRead";
    roles[Imap::Mailbox::RoleMessageIsMarkedForwarded] = "isMarkedForwarded";
    roles[Imap::Mailbox::RoleMessageIsMarkedReplied] = "isMarkedReplied";
    roles[Imap::Mailbox::RoleMessageIsMarkedRecent] = "isMarkedRecent";
    roles[Imap::Mailbox::RoleMessageIsMarkedFlagged] = "isMarkedFlagged";
    roles[Imap::Mailbox::RoleMessageDate] = "date";
    roles[Imap::Mailbox::RoleMessageInternalDate] = "receivedDate";
    roles[Imap::Mailbox::RoleMessageFrom] = "from";
    roles[Imap::Mailbox::RoleMessageTo] = "to";
    roles[Imap::Mailbox::RoleMessageCc] = "cc";
    roles[Imap::Mailbox::RoleMessageBcc] = "bcc";
    roles[Imap::Mailbox::RoleMessageSender] = "sender";
    roles[Imap::Mailbox::RoleMessageReplyTo] = "replyTo";
    roles[Imap::Mailbox::RoleMessageInReplyTo] = "inReplyTo";
    roles[Imap::Mailbox::RoleMessageMessageId] = "messageId";
    roles[Imap::Mailbox::RoleMessageSubject] = "subject";
    roles[Imap::Mailbox::RoleMessageFlags] = "flags";
    roles[Imap::Mailbox::RoleMessageSize] = "size";
    roles[Imap::Mailbox::RoleMessageFuzzyDate] = "fuzzyDate";
    roles[Imap::Mailbox::RoleMessageHasAttachments] = "hasAttachments";
    return roles;
}

QModelIndex MessageListModel::index(int row, int column, const QModelIndex &parent) const
{
    validateProxyListIndex();
    Q_UNUSED(parent);
    if (!originalMsgList) {
        return QModelIndex();
    }
    if (row >= originalMsgList->m_children.size() || row < 0) {
        return QModelIndex();
    }
    // This is where we determine the sort order
    int newRow;
    if (m_sortOrder == Qt::DescendingOrder) {
        newRow = originalMsgList->childrenCount(dynamic_cast<Imap::Mailbox::Model *>(sourceModel())) - 1 - row;
    } else {
        newRow = row;
    }
    return createIndex(newRow, column, originalMsgList->m_children[row]);
}

QModelIndex MessageListModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int MessageListModel::rowCount(const QModelIndex &parent) const
{
    validateProxyListIndex();
    if (parent.isValid() || !originalMsgList) {
        return 0;
    }
    return originalMsgList->rowCount(dynamic_cast<Imap::Mailbox::Model *>(sourceModel()));
}

int MessageListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 0;
}

QModelIndex MessageListModel::mapToSource(const QModelIndex &proxyIndex) const
{
    validateProxyListIndex();
    if (!proxyIndex.isValid() || proxyIndex.parent().isValid() || !originalMsgList) {
        return QModelIndex();
    }

    Imap::Mailbox::Model *model = dynamic_cast<Imap::Mailbox::Model *>(sourceModel());
    return model->createIndex(proxyIndex.row(), 0, originalMsgList->m_children[proxyIndex.row()]);
}

QModelIndex MessageListModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    validateProxyListIndex();
    if (!originalMsgList || (sourceIndex.model() != sourceModel())) {
        return QModelIndex();
    }
    if (dynamic_cast<Imap::Mailbox::TreeItemMessage *>(Imap::Mailbox::Model::realTreeItem(sourceIndex))) {
        return index(sourceIndex.row(), 0, QModelIndex());
    } else {
        return QModelIndex();
    }
}

bool MessageListModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}

QVariant MessageListModel::data(const QModelIndex &proxyIndex, int role) const
{
    using namespace Imap::Mailbox;
    validateProxyListIndex();
    if (!originalMsgList || !proxyIndex.isValid() || proxyIndex.model() != this) {
        return QVariant();
    }
    switch (role) {
    case Imap::Mailbox::RoleIsFetched:
    case Imap::Mailbox::RoleIsUnavailable:
    case Imap::Mailbox::RoleMessageUid:
    case Imap::Mailbox::RoleMessageIsMarkedDeleted:
    case Imap::Mailbox::RoleMessageIsMarkedRead:
    case Imap::Mailbox::RoleMessageIsMarkedForwarded:
    case Imap::Mailbox::RoleMessageIsMarkedReplied:
    case Imap::Mailbox::RoleMessageIsMarkedRecent:
    case Imap::Mailbox::RoleMessageIsMarkedFlagged:
    case Imap::Mailbox::RoleMessageDate:
    case Imap::Mailbox::RoleMessageFrom:
    case Imap::Mailbox::RoleMessageTo:
    case Imap::Mailbox::RoleMessageCc:
    case Imap::Mailbox::RoleMessageBcc:
    case Imap::Mailbox::RoleMessageSender:
    case Imap::Mailbox::RoleMessageReplyTo:
    case Imap::Mailbox::RoleMessageInReplyTo:
    case Imap::Mailbox::RoleMessageMessageId:
    case Imap::Mailbox::RoleMessageSubject:
    case Imap::Mailbox::RoleMessageFlags:
    case Imap::Mailbox::RoleMessageSize:
    case Imap::Mailbox::RoleMessageHeaderReferences:
    case Imap::Mailbox::RoleMessageHeaderListPost:
    case Imap::Mailbox::RoleMessageHeaderListPostNo:
    case Imap::Mailbox::RoleMessageHasAttachments:
        return dynamic_cast<Imap::Mailbox::TreeItemMessage *>(Imap::Mailbox::Model::realTreeItem(
                proxyIndex))->data(static_cast<Imap::Mailbox::Model *>(sourceModel()), role);
    default:
        int newRow;
        if (m_sortOrder == Qt::DescendingOrder) {
            newRow = originalMsgList->childrenCount(dynamic_cast<Imap::Mailbox::Model *>(sourceModel())) - 1 - proxyIndex.row();
        } else {
            newRow = proxyIndex.row();
        }
        return QAbstractProxyModel::data(createIndex(newRow, 0, proxyIndex.internalPointer()), role);
    }
}

QString MessageListModel::mailbox() const
{
    return m_mailbox;
}

void MessageListModel::setMailbox(const QString &mboxName)
{
    m_waitingForMessages = true;
    emit messagesAvailableChanged();
    Imap::Mailbox::Model *model = dynamic_cast<Imap::Mailbox::Model *>(sourceModel());
    Q_ASSERT(model);
    Imap::Mailbox::TreeItemMailbox *mbox = model->findMailboxByName(mboxName);
    Imap::Mailbox::TreeItemMsgList *msgList = dynamic_cast<Imap::Mailbox::TreeItemMsgList *>(
                mbox->child(0, const_cast<Imap::Mailbox::Model *>(model)));
    Q_ASSERT(msgList);
    validateProxyListIndex();
    if (msgList != originalMsgList) {
        originalMsgList = msgList;
        proxyMsgList = originalMsgList->toIndex(const_cast<Imap::Mailbox::Model *>(model));
        originalMsgList->resetWasUnreadState();
        RESET_MODEL;
        emit mailboxChanged(mbox->toIndex(model));
    }
    // start IDLE
    const_cast<Imap::Mailbox::Model *>(model)->switchToMailbox(mbox->toIndex(model));
}

bool MessageListModel::messagesAvailable() const
{
    return !m_waitingForMessages;
}

Qt::SortOrder MessageListModel::sortOrder() const
{
    return m_sortOrder;
}

void MessageListModel::setSortOrder(const Qt::SortOrder &order)
{
    m_sortOrder = order;
    emit sortOrderChanged();
}

bool MessageListModel::isWaitingForMessages() const
{
    return m_waitingForMessages;
}

QObject *MessageListModel::imapModel() const
{
    return static_cast<Imap::Mailbox::Model *>(sourceModel());
}

void MessageListModel::setImapModel(QObject *model)
{
    setSourceModel(static_cast<Imap::Mailbox::Model *>(model));
    setupSourceModelConnections(static_cast<Imap::Mailbox::Model *>(sourceModel()));
    emit modelChanged();
}

bool MessageListModel::itemsValid() const
{
    return currentMailbox().isValid();
}

QModelIndex MessageListModel::currentMailbox() const
{
    validateProxyListIndex();
    return proxyMsgList.parent();
}

void MessageListModel::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QModelIndex top = mapFromSource(topLeft);
    QModelIndex bottom = mapFromSource(bottomRight);

    if (!top.isValid() || !bottom.isValid()) {
        return;
    }
    int newRow;
    if (m_sortOrder == Qt::DescendingOrder) {
        newRow = originalMsgList->childrenCount(dynamic_cast<Imap::Mailbox::Model *>(sourceModel())) - 1 - bottom.row();
    } else {
        newRow = bottom.row();
    }
    bottom = createIndex(newRow, 0, Imap::Mailbox::Model::realTreeItem(bottom));
    if (top.parent() == bottom.parent()) {
        emit dataChanged(top, bottom);
    } else {
        Q_ASSERT(false);
        return;
    }
}

void MessageListModel::handleRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    if (!parent.isValid()) {
        resetModel();
        return;
    }

    validateProxyListIndex();
    if (!originalMsgList)
        return;

    Imap::Mailbox::TreeItem *item = Imap::Mailbox::Model::realTreeItem(parent);
    Imap::Mailbox::TreeItemMailbox *mailbox = dynamic_cast<Imap::Mailbox::TreeItemMailbox *>(item);
    Imap::Mailbox::TreeItemMsgList *newList = dynamic_cast<Imap::Mailbox::TreeItemMsgList *>(item);

    if (parent.isValid()) {
        Q_ASSERT(parent.model() == sourceModel());
    } else {
        mailbox = static_cast<Imap::Mailbox::Model *>(sourceModel())->m_mailboxes;
        Q_ASSERT(mailbox);
    }

    if (newList) {
        if (newList == originalMsgList) {
            beginRemoveRows(mapFromSource(parent), start, end);
            for (int i = start; i <= end; ++i)
                emit messageRemoved();
        }
    } else if (mailbox) {
        Q_ASSERT(start > 0);
        // if we're below it, we're gonna die
        for (int i = start; i <= end; ++i) {
            const Imap::Mailbox::Model *model = 0;
            QModelIndex translatedParent;
            Imap::Mailbox::Model::realTreeItem(parent, &model, &translatedParent);
            Imap::Mailbox::TreeItemMailbox *m = dynamic_cast<Imap::Mailbox::TreeItemMailbox *>(static_cast<Imap::Mailbox::TreeItem *>(model->index(i, 0, translatedParent).internalPointer()));
            Q_ASSERT(m);
            Imap::Mailbox::TreeItem *up = originalMsgList->parent();
            while (up) {
                if (m == up) {
                    resetModel();
                    return;
                }
                up = up->parent();
            }
        }
    }
}

void MessageListModel::handleRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);

    validateProxyListIndex();
    if (!originalMsgList)
        return;

    if (!parent.isValid()) {
        return;
    }

    if (dynamic_cast<Imap::Mailbox::TreeItemMsgList *>(Imap::Mailbox::Model::realTreeItem(parent)) == originalMsgList)
        endRemoveRows();
    emit layoutChanged();
}

void MessageListModel::handleRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    validateProxyListIndex();
    if (!parent.isValid()) {
        return;
    }
    Imap::Mailbox::TreeItemMsgList *newList = dynamic_cast<Imap::Mailbox::TreeItemMsgList *>(
                Imap::Mailbox::Model::realTreeItem(parent));
    if (originalMsgList && originalMsgList == newList) {
        beginInsertRows(mapFromSource(parent), start, end);
    }
}

void MessageListModel::handleRowsInserted(const QModelIndex &parent, int start, int end)
{
    validateProxyListIndex();
    if (!parent.isValid())
        return;

    Q_UNUSED(start);
    Q_UNUSED(end);
    Imap::Mailbox::TreeItemMsgList *newList = dynamic_cast<Imap::Mailbox::TreeItemMsgList *>(
                Imap::Mailbox::Model::realTreeItem(parent));
    if (originalMsgList && originalMsgList == newList) {
        endInsertRows();
    }

    if (m_waitingForMessages) {
        m_waitingForMessages = false;
        emit messagesAvailableChanged();
    }
}

void MessageListModel::resetModel()
{
    originalMsgList = 0;
    proxyMsgList = QModelIndex();
    RESET_MODEL;
}

void MessageListModel::validateProxyListIndex() const
{
    if (!proxyMsgList.isValid()) {
        if (originalMsgList) {
            emit const_cast<MessageListModel *>(this)->indexStateChanged();
        }
        originalMsgList = 0;
    }
}

void MessageListModel::setupSourceModelConnections(Imap::Mailbox::Model *imapModel)
{
    if (!sourceModel()) {
        return;
    }
    // lets first go with the minimum required SIGNAL-to-SLOTS
    connect(imapModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), this,
            SLOT(handleRowsAboutToBeInserted(QModelIndex,int,int)));
    connect(imapModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this,
            SLOT(handleRowsInserted(QModelIndex,int,int)));
    connect(imapModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this,
            SLOT(handleRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(imapModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this,
            SLOT(handleRowsRemoved(QModelIndex,int,int)));
    connect(imapModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this,
            SLOT(handleDataChanged(QModelIndex,QModelIndex)));
    connect(imapModel, SIGNAL(modelAboutToBeReset()),this ,
            SLOT(resetModel()));
    // and connect up sourcemodel signals to our signals
    connect(imapModel, SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
    connect(imapModel, SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
    connect(this, SIGNAL(layoutChanged()), this, SIGNAL(indexStateChanged()));
    connect(this, SIGNAL(modelReset()), this, SIGNAL(indexStateChanged()));
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SIGNAL(indexStateChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SIGNAL(indexStateChanged()));
}
}
}
