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
#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include <QAbstractProxyModel>
#include <QAbstractItemModel>
#include "3rdParty/trojita/Imap/Model/Model.h"
#include "3rdParty/trojita/Imap/Model/ItemRoles.h"
#include "3rdParty/trojita/Imap/Model/MailboxTree.h"

namespace Imap {
namespace Mailbox {
class Model;
class TreeItemMailbox;
class TreeItemMsgList;
class TreeItem;
}
}

namespace Dekko
{
namespace Models
{
class MessageListModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ imapModel WRITE setImapModel NOTIFY modelChanged)
    Q_PROPERTY(QString mailbox READ mailbox WRITE setMailbox NOTIFY mailboxChanged)
    Q_PROPERTY(bool messagesAvailable READ messagesAvailable NOTIFY messagesAvailableChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(bool itemsValid READ itemsValid NOTIFY indexStateChanged)
    Q_PROPERTY(bool isWaitingForMessages READ isWaitingForMessages NOTIFY messagesAvailableChanged)

public:
    explicit MessageListModel(QObject *parent = 0);

    QHash<int, QByteArray> roleNames() const override;

    QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    bool hasChildren(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &proxyIndex, int role=Qt::DisplayRole) const;

    QString mailbox() const;
    void setMailbox(const QString &mboxName);
    bool messagesAvailable() const;
    Qt::SortOrder sortOrder() const;
    void setSortOrder(const Qt::SortOrder &order);
    bool isWaitingForMessages() const;
    QObject *imapModel() const;
    void setImapModel(QObject *model);
    bool itemsValid() const;

    Q_INVOKABLE QModelIndex currentMailbox() const;

signals:
    void modelChanged();
    void mailboxChanged(const QModelIndex &mailbox);
    void messagesAvailableChanged();
    void sortOrderChanged();
    void indexStateChanged();
    void messageRemoved();

public slots:
    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void handleRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void handleRowsRemoved(const QModelIndex &parent, int start, int end);
    void handleRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void handleRowsInserted(const QModelIndex &parent, int start, int end);
    void resetModel();

private:
    QString m_mailbox;
    Qt::SortOrder m_sortOrder;
    bool m_waitingForMessages;
    mutable Imap::Mailbox::TreeItemMsgList *originalMsgList;
    QPersistentModelIndex proxyMsgList;
    void validateProxyListIndex() const;
    void setupSourceModelConnections(Imap::Mailbox::Model *imapModel);

    MessageListModel &operator=(const MessageListModel&);
    MessageListModel(const MessageListModel &);
};
}
}

#endif // MESSAGELISTMODEL_H
