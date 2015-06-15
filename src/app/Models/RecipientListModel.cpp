/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the Dekko

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

#include "RecipientListModel.h"
#include <QAbstractListModel>
#include <QDebug>
#include <algorithm>
#include <QSet>
#include <QStringList>
#include <QUrl>

#include "Imap/Model/ItemRoles.h"
#include "Imap/Model/Model.h"
#include "Imap/Model/MailboxTree.h"

namespace Dekko
{
namespace Models
{

RecipientListModel::RecipientListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

RecipientListModel::~RecipientListModel()
{
    this->clear();
}

QVariant RecipientListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_recipients.size()) {
        return QVariant();
    }
    switch (role) {
    case RecipientAddressRole:
        return m_recipients[index.row()]->recipientAddress();
    case RecipientKindRole:
        return m_recipients[index.row()]->recipientKind();
    case RecipientIndexRole:
        return index.row();
    }
    return QVariant();
}

int RecipientListModel::rowCount(const QModelIndex &) const
{
    return m_recipients.size();
}

int RecipientListModel::count() const
{
    return m_recipients.count();
}

bool RecipientListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= m_recipients.size()) {
        return false;
    }
    switch (role) {
    case RecipientKindRole:
        m_recipients[index.row()]->setRecipientKind((Recipient::RecipientKind)(value.toInt()));
        return true;
    case RecipientAddressRole:
        m_recipients[index.row()]->setRecipientAddress(value.toString());
        return true;
    }
    return false;

}

QModelIndex RecipientListModel::indexFromItem(Recipient *recipient) const
{
    if (recipient != NULL)
    {
        for (int i = 0; i < m_recipients.size(); i++) {
            if (m_recipients.at(i) == recipient) {
                return index(i);
            }
        }
    }
    return QModelIndex();
}

QHash<int, QByteArray> RecipientListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[RecipientAddressRole] = "recipientAddress";
    roles[RecipientKindRole] = "recipientKind";
    roles[RecipientIndexRole] = "recipientIndex";
    return roles;
}

void RecipientListModel::appendRow(Recipient *recipient)
{
    if (recipient != NULL) {
        appendRows(QList<Recipient *>() << recipient);
        emit countChanged();
    }
}

void RecipientListModel::appendRows(QList<Recipient *> &recipients)
{
    if (recipients.size() == 0) {
        return;
    }
    beginInsertRows(QModelIndex() , rowCount(), rowCount() + recipients.size() - 1);
    Q_FOREACH(Recipient *r, recipients) {
        r->setParent(this); // if not set parrent, recipient will be deleted in qml context.
        connect(r, SIGNAL(dataChanged()), this, SLOT(updateRecipient()));
        m_recipients.append(r);
    }
    endInsertRows();
    emit countChanged();
}

void RecipientListModel::insertRow(int row, Recipient *recipient)
{
    if (recipient == NULL) {
        return;
    }
    beginInsertRows(QModelIndex(), row, row);
    connect(recipient, SIGNAL(dataChanged()), this, SLOT(updateRecipient()));
    m_recipients.insert(row, recipient);
    endInsertRows();
    emit countChanged();
}

void RecipientListModel::updateRecipient()
{
    Recipient *recipient = static_cast<Recipient *>(sender());
    QModelIndex index = indexFromItem(recipient);
    if (index.isValid()) {
        emit dataChanged(index, index);
    }
}

Composer::RecipientList RecipientListModel::getRecipientsFromMessage(const QModelIndex &message)
{
    Composer::RecipientList originalRecipients;
    if (!message.isValid())
        return originalRecipients;

    using namespace Imap::Mailbox;
    using namespace Imap::Message;

    Envelope envelope = message.data(Imap::Mailbox::RoleMessageEnvelope).value<Imap::Message::Envelope>();

    // Prepare the list of recipients
    Q_FOREACH(const MailAddress &addr, envelope.from)
        originalRecipients << qMakePair(Composer::ADDRESS_FROM, addr);
    Q_FOREACH(const MailAddress &addr, envelope.to)
        originalRecipients << qMakePair(Composer::ADDRESS_TO, addr);
    Q_FOREACH(const MailAddress &addr, envelope.cc)
        originalRecipients << qMakePair(Composer::ADDRESS_CC, addr);
    Q_FOREACH(const MailAddress &addr, envelope.bcc)
        originalRecipients << qMakePair(Composer::ADDRESS_BCC, addr);
    Q_FOREACH(const MailAddress &addr, envelope.sender)
        originalRecipients << qMakePair(Composer::ADDRESS_SENDER, addr);
    Q_FOREACH(const MailAddress &addr, envelope.replyTo)
        originalRecipients << qMakePair(Composer::ADDRESS_REPLY_TO, addr);

    return originalRecipients;
}

Composer::RecipientList RecipientListModel::deduplicatedAndJustToCcBcc(Composer::RecipientList input)
{
    QList<Imap::Message::MailAddress> to, cc, bcc;

    Q_FOREACH(const Composer::Recipient &recipient, input) {
        switch (recipient.first) {
        case Composer::ADDRESS_TO:
            to << recipient.second;
            break;
        case Composer::ADDRESS_CC:
            cc << recipient.second;
            break;
        case Composer::ADDRESS_BCC:
            bcc << recipient.second;
            break;
        case Composer::ADDRESS_FROM:
        case Composer::ADDRESS_SENDER:
        case Composer::ADDRESS_REPLY_TO:
            // that's right, ignore these
            break;
        }
    }

    // Keep processing the To, Cc and Bcc fields, making sure that no duplicates (where comparing just the addresses) are present
    // in any of them and also making sure that an address is present in at most one of the (To, Cc, Bcc) groups.
    Composer::RecipientList result;
    QSet<QPair<QString, QString> > alreadySeen;

    Q_FOREACH(const Imap::Message::MailAddress &addr, to) {
        QPair<QString, QString> item = qMakePair(addr.mailbox, addr.host);
        if (!alreadySeen.contains(item)) {
            result << qMakePair(Composer::ADDRESS_TO, addr);
            alreadySeen.insert(item);
        }
    }

    Q_FOREACH(const Imap::Message::MailAddress &addr, cc) {
        QPair<QString, QString> item = qMakePair(addr.mailbox, addr.host);
        if (!alreadySeen.contains(item)) {
            result << qMakePair(result.isEmpty() ? Composer::ADDRESS_TO : Composer::ADDRESS_CC, addr);
            alreadySeen.insert(item);
        }
    }

    Q_FOREACH(const Imap::Message::MailAddress &addr, bcc) {
        QPair<QString, QString> item = qMakePair(addr.mailbox, addr.host);
        if (!alreadySeen.contains(item)) {
            result << qMakePair(Composer::ADDRESS_BCC, addr);
            alreadySeen.insert(item);
        }
    }

    return result;
}

Composer::RecipientList RecipientListModel::mapRecipients(Composer::RecipientList input, const QMap<Composer::RecipientKind, Composer::RecipientKind> &mapping)
{

    Composer::RecipientList::iterator recipient = input.begin();
    while (recipient != input.end()) {
        QMap<Composer::RecipientKind, Composer::RecipientKind>::const_iterator operation = mapping.constFind(recipient->first);
        if (operation == mapping.constEnd()) {
            recipient = input.erase(recipient);
        } else if (*operation != recipient->first) {
            recipient->first = *operation;
            ++recipient;
        } else {
            // don't modify items which don't need modification
            ++recipient;
        }
    }
    return input;
}

bool RecipientListModel::buildRecipientList(const int mode, const Composer::RecipientList &originalRecipients, const QList<QUrl> &headerListPost, const bool headerListPostNo)
{
    if (!m_recipients.isEmpty()) {
        clear();
    }
    Composer::RecipientList newRecipientsList;
    switch (mode) {
    case ReplyMode::REPLY_ALL:
        prepareReplyAll(originalRecipients, newRecipientsList);
        break;
    case ReplyMode::REPLY_ALL_BUT_ME:
        break;
    case ReplyMode::REPLY_LIST:
        prepareReplyList(headerListPost, headerListPostNo, newRecipientsList);
        break;
    case ReplyMode::REPLY_PRIVATE:
        prepareReply(originalRecipients, headerListPost, newRecipientsList);
        break;
    case ReplyMode::REPLY_NONE:
        break;
    }
    return appendRecipientsFromList(newRecipientsList);
}

bool RecipientListModel::prepareReplyAll(const Composer::RecipientList &recipients, Composer::RecipientList &output)
{
    QMap<Composer::RecipientKind, Composer::RecipientKind> mapping;
    mapping[Composer::ADDRESS_FROM] = Composer::ADDRESS_TO;
    mapping[Composer::ADDRESS_REPLY_TO] = Composer::ADDRESS_CC;
    mapping[Composer::ADDRESS_TO] = Composer::ADDRESS_CC;
    mapping[Composer::ADDRESS_CC] = Composer::ADDRESS_CC;
    mapping[Composer::ADDRESS_BCC] = Composer::ADDRESS_BCC;
    Composer::RecipientList res = deduplicatedAndJustToCcBcc(mapRecipients(recipients, mapping));
    if (res.isEmpty()) {
        return false;
    } else {
        output = res;
        return true;
    }
}

bool RecipientListModel::prepareReply(const Composer::RecipientList &recipients, const QList<QUrl> headerListPost, Composer::RecipientList &output)
{
    // Create a blacklist for the Reply-To filtering. This is needed to work with nasty mailing lists (hey, I run quite
    // a few like that) which do the reply-to munging.
    QList<QPair<QString, QString> > blacklist;
    Q_FOREACH(const QUrl &url, headerListPost) {
        if (url.scheme().toLower() != QLatin1String("mailto")) {
            // non-mail links are not relevant in this situation; they don't mean that we have to give up
            continue;
        }

        QStringList list = url.path().split(QLatin1Char('@'));
        if (list.size() != 2) {
            // Malformed mailto: link, maybe it relies on some fancy routing? Either way, play it safe and refuse to work on that
            return false;
        }

        blacklist << qMakePair(list[0].toLower(), list[1].toLower());
    }

    // Now gather all addresses from the From and Reply-To headers, taking care to skip munged Reply-To from ML software
    Composer::RecipientList originalFrom, originalReplyTo;
    Q_FOREACH(const Composer::Recipient &recipient, recipients) {
        switch (recipient.first) {
        case Composer::ADDRESS_FROM:
            originalFrom << qMakePair(Composer::ADDRESS_TO, recipient.second);
            break;
        case Composer::ADDRESS_REPLY_TO:
            if (blacklist.contains(qMakePair(recipient.second.mailbox.toLower(), recipient.second.host.toLower()))) {
                // This is the safe situation, this item in the Reply-To is set to a recognized mailing list address.
                // We can safely ignore that.
            } else {
                originalReplyTo << qMakePair(Composer::ADDRESS_TO, recipient.second);
            }
            break;
        default:
            break;
        }
    }

    if (!originalReplyTo.isEmpty()) {
        // Prefer replying to the (ML-demunged) Reply-To addresses
        output = originalReplyTo;
        return true;
    } else if (!originalFrom.isEmpty()) {
        // If no usable thing is in the Reply-To, fall back to anything in From
        output = originalFrom;
        return true;
    } else {
        // No recognized addresses -> bail out
        return false;
    }
}

bool RecipientListModel::prepareReplyList(const QList<QUrl> &headerListPost, const bool headerListPostNo, Composer::RecipientList &output)
{
    if (headerListPostNo)
        return false;

    Composer::RecipientList res;
    Q_FOREACH(const QUrl &url, headerListPost) {
        if (url.scheme().toLower() != QLatin1String("mailto"))
            continue;

        QStringList mail = url.path().split(QLatin1Char('@'));
        if (mail.size() != 2)
            continue;

        res << qMakePair(Composer::ADDRESS_TO, Imap::Message::MailAddress(QString(), QString(), mail[0], mail[1]));
    }

    if (!res.isEmpty()) {
        output = deduplicatedAndJustToCcBcc(res);
        return true;
    }

    return false;
}

bool RecipientListModel::appendRecipientsFromList(const Composer::RecipientList &recipients)
{

    Q_FOREACH(const Composer::Recipient &r, recipients) {
        Recipient *newRecipient = new Recipient(this);
        Recipient::RecipientKind kind;
        switch (r.first) {
        case Composer::ADDRESS_TO:
            kind = Recipient::ADDRESS_TO;
            break;
        case Composer::ADDRESS_CC:
            kind = Recipient::ADDRESS_CC;
            break;
        case Composer::ADDRESS_BCC:
            kind = Recipient::ADDRESS_BCC;
            break;
        case Composer::ADDRESS_FROM:
            kind = Recipient::ADDRESS_FROM;
            break;
        case Composer::ADDRESS_REPLY_TO:
            kind = Recipient::ADDRESS_REPLY_TO;
        case Composer::ADDRESS_SENDER:
            kind = Recipient::ADDRESS_SENDER;
            break;
        }
        newRecipient->setRecipientKind(kind);
        newRecipient->setRecipientAddress(r.second.asPrettyString());
        appendRow(newRecipient);
    }
    return true;
}

bool RecipientListModel::removeRow(int row, const QModelIndex &index)
{
    if (row >= 0 || row < m_recipients.size()) {
        beginRemoveRows(index, row, row);
        Recipient *r = m_recipients.takeAt(row);
        r->deleteLater();
        r = NULL;
        endRemoveRows();
        emit countChanged();
        return true;
    }
    return false;
}

bool RecipientListModel::removeRows(int row, int count, const QModelIndex &index)
{
    if (row >= 0 && count > 0 && (row + count) <= m_recipients.size())
    {
        beginRemoveRows(index, row, row + count - 1);
        Recipient *r = m_recipients.takeAt(row);
        Q_ASSERT(r);
        r->deleteLater();
        r = 0;
        endRemoveRows();
        emit countChanged();
        return true;
    }
    return false;
}

void RecipientListModel::clear()
{
    if (m_recipients.size() == 0) {
        return;
    }
    beginRemoveRows(QModelIndex(), 0, m_recipients.size());
    // since the recipients are owned by the qmlengine
    // lets just let the garbage collector pick them up
    m_recipients.clear();
    endRemoveRows();
    if (!m_ccIncludes.isEmpty()) {
        setIncludes(m_ccIncludes, true);
    }
    if (!m_bccIncludes.isEmpty()) {
        setIncludes(m_bccIncludes, false);
    }
    emit countChanged();
}

void RecipientListModel::setIncludes(const QStringList &includes, const bool isCc)
{
    auto removeOldAddNew = [=](const QStringList &oldList, const QStringList &newList) {
        // we need to remove the current and add the new ones
        Q_FOREACH(const QString &addr, oldList) {
            for (int i = 0; i < m_recipients.size(); ++i) {
                if (m_recipients.at(i)->recipientAddress() == addr) {
                    removeRow(i);
                }
            }
        }

        Q_FOREACH(const QString &addr, newList) {
            Recipient *r = new Recipient(this);
            r->setRecipientAddress(addr);
            if (isCc) {
                r->setRecipientKind(Recipient::ADDRESS_CC);
            } else {
                r->setRecipientKind(Recipient::ADDRESS_BCC);
            }
            m_recipients.append(QList<Recipient *>() << r);
        }
    };

    if (isCc ) {
        if (includes == m_ccIncludes || !includes.size()) {
            return;
        }
        removeOldAddNew(m_ccIncludes, includes);
        m_ccIncludes = includes;
    } else {
        if (includes == m_bccIncludes || !includes.size()) {
            return;
        }
        removeOldAddNew(m_bccIncludes, includes);
        m_bccIncludes = includes;
    }
}

QVariant RecipientListModel::get(int index)
{
    if (index < 0 || index > m_recipients.size()) {
        return QVariant();
    }
    Recipient *recipient = m_recipients.at(index);
    QMap<QString, QVariant> recipientData;
    recipientData.insert(QString("recipientAddress"), recipient->recipientAddress());
    recipientData.insert(QString("recipientKind"), recipient->recipientKind());
    return QVariant(recipientData);
}

Composer::RecipientList RecipientListModel::toRecipientList() const
{
    Composer::RecipientList recipientList;
    Q_FOREACH(Recipient *r, m_recipients) {
        Composer::Recipient recipient;
        switch (r->recipientKind()) {
        case Recipient::ADDRESS_TO:
            recipient.first = Composer::ADDRESS_TO;
            break;
        case Recipient::ADDRESS_CC:
            recipient.first = Composer::ADDRESS_CC;
            break;
        case Recipient::ADDRESS_BCC:
            recipient.first = Composer::ADDRESS_BCC;
            break;
        case Recipient::ADDRESS_FROM:
        case Recipient::ADDRESS_REPLY_TO:
        case Recipient::ADDRESS_SENDER:
            break;
        default:
            recipient.first = Composer::ADDRESS_TO;
            break;
        }

        Imap::Message::MailAddress mailAddress;
        if (Imap::Message::MailAddress::fromPrettyString(mailAddress, r->recipientAddress())) {
            recipient.second = mailAddress;
        }
        recipientList.append(recipient);
    }
    return recipientList;
}

bool RecipientListModel::buildRecipientList(const int mode, const QModelIndex &message)
{
    // first lets check this message is actually valid
    if (!message.isValid()) {
        return false;
    }
    clear();

    Composer::RecipientList allRecipients = getRecipientsFromMessage(message);
    QList<QUrl> listPost;
    Q_FOREACH(const QVariant &item, message.data(Imap::Mailbox::RoleMessageHeaderListPost).toList()) {
        listPost << item.toUrl();
    }

    return buildRecipientList(mode, allRecipients, listPost, message.data(Imap::Mailbox::RoleMessageHeaderListPostNo).toBool());
}
}
}
