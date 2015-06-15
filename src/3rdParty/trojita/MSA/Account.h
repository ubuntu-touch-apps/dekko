/* Copyright (C) 2014 Dan Chapman <dpniel@ubuntu.com>
   Copyright (C) 2006 - 2014 Jan Kundrát <jkt@flaska.net>

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
#ifndef MSA_ACCOUNT_H
#define MSA_ACCOUNT_H

#include <QObject>
#include <Plugins/PluginManager.h>
#include "UiUtils/PasswordWatcher.h"

class QSettings;

namespace MSA {

class Account : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UiUtils::PasswordWatcher *passwordWatcher READ passwordWatcher NOTIFY submissionMethodChanged)
    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(bool authenticateEnabled READ authenticateEnabled WRITE setAuthenticateEnabled NOTIFY authenticateEnabledChanged)
    Q_PROPERTY(QString pathToSendmail READ pathToSendmail WRITE setPathToSendmail NOTIFY pathToSendmailChanged)
    Q_PROPERTY(bool saveToImap READ saveToImap WRITE setSaveToImap NOTIFY saveToImapChanged)
    Q_PROPERTY(QString sentMailboxName READ sentMailboxName WRITE setSentMailboxName NOTIFY sentMailboxNameChanged)
    Q_PROPERTY(bool useBurl READ useBurl WRITE setUseBurl NOTIFY useBurlChanged)
    Q_PROPERTY(Method submissionMethod READ submissionMethod WRITE setSubmissionMethod NOTIFY submissionMethodChanged)
    Q_ENUMS(Method)

public:
    Account(QObject *parent, QSettings *settings, Plugins::PluginManager *pluginManager, const QString &accountName);

    enum class Method {
        SMTP, /**< Non-encrypted connection over network */
        SSMTP, /**< Encrypted network connection from the start */
        SMTP_STARTTLS, /**< Starts with a non-encrypted network connection. Which gets upgraded using STARTTLS */
        SENDMAIL, /**< Use a local Sendmail compatible transfer agent */
        IMAP_SENDMAIL, /**< Uses the IMAP SENDMAIL Extension */
    };

    UiUtils::PasswordWatcher *passwordWatcher() const;
    Method submissionMethod() const;
    QString server() const;
    int port() const;
    QString username() const;
    bool authenticateEnabled() const;
    QString pathToSendmail() const;
    bool saveToImap() const;
    QString sentMailboxName() const;
    bool useBurl() const;

    static quint16 defaultPort(const Method method);

    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void restoreSettings();

signals:
    void submissionMethodChanged();
    void serverChanged();
    void portChanged();
    void usernameChanged();
    void authenticateEnabledChanged();
    void pathToSendmailChanged();
    void saveToImapChanged();
    void sentMailboxNameChanged();
    void useBurlChanged();
    void showPortWarning(const QString &message);
    void settingsSaved();
    void settingsRestored();

public slots:
    void setSubmissionMethod(const Method method);
    void setServer(const QString &server);
    void setPort(const quint16 port);
    void setUsername(const QString &username);
    void setAuthenticateEnabled(const bool auth);
    void setPathToSendmail(const QString &pathToSendmail);
    void setSaveToImap(const bool selected);
    void setSentMailboxName(const QString &location);
    void setUseBurl(const bool selected);

private slots:
    void maybeShowPortWarning();

private:
    QSettings *m_settings;
    Plugins::PluginManager *m_pluginManager;
    UiUtils::PasswordWatcher *m_passwordWatcher;
    QString m_accountName;
    QString m_server;
    QString m_username;
    quint16 m_port;
    QString m_password;
    bool m_authenticateEnabled;
    QString m_pathToSendmail;
    bool m_saveToImap;
    QString m_sentMailboxName;
    bool m_useBurl;

    Method m_msaSubmissionMethod;

};

}

Q_DECLARE_METATYPE(MSA::Account::Method)

#endif // MSA_ACCOUNT_H
