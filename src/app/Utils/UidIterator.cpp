/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

   This file is part of Dekko mail client

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "UidIterator.h"

#include <QAbstractItemModel>
#include <QDebug>
#include "Imap/Model/ItemRoles.h"

namespace Dekko {

namespace Utils {

UidIterator::UidIterator(QObject *parent)
    : QObject(parent), m_messageListModel(0), m_uid(-1), m_nextUid(-1), m_prevUid(-1), m_currentPosition(0)
{

}

int UidIterator::uid()
{
    return m_uid;
}

void UidIterator::setUid(int uid)
{
    m_uid = uid;
    if (m_messageListModel) {
        findNextPrev();
    }
}

QAbstractItemModel *UidIterator::messageListModel()
{
    return m_messageListModel;
}

void UidIterator::setMessageListModel(QAbstractItemModel *model)
{
    //TODO connect m_messageListModel rowsRemoved, rowsAdded and layoutChanged to updating m_next, m_prev
    m_messageListModel = model;
    if (m_uid > 0) {
        findNextPrev();
    }
}

int UidIterator::goNext()
{
    m_uid = m_nextUid;
    findNextPrev();
    return m_uid;
}

int UidIterator::goPrev()
{
    m_uid = m_prevUid;
    findNextPrev();
    return m_uid;
}

void UidIterator::findNextPrev()
{
    // m_currentPosition record the last position where we find the current uid
    // however model can change before last time findNextPrev is called.
    // So if the uid of m_currentPosition is greater than m_uid, search forward. Otherwise search backward.
    // This assume the model is descending in uid.
    if (m_messageListModel->rowCount() == 0) {
        m_nextUid = -1;
        m_prevUid = -1;
        m_uid = -1;
        emit uidsChanged();
        return;
    }
    if (m_currentPosition >= m_messageListModel->rowCount()) {
        m_currentPosition = 0;
    }
    if (m_messageListModel->index(m_currentPosition, 0).data(Imap::Mailbox::RoleMessageUid).toInt() < m_uid) {
        for (int i = m_currentPosition; i > -1; i --) {
            if (m_messageListModel->index(i, 0).data(Imap::Mailbox::RoleMessageUid).toInt() >= m_uid) {
                m_currentPosition = i;
                break;
            }
        }
    } else {
        for (int i = m_currentPosition; i < m_messageListModel->rowCount(); i++) {
            if (m_messageListModel->index(i, 0).data(Imap::Mailbox::RoleMessageUid).toInt() <= m_uid) {
                m_currentPosition = i;
                break;
            }
        }
    }
    m_uid = m_messageListModel->index(m_currentPosition, 0).data(Imap::Mailbox::RoleMessageUid).toInt();
    if (m_currentPosition < m_messageListModel->rowCount() - 1) {
        m_nextUid = m_messageListModel->index(m_currentPosition + 1, 0).data(Imap::Mailbox::RoleMessageUid).toInt();
    } else {
        m_nextUid = -1;
    }
    if (m_currentPosition > 0) {
        m_prevUid = m_messageListModel->index(m_currentPosition - 1, 0).data(Imap::Mailbox::RoleMessageUid).toInt();
    } else {
        m_prevUid = -1;
    }
    emit uidsChanged();
}

}
}
