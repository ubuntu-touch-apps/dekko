/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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

#include "SearchForSpecialMailbox.h"
#include <QDebug>
#include "3rdParty/trojita/Imap/Model/ItemRoles.h"
#include "3rdParty/trojita/Imap/Model/MailboxTree.h"

namespace Dekko {
namespace Utils {

SearchForSpecialMailbox::SearchForSpecialMailbox(QObject *parent) :
    QObject(parent), m_model(0), m_isRunning(false)
{
}

void SearchForSpecialMailbox::findSpecialMailboxHelper(QModelIndex mailbox)
{

    if (mailbox.data(Imap::Mailbox::RoleMailboxIsINBOX).toBool()) {
        qDebug() << "INBOX FOUND";
        emit specialMailboxFound(mailbox.data(Imap::Mailbox::RoleMailboxName).toString(),
                                 MAILBOX_IS_INBOX);
        emit inboxFound(mailbox.data(Imap::Mailbox::RoleMailboxName).toString(), mailbox);
    }
    if (mailbox.data(Imap::Mailbox::RoleMailboxSpecialUseIsSENT).toBool()) {
        emit specialMailboxFound(mailbox.data(Imap::Mailbox::RoleMailboxName).toString(),
                                 MAILBOX_IS_SENT);
    }
    if (mailbox.data(Imap::Mailbox::RoleMailboxSpecialUseIsTRASH).toBool()) {
        emit specialMailboxFound(mailbox.data(Imap::Mailbox::RoleMailboxName).toString(),
                                 MAILBOX_IS_TRASH);
    }
    if (mailbox.data(Imap::Mailbox::RoleMailboxHasChildMailboxes).toBool()) {
        Imap::Mailbox::TreeItemMailbox *mailboxItem = static_cast<Imap::Mailbox::TreeItemMailbox *>(mailbox.internalPointer());
        if (mailbox.data(Imap::Mailbox::RoleIsFetched).toBool()) {
            for (unsigned int i = 0; i < mailboxItem->childrenCount(m_model); i++) {
                findSpecialMailboxHelper(mailboxItem->child(i, m_model)->toIndex(m_model));
            }
        } else {
            m_isRunning = true;
            emit isRunningChanged();
            mailboxItem->fetch(m_model);
            m_mailboxListWaitForLoad << mailboxItem->mailbox();
        }
    }
}

void SearchForSpecialMailbox::start()
{
    Q_ASSERT(m_model);

    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(handleRowsInserted(QModelIndex, int, int)));
    m_mailboxListWaitForLoad.clear();
    startFromTop();

}

QAbstractItemModel *SearchForSpecialMailbox::model()
{
    return m_model;
}

void SearchForSpecialMailbox::setModel(QAbstractItemModel *model)
{
    m_model = dynamic_cast<Imap::Mailbox::Model *>(model);
}

void SearchForSpecialMailbox::checkComplete()
{
    if (m_mailboxListWaitForLoad.isEmpty()) {
        m_isRunning = false;
        disconnect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(handleRowsInserted(QModelIndex, int, int)));
        emit isRunningChanged();
        emit complete();
    }
}

void SearchForSpecialMailbox::handleRowsInserted(QModelIndex parent, int first, int last)
{
    if (m_mailboxListWaitForLoad.contains(parent.data(Imap::Mailbox::RoleMailboxName).toString())) {
        QString mailboxName = parent.data(Imap::Mailbox::RoleMailboxName).toString();
        m_mailboxListWaitForLoad.removeOne(mailboxName);
        findSpecialMailboxHelper(parent);
    } else if (!parent.isValid()) {
        //Top level mailbox fetched
        m_mailboxListWaitForLoad.removeOne(TOP_MAILBOX_NAME_ALIAS);
        startFromTop();
    }
    checkComplete();
}

void SearchForSpecialMailbox::startFromTop() {
    Imap::Mailbox::TreeItemMailbox *mailboxItem = m_model->getToplevelMailboxItem();
    if (mailboxItem->fetched()) {
        for (int i = 0; i < m_model->rowCount(QModelIndex()); i++) {
            QModelIndex mailbox = m_model->index(i, 0, QModelIndex());
            findSpecialMailboxHelper(mailbox);
        }
    } else {
        m_isRunning = true;
        emit isRunningChanged();
        mailboxItem->fetch(m_model);
        if (mailboxItem->parent() == NULL)
            m_mailboxListWaitForLoad << TOP_MAILBOX_NAME_ALIAS;
        else
            m_mailboxListWaitForLoad << mailboxItem->mailbox();
        qDebug() << m_mailboxListWaitForLoad;
    }
    checkComplete();
}

}
}
