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
#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <QObject>
#include <QModelIndex>
#include "app/Models/MessageModel.h"
#include "app/Utils/PlainTextFormatter.h"

namespace Dekko
{
namespace Models
{
class MessageModel;
}
namespace Utils
{

class MessageBuilder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *messageModel READ messageModel WRITE setMessageModel NOTIFY modelChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(bool preferPlainText MEMBER m_preferPlainText)
    Q_PROPERTY(bool showHiddenParts MEMBER m_showHiddenParts)
    Q_PROPERTY(QModelIndex msgIndex MEMBER m_msgIndex)

public:
    explicit MessageBuilder(QObject *parent = 0);

    QObject *messageModel();
    void setMessageModel(QObject *model);

    QString message() const;

signals:
    void modelChanged();
    void messageChanged();
    void error(QString msg);

public slots:
    void buildMessage();

private slots:
    void finishBuildingMessage();
    void buildPlainTextPart();

protected:
    QString getDocumentStart();
    QString getDocumentEnd();
    QString buildMessageEnvelope();
    QString buildDateContainer();
    QString buildHeaderField(const QString &title, const QString &addresses);
    QString buildAddressList(const QList<Imap::Message::MailAddress> &addresses);
    QString getSubjectField();
    QString getAttachment(const QString partId, const QString iconName, const QString &fileName, const QString &size, const QString mimetype);
    QString getAttachmentContainerStart(bool exandable = false);
    QString getAttachmentContainerEnd();
    QString getAttachmentPopover();
    QString getAttachmentCollapseButton();
    QString getPopoverReverseMouseArea();
    QString getRecipientPopover();

    QString getHtmlPart(const QString &msgUrl);
    QString getPlainTextPart(const QString &msgUrl);
    void buildAttachmentPart(const QModelIndex &partIndex);

    void multipartAlternative(const QModelIndex &partIndex, const int recursionDepth);
    void multipartSigned(const QModelIndex &partIndex, const int recursionDepth);
    void genericMultipart(const QModelIndex &partIndex, const int recursionDepth);
    void message822(const QModelIndex &partIndex, const int recursionDepth);

    void walkPart(const QModelIndex &partIndex, const int recursionDepth, const bool isPreferredIndex);

private:
    Models::MessageModel *m_msgModel;
    QModelIndex m_msgIndex;
    QPointer<PlainTextFormatter> m_plainTextFormatter;
    QString m_message;
    bool m_preferPlainText;
    bool m_showHiddenParts;
    QString m_bodyParts;
    QStringList m_attachmentParts;
};
}
}

#endif // MESSAGEBUILDER_H
