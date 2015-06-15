/* Copyright (C) 2006 - 2014 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

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

#include "OneMessageModel.h"
#include "kdeui-itemviews/kdescendantsproxymodel.h"
#include "FindInterestingPart.h"
#include "ItemRoles.h"
#include "Model.h"
#include "SubtreeModel.h"
#include "SpecialFlagNames.h"

namespace Imap
{
namespace Mailbox
{

OneMessageModel::OneMessageModel(QObject *parent)
    : QObject(parent), m_subtree(0)
{

}

OneMessageModel::OneMessageModel(QObject *parent, Model *model): QObject(parent), m_subtree(0)
{
    init(model);
}

void OneMessageModel::setMessage(const QString &mailbox, const uint uid)
{
    m_mainPartUrl = QUrl(QLatin1String("about:blank"));
    emit mainPartUrlChanged();
    QAbstractItemModel *abstractModel = qobject_cast<QAbstractItemModel*>(m_model);
    Q_ASSERT(abstractModel);
    Model *model = qobject_cast<Model*>(abstractModel);
    Q_ASSERT(model);
    setMessage(model->messageIndexByUid(mailbox, uid));
}

void OneMessageModel::setMessage(const QModelIndex &message)
{
    Q_ASSERT(!message.isValid() || message.model() == m_model);
    m_message = message;
    m_subtree->setRootItem(message);

    // Now try to locate the interesting part of the current message
    QModelIndex idx;
    QString partMessage;
    FindInterestingPart::MainPartReturnCode res = FindInterestingPart::findMainPartOfMessage(m_message, idx, partMessage, 0);
    switch (res) {
    case Imap::Mailbox::FindInterestingPart::MAINPART_FOUND:
    case Imap::Mailbox::FindInterestingPart::MAINPART_PART_LOADING:
        Q_ASSERT(idx.isValid());
        m_mainPartIndex = idx;
        m_mainPartUrl = QLatin1String("trojita-imap://msg") + idx.data(RolePartPathToPart).toString();
        m_mainPartMime = QString(idx.data(RolePartMimeType).toString().toLower());
        break;
    case Imap::Mailbox::FindInterestingPart::MAINPART_MESSAGE_NOT_LOADED:
        Q_ASSERT(false);
        m_mainPartUrl = QLatin1String("data:text/plain;charset=utf-8;base64,") + QString::fromLocal8Bit(QByteArray("").toBase64());
        break;
    case Imap::Mailbox::FindInterestingPart::MAINPART_PART_CANNOT_DETERMINE:
        // FIXME: show a decent message here
        m_mainPartUrl = QLatin1String("data:text/plain;charset=utf-8;base64,") + QString::fromLocal8Bit(QByteArray(partMessage.toUtf8()).toBase64());
        break;
    }
    emit mainPartUrlChanged();
    emit messageFirstChildIndexChanged();
}

QList<QByteArray> OneMessageModel::replyReferences() const
{
    if(!m_message.isValid()) {
        return QList<QByteArray>();
    }
    QList<QByteArray> res = references();
    res.append(messageId());
    return res;
}

void OneMessageModel::markMessageAsRead()
{
    if (!m_message.isValid())
        return;
    Imap::Mailbox::Model *model = const_cast<Imap::Mailbox::Model *>(dynamic_cast<const Imap::Mailbox::Model *>(m_message.model()));
    Q_ASSERT(model);
    if (!model->isNetworkAvailable())
        return;
    if (!m_message.data(Imap::Mailbox::RoleMessageIsMarkedRead).toBool())
        model->markMessagesRead(QModelIndexList() << m_message, Imap::Mailbox::FLAG_ADD);
}

void OneMessageModel::init(Model * model)
{
    m_model = model;
    m_subtree = new SubtreeModelOfModel(this);
    m_subtree->setSourceModel(model);
    connect(m_subtree, SIGNAL(modelReset()), this, SIGNAL(envelopeChanged()));
    connect(m_subtree, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(envelopeChanged()));
    connect(this, SIGNAL(envelopeChanged()), this, SIGNAL(flagsChanged()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(handleModelDataChanged(QModelIndex,QModelIndex)));
    m_flatteningModel = new KDescendantsProxyModel(this);
    m_flatteningModel->setSourceModel(m_subtree);
    QHash<int, QByteArray> roleNames;
    roleNames[RoleIsFetched] = "isFetched";
    roleNames[RolePartMimeType] = "mimeType";
    roleNames[RolePartCharset] = "charset";
    roleNames[RolePartContentFormat] = "contentFormat";
    roleNames[RolePartEncoding] = "encoding";
    roleNames[RolePartBodyFldId] = "bodyFldId";
    roleNames[RolePartBodyDisposition] = "bodyDisposition";
    roleNames[RolePartFileName] = "fileName";
    roleNames[RolePartOctets] = "size";
    roleNames[RolePartId] = "partId";
    roleNames[RolePartPathToPart] = "pathToPart";
    m_flatteningModel->proxySetRoleNames(roleNames);
}

QDateTime OneMessageModel::date() const
{
    return m_message.data(RoleMessageDate).toDateTime();
}

QDateTime OneMessageModel::receivedDate() const
{
    return m_message.data(RoleMessageInternalDate).toDateTime();
}

QString OneMessageModel::subject() const
{
    return m_message.data(RoleMessageSubject).toString();
}

QVariantList OneMessageModel::from() const
{
    return m_message.data(RoleMessageFrom).toList();
}

QVariantList OneMessageModel::to() const
{
    return m_message.data(RoleMessageTo).toList();
}

QVariantList OneMessageModel::cc() const
{
    return m_message.data(RoleMessageCc).toList();
}

QVariantList OneMessageModel::bcc() const
{
    return m_message.data(RoleMessageBcc).toList();
}

QVariantList OneMessageModel::sender() const
{
    return m_message.data(RoleMessageSender).toList();
}

QVariantList OneMessageModel::replyTo() const
{
    return m_message.data(RoleMessageReplyTo).toList();
}

QByteArray OneMessageModel::inReplyTo() const
{
    return m_message.data(RoleMessageInReplyTo).toByteArray();
}

QByteArray OneMessageModel::messageId() const
{
    return m_message.data(RoleMessageMessageId).toByteArray();
}

QList<QByteArray> OneMessageModel::references() const
{
    return m_message.data(Imap::Mailbox::RoleMessageHeaderReferences).value<QList<QByteArray> >();
}

bool OneMessageModel::isMarkedDeleted() const
{
    return m_message.data(RoleMessageIsMarkedDeleted).toBool();
}

bool OneMessageModel::isMarkedRead() const
{
    return m_message.data(RoleMessageIsMarkedRead).toBool();
}

bool OneMessageModel::isMarkedForwarded() const
{
    return m_message.data(RoleMessageIsMarkedForwarded).toBool();
}

bool OneMessageModel::isMarkedReplied() const
{
    return m_message.data(RoleMessageIsMarkedReplied).toBool();
}

bool OneMessageModel::isMarkedRecent() const
{
    return m_message.data(RoleMessageIsMarkedRecent).toBool();
}

bool OneMessageModel::isMarkedFlagged() const
{
    return m_message.data(RoleMessageIsMarkedFlagged).toBool();
}

void OneMessageModel::setMarkedFlagged(const bool marked)
{
    qDebug() << "Marking message as flagged to: " << marked;
    if (!m_message.isValid())
        return;

    qobject_cast<Model*>(parent())->setMessageFlags(
                QList<QModelIndex>() << m_message,
                Imap::Mailbox::FlagNames::flagged,
                marked ? FLAG_ADD : FLAG_REMOVE);
    emit flagsChanged();
}

QUrl OneMessageModel::mainPartUrl() const
{
    return m_mainPartUrl;
}

QString OneMessageModel::mainPartMimeType() const
{
    return m_mainPartMime;
}

QModelIndex OneMessageModel::mainPartIndex() const
{
    return m_mainPartIndex;
}

QObject *OneMessageModel::attachmentsModel() const
{
    return m_flatteningModel;
}

void OneMessageModel::setMarkedDeleted(const bool marked)
{
    if (!m_message.isValid())
        return;

    qobject_cast<Model*>(parent())->markMessagesDeleted(QList<QModelIndex>() << m_message, marked ? FLAG_ADD : FLAG_REMOVE);
}

void OneMessageModel::setMarkedRead(const bool marked)
{
    if (!m_message.isValid())
        return;

    qobject_cast<Model*>(parent())->markMessagesRead(QList<QModelIndex>() << m_message, marked ? FLAG_ADD : FLAG_REMOVE);
}

void OneMessageModel::handleModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_ASSERT(topLeft.row() == bottomRight.row());
    Q_ASSERT(topLeft.parent() == bottomRight.parent());
    Q_ASSERT(topLeft.model() == bottomRight.model());

    if (m_message.isValid() && topLeft == m_message)
        emit flagsChanged();
}

bool OneMessageModel::hasValidIndex() const
{
    return m_message.isValid();
}

QModelIndex OneMessageModel::messageFirstChildIndex() const
{
    return m_message.child(0, 0);
}

QModelIndex OneMessageModel::messageIndex() const
{
    return m_message;
}

bool OneMessageModel::isListPost() const
{
    QList<QUrl> listPost;
    Q_FOREACH(const QVariant &item, m_message.data(Imap::Mailbox::RoleMessageHeaderListPost).toList()) {
        listPost << item.toUrl();
    }
    return !listPost.isEmpty();
}

}
}
