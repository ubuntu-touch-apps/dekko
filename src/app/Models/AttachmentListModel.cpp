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

#include "AttachmentListModel.h"
#include <QFileInfo>
#include <QMimeDatabase>
#include "UiUtils/Formatting.h"
#include "3rdParty/trojita/Imap/Model/ItemRoles.h"
#include "3rdParty/trojita/Imap/Model/MailboxTree.h"
#include "3rdParty/trojita/Composer/ContentDisposition.h"
#include "3rdParty/trojita/Composer/ComposerAttachments.h"
namespace Dekko {

namespace Models {

AttachmentListModel::AttachmentListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> AttachmentListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[AttachmentCaptionRole] = "caption";
    roles[HumanReadableSizeRole] = "humanReadableSize";
    roles[MimeTypeIconName] = "mimeTypeIconName";
    return roles;
}

QVariant AttachmentListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_attachmentList.size()) {
        return QVariant();
    }
    switch (role) {
    case AttachmentCaptionRole:
        return m_attachmentList[index.row()]->caption();
    case HumanReadableSizeRole:
        return UiUtils::Formatting::prettySize(m_attachmentList[index.row()]->sizeInBytes(), UiUtils::Formatting::BytesSuffix::WITH_BYTES_SUFFIX);
    case MimeTypeIconName:
    {
        QString mimetype = QString::fromUtf8(m_attachmentList[index.row()]->mimeType());
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForName(mimetype);
        return mime.genericIconName();
    }
    }
    return QVariant();
}

int AttachmentListModel::rowCount(const QModelIndex &parent) const
{
    return m_attachmentList.size();
}

void AttachmentListModel::addFileAttachment(const QString &path)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    Composer::AttachmentItem* attachment = new Composer::FileAttachmentItem(path);
    m_attachmentList.append(attachment);
    endInsertRows();
    emit countChanged();
}

void AttachmentListModel::removeAttachmentAtIndex(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    delete m_attachmentList[idx];
    m_attachmentList.removeAt(idx);
    endRemoveRows();
    emit countChanged();
}

void AttachmentListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    for (int i = 0; i < m_attachmentList.count(); i++) {
        delete m_attachmentList[i];
    }
    m_attachmentList.clear();
    endRemoveRows();
    emit countChanged();
}

void AttachmentListModel::addImapMessageAttachment(QModelIndex messageIndex, QAbstractItemModel *model)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    Composer::AttachmentItem* attachment =
            new Composer::ImapMessageAttachmentItem(static_cast<Imap::Mailbox::Model *>(model),
                    messageIndex.data(Imap::Mailbox::RoleMailboxName).toString(),
                    messageIndex.data(Imap::Mailbox::RoleMailboxUidValidity).toInt(),
                    messageIndex.data(Imap::Mailbox::RoleMessageUid).toInt());
    attachment->preload();
    attachment->setContentDispositionMode(Composer::CDN_INLINE);
    m_attachmentList.append(attachment);
    endInsertRows();
    emit countChanged();
}

void AttachmentListModel::addImapPartAttachment(QModelIndex messageIndex, QAbstractItemModel *model)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    Composer::AttachmentItem* attachment =
            new Composer::ImapPartAttachmentItem(static_cast<Imap::Mailbox::Model *>(model),
                    messageIndex.data(Imap::Mailbox::RoleMailboxName).toString(),
                    messageIndex.data(Imap::Mailbox::RoleMailboxUidValidity).toInt(),
                    messageIndex.data(Imap::Mailbox::RoleMessageUid).toInt(),
                    messageIndex.data(Imap::Mailbox::RolePartPathToPart).toString());
    attachment->preload();
    attachment->setContentDispositionMode(Composer::CDN_INLINE);
    m_attachmentList.append(attachment);
    endInsertRows();
    emit countChanged();
}

Composer::AttachmentItem *AttachmentListModel::getAttachmentAtIndex(int index)
{
    return m_attachmentList[index];
}

}
}
