/* Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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

#include "MessageModel.h"
#include "app/Utils/FindTextPart.h"

namespace Dekko {
namespace Models {

MessageModel::MessageModel(QObject *parent)
    : Imap::Mailbox::OneMessageModel(parent)
{

}

void MessageModel::openMessage()
{
    setMessage(m_mailbox, m_uid);
}

void MessageModel::markMessageDeleted(QModelIndex messageIndex, bool marked)
{
    m_model->markMessagesDeleted(QModelIndexList() << messageIndex, marked ? Imap::Mailbox::FLAG_ADD : Imap::Mailbox::FLAG_REMOVE);
}

void MessageModel::setMessageModelIndex(QModelIndex index)
{
    setMessage(index);
}

QModelIndex MessageModel::getModelIndexFromPartId(QString partId)
{
    QModelIndex item = messageFirstChildIndex();
    QStringList separated = partId.split('.');
    for (QStringList::const_iterator it = separated.constBegin(); it != separated.constEnd(); ++it) {
        bool ok;
        uint number = it->toUInt(&ok);
        Q_ASSERT(ok);
        // Normal path: descending down and finding the correct part
        QModelIndex child = item.child(0, 0);
        if (child.isValid() && child.data(Imap::Mailbox::RolePartIsTopLevelMultipart).toBool())
            item = child;
        item = item.child(number - 1, 0);
        Q_ASSERT(item.isValid());
    }
    return item;

}

QObject *MessageModel::imapModel()
{
    return m_model;
}

void MessageModel::setImapModel(QObject *imapModel)
{
    if (!imapModel) {
        return;
    }
    this->m_imapModel = qobject_cast<Imap::Mailbox::Model *>(imapModel);
    init(m_imapModel);
}

int MessageModel::uid()
{
    return m_uid;
}

void MessageModel::setUid(int uid)
{
    this->m_uid = uid;
}

QString MessageModel::mailbox()
{
    return m_mailbox;
}

void MessageModel::setMailbox(QString mailbox)
{
    this->m_mailbox = mailbox;
}

QModelIndex MessageModel::mainTextPartIndex()
{
    QModelIndex idx;
    QString partMessage;
    Dekko::Utils::FindTextPart::findTextPartOfMessage(messageIndex(), idx, partMessage, 0);
    return idx;
}

}
}
