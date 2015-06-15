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

#include "SubmissionManager.h"
#include <QObject>
#include <QAbstractItemModel>
#include "Composer/MessageComposer.h"
#include "Composer/Submission.h"
#include "Composer/PlainTextFormatter.h"
#include "Composer/Mailto.h"
#include "Imap/Encoders.h"
#include "Imap/Model/ImapAccess.h"
#include "Imap/Model/Model.h"
#include "app/Models/Recipient.h"
#include "Imap/Parser/MailAddress.h"
#include "MSA/Account.h"
#include "MSA/SMTP.h"
#include "MSA/Sendmail.h"

namespace Dekko
{

SubmissionManager::SubmissionManager(QObject *parent) :
    QObject(parent), m_submission(0), m_account(0), m_progressMin(0),
    m_progressMax(0), m_progress(0), m_progressIndeterminate(false), m_statusMessage(QString()), m_mboxSettings(new Dekko::Settings::MailboxSettings),
    m_msgList(0)
{
    m_recipientListModel = new Dekko::Models::RecipientListModel(this);
    m_attachmentListModel = new Dekko::Models::AttachmentListModel(this);
    connect(this, &SubmissionManager::accountChanged, [=](){
        m_mboxSettings.data()->setAccount(qobject_cast<Accounts::Account *>(m_account));
        if (m_msgList) {
            m_msgList = 0;
            delete m_msgList;
        }
        m_msgList = static_cast<Imap::Mailbox::MsgListModel *>(m_account->msgListModel());
        if (!m_msgList->mailboxName().isEmpty()) {
            m_mboxSettings.data()->setMailboxName(m_msgList->mailboxName());
        } else {
            m_mboxSettings.data()->setMailboxName(QString("global"));
        }
        connect(m_msgList, &Imap::Mailbox::MsgListModel::mailboxChanged, [=](){
            m_mboxSettings.data()->setMailboxName(m_msgList->mailboxName());
        });
    });
    connect(m_mboxSettings, &Settings::MailboxSettings::pathChanged, [=](){
        QVariantMap keys = m_mboxSettings.data()->keys();
        bool includeCC = m_mboxSettings.data()->read(keys.value("includeCC").toString()).toBool();
        bool includeBCC = m_mboxSettings.data()->read(keys.value("includeBCC").toString()).toBool();
        if (includeCC) {
            QStringList includes = m_mboxSettings.data()->read(keys.value("cc").toString()).toString().split(',');
            for (int i = 0; i < includes.size() - 1; ++i) {
                includes.replace(i, includes.at(i).trimmed());
            }
            m_recipientListModel->setIncludes(includes, true);
        }
        if (includeBCC) {
            QStringList includes = m_mboxSettings.data()->read(keys.value("bcc").toString()).toString().split(',');
            for (int i = 0; i < includes.size() - 1; ++i) {
                includes.replace(i, includes.at(i).trimmed());
            }
            m_recipientListModel->setIncludes(includes, false);
        }
    });

    emit modelsChanged();
}

Composer::MessageComposer *SubmissionManager::composer()
{
    return m_submission->composer();
}

Composer::Submission *SubmissionManager::submission()
{
    return m_submission;
}

QObject *SubmissionManager::recipientsModel() const
{
    return m_recipientListModel;
}

QObject *SubmissionManager::attachmentsModel() const
{
    return m_attachmentListModel;
}

bool SubmissionManager::setupConnection()
{
    // Delete the old pointer before creating the new one
    // We should, instead, check if some parameter changed and update the
    // configuration properly.
    if (m_submission) {
        m_submission->deleteLater();
        m_submission = 0;
        m_progressMin = 0;
        m_progressMax = 0;
        m_progress = 0;
        m_statusMessage = QString();
    }

    MSA::MSAFactory *msaFactory = 0;

    Dekko::Settings::SmtpSettings *smtpSettings = qobject_cast<Dekko::Settings::SmtpSettings *>(m_account->smtpSettings());
    Composer::Submission::Method method = (Composer::Submission::Method)((int)smtpSettings->submissionMethod());

    if (method == Composer::Submission::Method::SMTP || method == Composer::Submission::Method::SSMTP
            || method == Composer::Submission::Method::SMTP_STARTTLS) {
        msaFactory = new MSA::SMTPFactory(smtpSettings->server(),
                smtpSettings->port(),
                (Composer::Submission::Method)(smtpSettings->submissionMethod()) == Composer::Submission::Method::SSMTP,
                (Composer::Submission::Method)(smtpSettings->submissionMethod()) == Composer::Submission::Method::SMTP_STARTTLS,
                true, //smtpSettings->submissionAuthenticateEnabled(),
                smtpSettings->username());
    } else if (method == Composer::Submission::Method::SENDMAIL) {
        qDebug() << "Error Configuration not supported yet.";
        return false;
//        QStringList args = m_account->submissionPathToSendmail().split(QLatin1Char(' '));
//        if (args.isEmpty()) {
//            qDebug() << "Error Please configure the SMTP or sendmail settings in application settings.";
//            return false;
//        }
//        msaFactory = new MSA::SendmailFactory(args.takeFirst(), args);
    } else if (method == Composer::Submission::Method::IMAP_SENDMAIL) {
        qDebug() << "Error Configuration not supported yet.";
        return false;
    } else {
        qDebug() << "Error Please configure the application.";
        return false;
    }

    m_submission = new Composer::Submission(this, static_cast<Imap::Mailbox::Model *>(m_account->imapModel()), msaFactory);
    connect(m_submission, SIGNAL(failed(const QString &)), this, SIGNAL(failed(const QString &)));
    connect(m_submission, SIGNAL(succeeded()), this, SIGNAL(succeeded()));
    connect(m_submission, SIGNAL(succeeded()), m_recipientListModel, SLOT(clear()));
    connect(m_submission, SIGNAL(passwordRequested(const QString &, const QString &)), this, SIGNAL(passwordRequested(const QString &, const QString &)));
    connect(m_submission, SIGNAL(gotPassword(const QString &)), this, SIGNAL(gotPassword(const QString &)));
    connect(m_submission, SIGNAL(progressMin(int)), this, SLOT(setProgressMin(int)));
    connect(m_submission, SIGNAL(progressMax(int)), this, SLOT(setProgressMax(int)));
    connect(m_submission, SIGNAL(progress(int)), this, SLOT(setProgress(int)));
    connect(m_submission, SIGNAL(updateStatusMessage(QString)), this, SLOT(setStatusMessage(QString)));
    connect(m_submission, SIGNAL(connectionStateChanged(Composer::Submission::SubmissionProgress)), this, SLOT(handleConnectionStateChanged(Composer::Submission::SubmissionProgress)));
    Q_ASSERT(m_submission);
    if (m_submission)
        return true;
    return false;
}

bool SubmissionManager::buildMessage(const QString &from, const QString &subject, const QString &mailText)
{
    QString errorMessage;
    Composer::RecipientList recipientList = m_recipientListModel->toRecipientList();
    if (recipientList.isEmpty()) {
        errorMessage = tr("No recipients given, please provide a recipient for this message");
        emit messageBuildFailed(errorMessage);
        return false;
    }

    m_submission->composer()->setRecipients(recipientList);

    Imap::Message::MailAddress fromAddress;
    if (!Imap::Message::MailAddress::fromPrettyString(fromAddress, from)) {
        // TRANSLATORS: %1 is an email address
        errorMessage = QString("%1 is not a vaild mailaddress format. Please check your Sender Identity configuration in settings").arg(from);
        emit messageBuildFailed(errorMessage);
        return false;
    }
    if (subject.isEmpty()) {
        emit messageBuildFailed("Subject field is empty, please provide a subject for this message");
        return false;
    }
    if(!m_inReplyTo.isEmpty()) {
        QList<QByteArray> inReplyTo;
        inReplyTo.append(m_inReplyTo);
        m_submission->composer()->setInReplyTo(inReplyTo);
        m_submission->composer()->setReferences(m_references);
        m_submission->composer()->setReplyingToMessage(m_replyingToMessage);
    }
    m_submission->composer()->setFrom(fromAddress);

    m_submission->composer()->setTimestamp(QDateTime::currentDateTime());
    m_submission->composer()->setSubject(subject);
    m_submission->composer()->setText(mailText);

    for (int i = 0; i < m_attachmentListModel->rowCount(); i++) {
        m_submission->composer()->addAttachmentItem(m_attachmentListModel->getAttachmentAtIndex(i));
    }

    if (!m_submission->composer()->isReadyForSerialization()) {
        errorMessage = tr("Something went wrong and dekko cannot serialize this message.");
        emit messageBuildFailed(errorMessage);
        return false;
    }
    return true;
}

void SubmissionManager::setPassword(const QString &password)
{
    m_submission->setPassword(password);
}

void SubmissionManager::cancelPassword()
{
    m_submission->cancelPassword();
}

void SubmissionManager::send()
{
    if (m_submission == nullptr) {
        emit failed(tr("Unable to send message, please provide a proper configuration"));
        return;
    }
    Dekko::Settings::SmtpSettings *smtpSettings = qobject_cast<Dekko::Settings::SmtpSettings *>(m_account->smtpSettings());
    QVariantMap keys = m_mboxSettings.data()->keys();
    m_submission->setImapOptions(m_mboxSettings.data()->read(keys.value("saveToImap").toString()).toBool(),
                                 m_mboxSettings.data()->read(keys.value("sentMailbox").toString()).toString(),
                                 smtpSettings->server(), smtpSettings->username(),
                                 (Composer::Submission::Method)(smtpSettings->submissionMethod()) == Composer::Submission::Method::IMAP_SENDMAIL);

    m_submission->setSmtpOptions(smtpSettings->useBurl(), smtpSettings->username());
    m_submission->send();
}

void SubmissionManager::setReplyingToMessage(const QString &mailbox, const uint uid)
{
    Imap::Mailbox::Model *model = qobject_cast<Imap::Mailbox::Model*>(m_account->imapModel());
    Q_ASSERT(model);
    m_replyingToMessage = model->messageIndexByUid(mailbox, uid);
}

QString SubmissionManager::quoteText()
{
    QString quote = Imap::decodeByteArray(m_replyingToMessage.data(Imap::Mailbox::RolePartData).toByteArray(),
                          m_replyingToMessage.data(Imap::Mailbox::RolePartCharset).toString());
    return quote;
}

QVariantMap SubmissionManager::parseMailtoUrl(QString mailtoUrl)
{
    QString decodedSubject;
    QString decodedBody;
    QList<QPair<Composer::RecipientKind, QString>> decodedRecipients;
    QList<QByteArray> decodedInReplyTo;
    QList<QByteArray> decodedReferences;

    Composer::parseRFC6068Mailto(QUrl(mailtoUrl), decodedSubject, decodedBody, decodedRecipients, decodedInReplyTo, decodedReferences);
    for (int i = 0; i < decodedRecipients.size(); i++) {
        Dekko::Models::Recipient* r = new Dekko::Models::Recipient(this);
        r->setRecipientAddress(decodedRecipients[i].second);
        r->setRecipientKind((Dekko::Models::Recipient::RecipientKind)decodedRecipients[i].first);
        m_recipientListModel->appendRow(r);
    };
    if (decodedInReplyTo.size() > 0)
        setInReplyTo(decodedInReplyTo[0]);
    setReferences(decodedReferences);

    QVariantMap map;
    map["subject"] = decodedSubject;
    map["body"] = decodedBody;
    return map;
}

int SubmissionManager::progressMin() const
{
    return m_progressMin;
}

void SubmissionManager::setProgressMin(const int &val)
{
    if (val == m_progressMin) {
        return;
    }
    m_progressMin = val;
    progressChanged();
}

int SubmissionManager::progressMax() const
{
    return m_progressMax;
}

void SubmissionManager::setProgressMax(const int &val)
{
    if (val == m_progressMax) {
        return;
    }
    m_progressMax = val;
    progressChanged();
}

int SubmissionManager::progress() const
{
    return m_progress;
}

bool SubmissionManager::progressIndeterminate() const
{
    return m_progressIndeterminate;
}

QString SubmissionManager::statusMessage() const
{
    return m_statusMessage;
}

QByteArray SubmissionManager::inReplyTo() const
{
    return m_inReplyTo;
}

void SubmissionManager::setAccount(QObject *account)
{
    m_account = qobject_cast<Dekko::Accounts::Account *>(account);
    emit accountChanged();
}

void SubmissionManager::setReferences(const QList<QByteArray> &refs)
{
    if (refs == m_references) {
        return;
    }
    m_references = refs;
}

void SubmissionManager::handleConnectionStateChanged(const Composer::Submission::SubmissionProgress &state)
{
    switch (state) {
    case Composer::Submission::STATE_INIT:
    case Composer::Submission::STATE_BUILDING_MESSAGE:
    case Composer::Submission::STATE_SAVING:
    case Composer::Submission::STATE_FAILED:
        m_progressIndeterminate = true;
        break;
    case Composer::Submission::STATE_PREPARING_URLAUTH:
    case Composer::Submission::STATE_SUBMITTING:
    case Composer::Submission::STATE_UPDATING_FLAGS:
    case Composer::Submission::STATE_SENT:
        m_progressIndeterminate = false;
        break;
    }
    emit progressIndeterminateChanged();
}

void SubmissionManager::setProgress(const int &val)
{
    if (val == m_progress) {
        return;
    }
    m_progress = val;
    progressChanged();
}

void SubmissionManager::setStatusMessage(const QString &status)
{
    if (status == m_statusMessage) {
        return;
    }
    m_statusMessage = status;
    progressChanged();
}

void SubmissionManager::setInReplyTo(const QByteArray &addresses)
{
    if (addresses == m_inReplyTo) {
        return;
    }
    m_inReplyTo = addresses;
    inReplyToChanged();
}

}
