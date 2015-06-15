/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
/* Parts of this were copied/modified from trojita gui/PartWidgetFactory.cpp */
#include "MessageBuilder.h"
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>
#include "UiUtils/Formatting.h"
#include "Imap/Parser/Message.h"
#include "Imap/Parser/MailAddress.h"
#include "Imap/Model/ItemRoles.h"
#include "Imap/Network/MsgPartNetAccessManager.h"

namespace Dekko
{
namespace Utils
{
MessageBuilder::MessageBuilder(QObject *parent) :
    QObject(parent), m_msgModel(0), m_plainTextFormatter(new PlainTextFormatter), m_preferPlainText(false),
    m_showHiddenParts(false)
{
}

QObject *MessageBuilder::messageModel()
{
    return m_msgModel;
}

void MessageBuilder::setMessageModel(QObject *model)
{
    m_msgModel = qobject_cast<Models::MessageModel *>(model);
    emit modelChanged();
}

QString MessageBuilder::message() const
{
    return m_message;
}

void MessageBuilder::buildMessage()
{
    if (!m_msgModel) {
        return;
    }

    m_message = QString();
    m_bodyParts = QString();
    m_attachmentParts.clear();

    if (!m_msgIndex.isValid())
        m_msgIndex = m_msgModel->messageIndex();

    m_message += getDocumentStart();
    // So we start with the header container
    m_message += QString("<div class=\"header_container\">");
    // add the message date
    m_message += buildDateContainer();
    // now add the header section. This holds the header fields i.e To, From, Cc & Bcc
    m_message += QString("<div class=\"header\">");
    // Now we build the message envelope this returns a header_container consisting of title i.e "To" and addresses for that title
    m_message += buildMessageEnvelope();
    // end header section
    m_message += QString("</div>");
    // End header_container
    m_message += QString("</div>");
    // We need a subject
    m_message += getSubjectField();
    // Now walk the msg parts to sort attachments and message body
    walkPart(m_msgIndex.child(0, 0), 0, false);
    // So now we build a list of attachments, text/plain and text/html body parts
    // Once that is complete we can then build the rest of the message
    // now attachments
    if (!m_attachmentParts.isEmpty()) {
        m_message += getAttachmentContainerStart(m_attachmentParts.size() > 1);
        if (m_attachmentParts.size() > 1)
            m_message += getAttachmentCollapseButton();
        m_message += "<ul>";
        m_message += m_attachmentParts.join(' ');
        m_message += getAttachmentContainerEnd();
        m_message += getAttachmentPopover();
    }
    m_message += getPopoverReverseMouseArea();
    m_message += getRecipientPopover();
    // Now finally add the body
    m_message += m_bodyParts;
    // and were done!! so wrap up the final markup and off we go.
    finishBuildingMessage();

}

void MessageBuilder::finishBuildingMessage()
{
    m_message += getDocumentEnd();
    emit messageChanged();
}

void MessageBuilder::buildPlainTextPart()
{
    QString message = m_plainTextFormatter.data()->message();
    if(message.isEmpty()) {
        message = tr("Error loading message");
    }
    m_message += QString("<div class=\"plaintext_container\">%1</div>").arg(message);
    finishBuildingMessage();
}

QString MessageBuilder::getDocumentStart()
{
    QString documentStart("<!DOCTYPE html>"
                          "<html>"
                          "<head>"
                          "<meta charset=\"utf-8\">"
                          "<meta name=\"description\" content=\"An Ubuntu HTML5 application\">"
                          "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1, user-scalable=0\">"
                          "<link rel=\"stylesheet\" href=\"./app.css\" type=\"text/css\" />"
                          "<link href=\"/usr/share/ubuntu-html5-ui-toolkit/0.1/ambiance/css/appTemplate.css\" rel=\"stylesheet\" type=\"text/css\" />"
                          "<script src=\"/usr/share/ubuntu-html5-ui-toolkit/0.1/ambiance/js/fast-buttons.js\"></script>"
                          "<script src=\"./UbuntuUI/core.js\"></script>"
                          "<script src=\"/usr/share/ubuntu-html5-ui-toolkit/0.1/ambiance/js/buttons.js\"></script>"
                          "<script src=\"/usr/share/ubuntu-html5-ui-toolkit/0.1/ambiance/js/page.js\"></script>"
                          "<script src=\"/usr/share/ubuntu-html5-ui-toolkit/0.1/ambiance/js/tab.js\"></script>"
                          "<script src=\"/usr/share/ubuntu-html5-ui-toolkit/0.1/ambiance/js/popovers.js\"></script>"
                          "</head>"
                          "<body>"
                          "<div data-role=\"mainview\">"
                          "<div data-role=\"content\">"
                          "<div data-role=\"tab\" id=\"dekko-page\">");
    return documentStart;
}

QString MessageBuilder::getDocumentEnd()
{
    QString documentEnd("</div></div></div><script src=\"./app.js\"></script></body></html>");
    return documentEnd;
}

QString MessageBuilder::buildMessageEnvelope()
{
    if (!m_msgIndex.isValid()) {
        return QString();
    }
    QString envelope; // formatted evelope to return
    // Lets get the envelope directly as we don't really need to work on it as a qvariant in this context
    const Imap::Message::Envelope msg = m_msgIndex.data(Imap::Mailbox::RoleMessageEnvelope).value<Imap::Message::Envelope>();

    if(!msg.from.isEmpty()) {
        envelope += buildHeaderField(tr("From:"), buildAddressList(msg.from));
    }
    if(!msg.to.isEmpty()) {
        envelope += buildHeaderField(tr("To:"), buildAddressList(msg.to));
    }
    if(!msg.cc.isEmpty()) {
        envelope += buildHeaderField(tr("Cc:"), buildAddressList(msg.cc));
    }
    if(!msg.bcc.isEmpty()) {
        envelope += buildHeaderField(tr("Bcc:"), buildAddressList(msg.bcc));
    }
    return envelope;
}

QString MessageBuilder::buildDateContainer()
{
    return QString("<div class=\"date_container\">%1</div>").arg(UiUtils::Formatting::prettyDate(
        m_msgIndex.data(Imap::Mailbox::RoleMessageDate).toDateTime().toLocalTime()));
}

QString MessageBuilder::buildHeaderField(const QString &title, const QString &addresses)
{

    return QString("<div class=\"field\">"
                   "<div class=\"header_title\">%1</div>"
                   "<div class=\"value\">%2</div></div>"
                   ).arg(title).arg(addresses);
}

QString MessageBuilder::buildAddressList(const QList<Imap::Message::MailAddress> &addresses)
{
    if (addresses.isEmpty()) {
        return QString();
    }
    // create a list of addresses;
    QStringList addrList;
    Q_FOREACH(const Imap::Message::MailAddress &addr, addresses) {
        //TODO: make this configurable. For phone as a default lets only show the name if available
        QString name = addr.prettyName(Imap::Message::MailAddress::FORMAT_JUST_NAME);
        QString readable = addr.prettyName(Imap::Message::MailAddress::FORMAT_READABLE);
        addrList.append(QString(
            "<a class=\"value_container recipient\" data-mailto=\"%1\" onclick=\"javascript:recipientClicked(this)\">"
                "<span class=\"address_name\">%2</span>"
                "<img src=\"/usr/share/icons/suru/actions/scalable/starred.svg\"/>"
            "</a>").arg(readable).arg(name.isEmpty() ? readable : name));
    }

    return addrList.join(QLatin1String(", "));
}

QString MessageBuilder::getSubjectField()
{
    return QString("<section data-role=\"list\" id=\"subject\">"
                   "<ul>"
                   "<li>"
                   "<p>%1</p>"
                   "</li>"
                   "</ul>"
                   "</section>").arg(m_msgIndex.data(Imap::Mailbox::RoleMessageSubject).toString());
}

QString MessageBuilder::getAttachment(const QString partId, const QString iconName, const QString &fileName, const QString &size, const QString mimetype)
{
    return QString("<li id=\"attachment_%1\" class=\"attachment\" onclick=\"javascript:attachmentClicked(this)\" data-mimetype=\"%2\">"
                   "<aside id=\"attach_aside\">"
                   "<object class=\"attachment_image\" data=\"/usr/share/icons/suru/mimetypes/scalable/%3-symbolic.svg\">"
                   "<img class=\"attachment_image\" src=\"/usr/share/icons/suru/mimetypes/scalable/empty-symbolic.svg\">"
                   "</object>"
                   "</aside>"
                   "<p>%4 <b>(%5)</b></p>"
                   "</li>").arg(partId).arg(mimetype).arg(iconName).arg(fileName).arg(size);
}

QString MessageBuilder::getAttachmentContainerStart(bool exandable)
{
    if (!exandable)
        return QString("<section data-role=\"list\" id=\"attachments_list\">");
    else
        return QString("<section data-role=\"list\" id=\"attachments_list\" class=\"expandable\">");
}

QString MessageBuilder::getAttachmentPopover()
{
    return QString( "<div class=\"popover\" data-gravity=\"n\" id=\"attachment_options_popover\">"
                    "<ul class=\"list\">"
                    "<li onclick=\"javacript:downloadAttachment()\"><a>Open</a></li>"
                    "</ul>"
                    "</div>");
}

QString MessageBuilder::getAttachmentCollapseButton()
{
    return QString("<div id='attachment-collapse-button-container' onclick='javascript: toggleExpandAttachment()'>"
                   "<span id='attachemnt-count'>%1</span>"
                   "<span>&nbsp;</span>"
                   "<img src='/usr/share/icons/suru/actions/scalable/attachment.svg'></img>"
                   "<span>&nbsp;</span>"
                   "<img class='down-img' src='/usr/share/icons/suru/actions/scalable/down.svg'></img>"
                   "<img class='up-img' src='/usr/share/icons/suru/actions/scalable/up.svg'></img>"
                   "</div>").arg(m_attachmentParts.size());
}

QString MessageBuilder::getPopoverReverseMouseArea()
{
    return QString("<div id='popover_inverse_mousearea' onclick='javascript:closePopovers()'></div>");
}

QString MessageBuilder::getRecipientPopover()
{
    return QString("<div class=\"popover\" data-gravity=\"n\" id=\"recipient-popover\">"
                    "<ul class=\"list\">"
                        "<li><a onclick=\"javascript:composeMessageTo()\">"
                            + tr("Compose message to") +
                        "</a></li>"
                        "<li id= \"addToAddressBookContextButton\"><a onclick=\"javascript:addToAddressbook()\">"
                            + tr("Add to Addressbook") +
                        "</a></li>"
                    "</ul>"
                   "</div>");
}

QString MessageBuilder::getAttachmentContainerEnd()
{
    return QString("</ul></section>");
}

QString MessageBuilder::getHtmlPart(const QString &msgUrl)
{
    return QString("<iframe id=\"textHtml\" class=\"html-container\" sandbox=\"allow-same-origin allow-top-navigation\" seamless src=\"%1\"></iframe>").arg(msgUrl);
}
// TODO: change this to work with the plaintextformatter. This just get's us up and going to start with.
QString MessageBuilder::getPlainTextPart(const QString &msgUrl)
{
    return QString("<iframe id=\"textPlain\" class=\"html-container\" sandbox=\"allow-same-origin allow-top-navigation\" seamless src=\"%1\"></iframe>").arg(msgUrl);
}

void MessageBuilder::buildAttachmentPart(const QModelIndex &partIndex)
{
    QString mimedescription = partIndex.data(Imap::Mailbox::RolePartMimeType).toString();
    QMimeType mimeType = QMimeDatabase().mimeTypeForName(mimedescription);
    QString fileSize = UiUtils::Formatting::prettySize(partIndex.data(Imap::Mailbox::RolePartOctets).toUInt(),
                                                       UiUtils::Formatting::BytesSuffix::WITH_BYTES_SUFFIX);
    QString fileName = partIndex.data(Imap::Mailbox::RolePartFileName).toString();
    QString partId = partIndex.data(Imap::Mailbox::RolePartId).toString();
    QString composition = partIndex.data(Imap::Mailbox::RolePartBodyDisposition).toString();
    // TODO. This temporarily disable viewing message as attachment because it cannot be displayed correctly.
    //       Should be able to view message/rfc822 of composition "attachment".
    if (composition != "INLINE" && mimedescription == "message/rfc822") {
        mimedescription = "";
    }
    m_attachmentParts << getAttachment(partId, mimeType.genericIconName(), fileName, fileSize, mimedescription);
}

void MessageBuilder::multipartAlternative(const QModelIndex &partIndex, const int recursionDepth)
{
    // We follow the same kind of flow as trojita here minus the widget building part
    // So we loop twice here, first to find the part we are interested in and then actually create the markup for it
    // There is also the option to showhidden parts but i doubt there will be much use of that, but it's good to have
    int interestedIndex = -1;
    for (int i = 0; i < partIndex.model()->rowCount(partIndex); ++i) {
        QModelIndex nextPart = partIndex.child(i, 0);
        Q_ASSERT(nextPart.isValid());
        QString mimeType = nextPart.data(Imap::Mailbox::RolePartMimeType).toString();
        const bool isPlainText = mimeType == QLatin1String("text/plain");
        const bool isHtml = mimeType == QLatin1String("text/html");
        if (isPlainText && m_preferPlainText) {
            interestedIndex = i;
        } else if (isHtml && !m_preferPlainText) {
            interestedIndex = i;
        }
    }
    // Right so if our interested index is still at -1 then we set our preferred part to the last
    // index in our list, as later parts should always win.
    int preferredPart = interestedIndex == -1 ? partIndex.model()->rowCount(partIndex) - 1 : interestedIndex;

    // Now the second loop to walk the parts and create the actual markup for our preferredPart and others if showHidden is true
    for (int i = 0; i < partIndex.model()->rowCount(partIndex); ++i) {
        QModelIndex nextPart = partIndex.child(i, 0);
        Q_ASSERT(nextPart.isValid());
        walkPart(nextPart, recursionDepth + 1, i == preferredPart);
    }
}

void MessageBuilder::multipartSigned(const QModelIndex &partIndex, const int recursionDepth)
{
    Q_ASSERT(partIndex.isValid());
    emit error(QString("Multipart/signed un-supported at this time"));
    // lets walk it and see if we find any displayble though
    uint childrenCount = partIndex.model()->rowCount(partIndex);
    qDebug() << "children count is" << childrenCount;
    if (childrenCount == 1) {
        qDebug() << "Children count is 1";
        QModelIndex anotherPart = partIndex.child(0, 0);
        Q_ASSERT(anotherPart.isValid()); // guaranteed by the MVC
        walkPart(anotherPart, recursionDepth + 1, true);
    } else if (childrenCount != 2) {
        qDebug() << QString("Malformed multipart/signed message: %1 nested parts").arg(QString::number(childrenCount));
        emit error(QString("Malformed multipart/signed message: %1 nested parts").arg(QString::number(childrenCount)));
        return;
    } else {
        Q_ASSERT(childrenCount == 2); // from the if logic; FIXME: refactor
        QModelIndex anotherPart = partIndex.child(0, 0);
        Q_ASSERT(anotherPart.isValid()); // guaranteed by the MVC
        walkPart(anotherPart, recursionDepth + 1, true);
    }
    return;
}

void MessageBuilder::genericMultipart(const QModelIndex &partIndex, const int recursionDepth)
{
    bool plainTextSeen = false;
    bool htmlSeen = false;
    QModelIndex plainTextSeenIndex;
    QModelIndex htmlSeenIndex;
    int rd = 0;
    for (int i = 0; i < partIndex.model()->rowCount(partIndex); ++i) {
        QModelIndex nextPart = partIndex.child(i, 0);
        QString mimeType = nextPart.data(Imap::Mailbox::RolePartMimeType).toString();
        qDebug() << "MIMETYPE:" << mimeType;

        bool isAttachment = !nextPart.data(Imap::Mailbox::RolePartFileName).toString().isEmpty();
        if (mimeType == QLatin1String("text/plain") && !plainTextSeen && !isAttachment) {
            plainTextSeen = true;
            plainTextSeenIndex = nextPart;
            rd = recursionDepth + 1;
        } else if (mimeType == QLatin1String("text/html") && !htmlSeen && !isAttachment) {
            htmlSeen = true;
            htmlSeenIndex = nextPart;
            rd = recursionDepth + 1;
        } else {
            Q_ASSERT(nextPart.isValid());
            walkPart(nextPart, recursionDepth + 1, true);
        }
    }
    if (htmlSeen  && plainTextSeen) {
        if (m_preferPlainText) {
            walkPart(plainTextSeenIndex, rd, true);
        } else {
            walkPart(htmlSeenIndex, rd, true);
        }
    } else if (htmlSeen) {
        walkPart(htmlSeenIndex, rd, true);
    } else if (plainTextSeen) {
        walkPart(plainTextSeenIndex, rd, true);
    } else {
        emit error(QString("No usable message part"));
    }
}

void MessageBuilder::walkPart(const QModelIndex &partIndex, const int recursionDepth, const bool isPreferredIndex)
{
    Q_ASSERT(partIndex.isValid());
    if (recursionDepth > 1000) {
        emit error(QString("We are too deep in the nesting, stopping to prevent stack exhaustion"));
        return;
    }
    QString mimeType = partIndex.data(Imap::Mailbox::RolePartMimeType).toString().toLower();
    qDebug() << "Mime type: " << mimeType << "Depth: " << recursionDepth;
    bool isMessageRfc822 = mimeType == QLatin1String("message/rfc822");
    bool isCompoundMimeType = mimeType.startsWith(QLatin1String("multipart/")) || isMessageRfc822;

    // Check whether we can render this MIME type at all
    QStringList allowedMimeTypes;
    allowedMimeTypes << QLatin1String("text/html") << QLatin1String("text/plain") << QLatin1String("image/jpeg") <<
                        QLatin1String("image/jpg") << QLatin1String("image/pjpeg") << QLatin1String("image/png") << QLatin1String("image/gif");
    bool recognizedMimeType = isCompoundMimeType || allowedMimeTypes.contains(mimeType);
    bool isDerivedMimeType = false;
    if (!recognizedMimeType) {
        // QMimeType's docs say that one shall use inherit() to check for "is this a recognized MIME type".
        // E.g. text/x-csrc inherits text/plain.
        QMimeType partType = QMimeDatabase().mimeTypeForName(mimeType);
        Q_FOREACH(const QString &candidate, allowedMimeTypes) {
            if (partType.isValid() && !partType.isDefault() && partType.inherits(candidate)) {
                // Looks like we shall be able to show this
                recognizedMimeType = true;
                // If it's a derived MIME type, it makes sense to not block inline display, yet still make it possible to hide it
                // using the regular attachment controls
                isDerivedMimeType = true;
                break;
            }
        }
    }
    // Check whether it shall be wrapped inside an AttachmentView
    // From section 2.8 of RFC 2183: "Unrecognized disposition types should be treated as `attachment'."
    const QByteArray contentDisposition = partIndex.data(Imap::Mailbox::RolePartBodyDisposition).toByteArray().toLower();
    const bool isInline = contentDisposition.isEmpty() || contentDisposition == "inline";
    const bool looksLikeAttachment = !partIndex.data(Imap::Mailbox::RolePartFileName).toString().isEmpty();
    const bool wrapInAttachmentView = (looksLikeAttachment || !isInline || !recognizedMimeType || isDerivedMimeType || isMessageRfc822);
    if(wrapInAttachmentView) {
        if (!isCompoundMimeType) {
            partIndex.data(Imap::Mailbox::RolePartForceFetchFromCache);
        }
        buildAttachmentPart(partIndex);
        return;
    }
    if (mimeType.startsWith(QLatin1String("multipart/"))) {
        // it's a compound part
        if (mimeType == QLatin1String("multipart/alternative")) {
            return multipartAlternative(partIndex, recursionDepth);
        } else if (mimeType == QLatin1String("multipart/signed")) {
            return multipartSigned(partIndex, recursionDepth); // TODO: get multipart/signed working
        } else if (mimeType == QLatin1String("multipart/related")) {
            // We need to find the mainpart
            QModelIndex mainPartIndex;
            QVariant mainPartCID = partIndex.data(Imap::Mailbox::RolePartMultipartRelatedMainCid);
            if (mainPartCID.isValid()) {
                mainPartIndex = Imap::Network::MsgPartNetAccessManager::cidToPart(partIndex, mainPartCID.toByteArray());
            }
            if (!mainPartIndex.isValid()) {
                // The Content-Type-based start parameter was not terribly useful. Let's find the HTML part manually.
                QModelIndex candidate = partIndex.child(0, 0);
                while (candidate.isValid()) {
                    if (candidate.data(Imap::Mailbox::RolePartMimeType).toString() == QLatin1String("text/html")) {
                        mainPartIndex = candidate;
                        break;
                    }
                    candidate = candidate.sibling(candidate.row() + 1, 0);
                }
            }

            if (mainPartIndex.isValid()) {
                if (mainPartIndex.data(Imap::Mailbox::RolePartMimeType).toString() == QLatin1String("text/html")) {
                    return walkPart(mainPartIndex, recursionDepth+1, true);
                } else {
                    // Sorry, but anything else than text/html is by definition suspicious here. Better than picking some random
                    // choice, let's just show everything.
                    return genericMultipart(partIndex, recursionDepth);
                }
            } else {
                // The RFC2387's wording is clear that in absence of an explicit START argument, the first part is the starting one.
                // On the other hand, I've seen real-world messages whose first part is some utter garbage (an image sent as
                // application/octet-stream, for example) and some *other* part is an HTML text. In that case (and if we somehow
                // failed to pick the HTML part by a heuristic), it's better to show everything.
                return genericMultipart(partIndex, recursionDepth);
            }
        } else {
            return genericMultipart(partIndex, recursionDepth);
        }
    } else if (mimeType == QLatin1String("message/rfc822")) {
        return genericMultipart(partIndex, recursionDepth);
    } else {
        partIndex.data(Imap::Mailbox::RolePartForceFetchFromCache);
        const bool isPlainText = mimeType == QLatin1String("text/plain");
        const bool isHtml = mimeType == QLatin1String("text/html");
        if (isPlainText) {
            if (isPreferredIndex || m_showHiddenParts || !recursionDepth) {
                // This is a temporary fix. We still need to parse this message with the plaintextformatter
                qDebug() << "Settings plaintextpart";
                m_bodyParts += getPlainTextPart(QString("trojita-imap://msg/" + partIndex.data(Imap::Mailbox::RolePartPathToPart).toString()
                                                        + "?requestFormatting=true"));
                return;
            }
        } else if (isHtml) {
            if (isPreferredIndex || m_showHiddenParts || !recursionDepth) {
                m_bodyParts += getHtmlPart(QString("trojita-imap://msg/" + partIndex.data(Imap::Mailbox::RolePartPathToPart).toString()));
                return;
            }
        }
    }
}

}
}
