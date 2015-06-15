/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef MAILBOXSETTINGS_H
#define MAILBOXSETTINGS_H

#include <QObject>
#include <QPointer>
#include "SettingsObjectBase.h"
#include "app/Accounts/Account.h"
#include "Imap/Model/MailboxModel.h"

namespace Dekko
{
namespace Accounts
{
class Account;
}
namespace Settings
{
// When a new account is created the default global object will look something like this (more will be added)
//{
//    "954d7048-1324-462e-a91f-7d9dc0b634b8": {
//        "global": {
//            "archive": {
//                "enabled": false,
//                "folder": "",
//                "format": "monthly",
//                "keepMboxStructure": false
//            },
//            "deleted": {
//                "autoExpunge": false,
//                "markRead": false,
//                "moveToTrash": false,
//                "preserveFlagged": false
//            },
//            "drafts": {
//                "saveDraftInterval": 1000,
//                "saveLocal": true,
//                "saveToDrafts": false
//            },
//            "draftsMailbox": "",
//            "sentMailbox": "[Gmail]/Sent Mail",
//            "submission": {
//                "allowRetries": false,
//                "bcc": [
//                ],
//                "cc": [
//                ],
//                "includeBCC": false,
//                "includeCC": false,
//                "saveToImap": true
//            },
//            "trashMailbox": ""
//        }
//    }
//}

class MailboxSettings : public SettingsObjectBase
{
    Q_OBJECT

    Q_PROPERTY(QObject *account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(QString mailboxName READ mailboxName WRITE setMailboxName NOTIFY mailboxNameChanged)
    Q_PROPERTY(QJsonObject deleted READ deleted NOTIFY objectDataChanged)
    Q_PROPERTY(QJsonObject submission READ submission NOTIFY objectDataChanged)
    Q_PROPERTY(QJsonObject drafts READ drafts NOTIFY objectDataChanged)
    Q_PROPERTY(QVariantMap keys READ keys CONSTANT)
    Q_PROPERTY(bool hasOverride MEMBER m_hasOverride NOTIFY objectDataChanged)

public:
    explicit MailboxSettings(QObject *parent = 0);

    static const QVariantMap keys();

    QObject *account();
    void setAccount(QObject *account);
    QString mailboxName() const;
    void setMailboxName(const QString &mboxName);
    QJsonObject submission() const;
    QJsonObject deleted() const;
    QJsonObject drafts() const;
    // We set this in main.cpp or early on then you will be able
    // to create multiple instances and not have to pass around SettingsFile's all over the place
    // and get the benefit of change notifications across them all.
    // (Now that's already an improvement on QSettings :-D )
    static SettingsFileBase *defaultFile();
    static void setDefaultFile(SettingsFileBase *file);

    Q_INVOKABLE void createOverride();
    Q_INVOKABLE void removeOverride();
    Q_INVOKABLE void createDefaults();
    Q_INVOKABLE void removeAccount();

signals:
    void accountChanged();
    void mailboxNameChanged();
    void objectDataChanged();

private:
    QPointer<Accounts::Account> m_account;
    QString m_mboxName;
    QString m_mboxPath;
    QString seperator() const;
    bool m_hasOverride;

    void determineGlobalOrOverride();
};


}
}
#endif // MAILBOXSETTINGS_H
