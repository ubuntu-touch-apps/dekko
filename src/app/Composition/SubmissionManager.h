/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>
 * Copyright (C) 2014 Giulio Collura <random.cpp@gmail.com>

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
#ifndef SUBMISSIONMANAGER_H
#define SUBMISSIONMANAGER_H

#include <QObject>
#include <QPersistentModelIndex>
#include "Composer/Recipients.h"
#include "Imap/Model/OneMessageModel.h"
#include "ComposerUtils.h"
#include "app/Models/RecipientListModel.h"
#include "app/Models/AttachmentListModel.h"
#include "app/Accounts/Account.h"
#include "app/Settings/MailboxSettings.h"
#include "Imap/Model/MsgListModel.h"

namespace Imap {
class ImapAccess;
namespace Mailbox {
class Model;
class OneMessageModel;
}
}

namespace MSA {
class MSAFactory;
}

namespace Composer {

class MessageComposer;
class Submission;
class Recipients;
}

namespace Dekko
{

class SubmissionManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int progressMin READ progressMin WRITE setProgressMin NOTIFY progressChanged)
    Q_PROPERTY(int progressMax READ progressMax WRITE setProgressMax NOTIFY progressChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(bool progressIndeterminate READ progressIndeterminate NOTIFY progressIndeterminateChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage NOTIFY progressChanged)
    Q_PROPERTY(QByteArray inReplyTo READ inReplyTo WRITE setInReplyTo NOTIFY inReplyToChanged)
    Q_PROPERTY(QObject *recipientsModel READ recipientsModel NOTIFY modelsChanged)
    Q_PROPERTY(QObject *attachmentsModel READ attachmentsModel NOTIFY modelsChanged)

public:
    SubmissionManager(QObject *parent = 0);

    Composer::MessageComposer *composer();
    Composer::Submission *submission();

    QObject *recipientsModel() const;
    QObject *attachmentsModel() const;

    Q_INVOKABLE bool setupConnection();
    Q_INVOKABLE bool buildMessage(const QString &from, const QString &subject, const QString &mailText);
    Q_INVOKABLE void send();

    Q_INVOKABLE void setReplyingToMessage(const QString &mailbox, const uint uid);
    Q_INVOKABLE QString quoteText();
    Q_INVOKABLE QVariantMap parseMailtoUrl(QString mailtoUrl);
    int progressMin() const;
    int progressMax() const;
    int progress() const;
    bool progressIndeterminate() const;
    QString statusMessage() const;
    QByteArray inReplyTo() const;


public slots:
    void setPassword(const QString &password);
    void cancelPassword();
    void setProgressMin(const int &val);
    void setProgressMax(const int &val);
    void setProgress(const int &val);
    void setStatusMessage(const QString &status);
    void setInReplyTo(const QByteArray &addresses);
    void setAccount(QObject * account);
    void setReferences(const QList<QByteArray> &refs);

signals:
    void succeeded();
    void failed(const QString &message);
    void messageBuildFailed(const QString &message);
    void passwordRequested(const QString &user, const QString &host);
    void gotPassword(const QString &password);
    void progressChanged();
    void inReplyToChanged();
    void senderAddressesChanged();
    void replyToAddressesChanged();
    void modelsChanged();
    void accountChanged();
    void progressIndeterminateChanged();
private slots:
    void handleConnectionStateChanged(const Composer::Submission::SubmissionProgress &state);
private:

    Composer::Submission *m_submission;
    Dekko::Accounts::Account *m_account;
    Dekko::Models::RecipientListModel *m_recipientListModel;
    Dekko::Models::AttachmentListModel *m_attachmentListModel;

    QByteArray m_inReplyTo;
    QPersistentModelIndex m_replyingToMessage;
    Composer::RecipientList m_recipientList;
    QList<QByteArray> m_references;
    int m_progressMin;
    int m_progressMax;
    int m_progress;
    bool m_progressIndeterminate;
    QString m_statusMessage;
    QPointer<Dekko::Settings::MailboxSettings> m_mboxSettings;
    Imap::Mailbox::MsgListModel *m_msgList;
};
}
#endif // SUBMISSIONMANAGER_H
