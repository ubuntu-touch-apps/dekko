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

#ifndef IMAP_MODEL_CACHE_H
#define IMAP_MODEL_CACHE_H

#include <QUrl>
#include "MailboxMetadata.h"
#include "../Parser/Message.h"
#include "Imap/Parser/ThreadingNode.h"

/** @short Namespace for IMAP interaction */
namespace Imap
{

/** @short Classes for handling of mailboxes and connections */
namespace Mailbox
{

/** @short An abstract parent for all IMAP cache implementations */
class AbstractCache: public QObject
{
    Q_OBJECT
public:

    /** @short Helper for retrieving all data about a particular message from the cache */
    struct MessageDataBundle {
        /** @short The UID of the message */
        uint uid;
        /** @short Envelope */
        Imap::Message::Envelope envelope;
        /** @short The IMAP's INTERNALDATE */
        QDateTime internalDate;
        /** @short RFC822.SIZE */
        uint size;
        /** @short Serialized form of BODYSTRUCTURE

        Due to the complex nature of BODYSTRUCTURE and the way we use, simly
        archiving the resulting object is far from trivial. The simplest way is
        offered by Imap::Message::AbstractMessage::fromList. Therefore, this item
        contains a QVariantList as serialized by QDataStream.
        */
        QByteArray serializedBodyStructure;

        /** @short Parsed form of the References RFC5322 header */
        QList<QByteArray> hdrReferences;

        /** @short The List-Post from RFC 2368 */
        QList<QUrl> hdrListPost;
        /** @short Is the List-Post set to "NO"? */
        bool hdrListPostNo;

        MessageDataBundle(): uid(0), size(0), hdrListPostNo(false) {}

        bool operator==(const MessageDataBundle &other) const
        {
            return uid == other.uid && envelope == other.envelope && internalDate == other.internalDate &&
                    serializedBodyStructure == other.serializedBodyStructure && size == other.size &&
                    hdrReferences == other.hdrReferences && hdrListPost == other.hdrListPost &&
                    hdrListPostNo == other.hdrListPostNo;
        }
    };

    explicit AbstractCache(QObject *parent);
    virtual ~AbstractCache();

    /** @short Return a list of all known child mailboxes */
    virtual QList<MailboxMetadata> childMailboxes(const QString &mailbox) const = 0;
    /** @short Is the result of childMailboxes() fresh enough? */
    virtual bool childMailboxesFresh(const QString &mailbox) const = 0;
    /** @short Update cache info about the state of child mailboxes */
    virtual void setChildMailboxes(const QString &mailbox, const QList<MailboxMetadata> &data) = 0;

    /** @short Return previous known state of a mailbox */
    virtual SyncState mailboxSyncState(const QString &mailbox) const = 0;
    /** @short Set current syncing state */
    virtual void setMailboxSyncState(const QString &mailbox, const SyncState &state) = 0;

    /** @short Store the mapping of sequence numbers to UIDs */
    virtual void setUidMapping(const QString &mailbox, const QList<uint> &seqToUid) = 0;
    /** @short Forget the cached seq->UID mapping for given mailbox */
    virtual void clearUidMapping(const QString &mailbox) = 0;
    /** @short Retrieve sequence to UID mapping */
    virtual QList<uint> uidMapping(const QString &mailbox) const = 0;

    /** @short Remove all messages in given mailbox from the cache */
    virtual void clearAllMessages(const QString &mailbox) = 0;
    /** @short Remove all info for given message in the mailbox from cache */
    virtual void clearMessage(const QString mailbox, const uint uid) = 0;

    /** @short Returns all known data for a message in the given mailbox (except real parts data) */
    virtual MessageDataBundle messageMetadata(const QString &mailbox, uint uid) const = 0;
    virtual void setMessageMetadata(const QString &mailbox, const uint uid, const MessageDataBundle &metadata) = 0;

    /** @short Retrieve flags for one message in a mailbox */
    virtual QStringList msgFlags(const QString &mailbox, const uint uid) const = 0;
    /** @short Save flags for one message in mailbox */
    virtual void setMsgFlags(const QString &mailbox, const uint uid, const QStringList &flags) = 0;

    /** @short Return part data or a null QByteArray if none available */
    virtual QByteArray messagePart(const QString &mailbox, const uint uid, const QString &partId) const = 0;
    /** @short Save data for one message part */
    virtual void setMsgPart(const QString &mailbox, const uint uid, const QString &partId, const QByteArray &data) = 0;
    /** @short Drop the data for a message part which is no longer needed */
    virtual void forgetMessagePart(const QString &mailbox, const uint uid, const QString &partId) = 0;

    /** @short Return cached threading info for a given mailbox */
    virtual QVector<Imap::Responses::ThreadingNode> messageThreading(const QString &mailbox) = 0;
    /** @short Save information about how messages are threaded */
    virtual void setMessageThreading(const QString &mailbox, const QVector<Imap::Responses::ThreadingNode> &threading) = 0;

    /** @short How many days is it OK not to mark entries as accessed? */
    virtual void setRenewalThreshold(const int days) = 0;

    virtual QString messagePreview(const QString mailbox, const uint uid) = 0;
    virtual void setMessagePreview(const QString mailbox, const uint uid, const QString preview) = 0;

signals:
    /** @short Some cache error has occurred */
    void error(const QString &error) const;
};

}

}

#endif /* IMAP_MODEL_CACHE_H */
