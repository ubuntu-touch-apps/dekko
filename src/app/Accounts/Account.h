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
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QDebug>
#include <QSettings>
#include <QSslError>
#include "Common/ConnectionMethod.h"
#include "Common/Logging.h"
#include "Composer/Submission.h"
#include "Imap/Model/Model.h"
#include "Imap/Model/MailboxModel.h"
#include "Imap/Model/MsgListModel.h"
#include "Imap/Model/SubtreeModel.h"
#include "Imap/Model/ModelWatcher.h"
#include "Imap/Model/ThreadingMsgListModel.h"
#include "Imap/Model/VisibleTasksModel.h"
#include "UiUtils/PasswordWatcher.h"
#include "Imap/Model/DummyNetworkWatcher.h"
#include "Imap/Model/NetworkWatcher.h"
#include "UiUtils/Formatting.h"
#include "Plugins/PluginManager.h"
#include "SenderIdentityModel.h"
#include "app/Settings/Settings.h"
#include "AccountUtils.h"
namespace Imap {
namespace Mailbox {
class Model;
class MailboxModel;
class MsgListModel;
class ThreadingMsgListModel;
class SubtreeModelOfMailboxModel;
}
}

/*! \short Dekko namespace */
namespace Dekko
{
/*! Accounts namespace contains all classes related to managing user accounts */
namespace Accounts
{
/*!
 \brief The Account class represents a complete user account.

This will be the main interface for accessing account settings and connecting to the accounts imap server.

 \class Account Account.h "src/app/Accounts/Account.h"

 \note The accountId property needs to be set on an instance of this class for it to be useful.
   An accountId can be obtained by selecting a configured account from the AccountsListModel.

   If creating a new account the newAccount property needs to be set to true, which will generate an accountId
   for you.
*/
class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *imapModel READ imapModel NOTIFY modelsChanged)
    Q_PROPERTY(QObject *mailboxModel READ mailboxModel NOTIFY modelsChanged)
    Q_PROPERTY(QObject *msgListModel READ msgListModel NOTIFY modelsChanged)
    Q_PROPERTY(QObject *threadingModel READ threadingModel NOTIFY modelsChanged)
    Q_PROPERTY(QSettings *settings READ settings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(QObject *profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(QObject *imapSettings READ imapSettings WRITE setImapSettings NOTIFY imapSettingsChanged)
    Q_PROPERTY(QObject *smtpSettings READ smtpSettings WRITE setSmtpSettings NOTIFY smtpSettingsChanged)
    Q_PROPERTY(QObject *uoaSettings READ uoaSettings WRITE setUoaSettings NOTIFY uoaSettingsChanged)
    Q_PROPERTY(QObject *network READ network NOTIFY modelsChanged)
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(bool newAccount READ newAccount WRITE setNewAccount NOTIFY accountIdChanged)
    Q_PROPERTY(bool hasVisibleTasks READ hasVisibleTasks NOTIFY visibleTasksChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY modelsChanged)
    Q_PROPERTY(bool isGmail READ isGmail CONSTANT)

    // PREFERENCES
    Q_PROPERTY(bool passwordMigrated READ passwordMigrated WRITE setPasswordMigrated NOTIFY passwordMigrationChanged)
public:
    /*!
     \brief Account Constructor

     \fn Account
     \param parent
    */
    explicit Account(QObject *parent = 0);

    /*!
     \brief Account Destructor

     \fn ~Account
    */
    ~Account(){ this->deleteLater(); }

    bool isOnline() const;

    /*!
     \brief A model representing a view of the entire imap server for this Account.

     This gives us a view of the whole imap server and is our main
     entry point for interacting with the server. This model can be passed
     as the sourceModel for the various proxy models we use to drive the UI.



     an example of using this model for displaying a message list. The account
     property has been set by selecting an account from one of the accounts models

     \verbatim
     Item {
        id: root
        property Account account

        MessageListModel {
            id: msgList
            sourceModel: account.imapModel
            sortOrder: Qt.DescendingOrder
        }

        UbuntuListView {
            id: msgListView
            anchors.fill: parent
            model: msgList
            delegate: Label { text: model.from }
        }
     }
     \endverbatim

     Or you can call any of the slots or functions in Imap::Mailbox::Model. So an example
     of simply reloading the mailbox list

     \verbatim
     Item {
        id: root

        property Account account

        MailboxListModel {
            ...
            Component.onCompleted: account.imapModel.reloadMailboxList()
        }
     }
     \endverbatim

     \fn imapModel
     \return QObject - pointer to Imap::Mailbox::Model instance.

    */
    QObject *imapModel() const;
    QObject *mailboxModel() const;
    QObject *msgListModel() const;
    QObject *threadingModel() const;
    /*!
     \brief The settings file currently being used

     \fn settings
     \return QSettings
    */
    QSettings *settings();

    /*!
     \brief Set a new settings file to be used when accessing account settings.

     \fn setSettings
     \param QSettings settings - settings file to be used.
    */
    void setSettings(QSettings *settings);

    /*!
     \brief Access to the users profile for this account

     \fn profile
     \return QObject - ptr to the Dekko::Settings::AccountProfile instance for accountId()
    */
    QObject *profile() const;

    /*!
     \brief Overwrite current account profle

     \fn setProfile
     \param profile - a Dekko::Settings::AccountProfile instance configured with same accountId as this Account
    */
    void setProfile(QObject *profile);

    /*!
     \brief Access to the users imap settings for this account

     \fn imapSettings
     \return QObject - ptr to the Dekko::Settings::ImapSettings instance for accountId()
    */
    QObject *imapSettings() const;

    /*! Overwrite current imap settings

      \param Dekko::Settings::ImapSettings - Instance configured with same accountId as this Account instance

      \verbatim embed:rst
        ..note::
            There is no need to call save on the new Dekko::Settings::ImapSettings instance. Save get's called on in it
            when passed here
      \endverbatim
    */
    /*!
     \brief

     \fn setImapSettings
     \param imapSettings
    */
    void setImapSettings(QObject *imapSettings);

    /*!
     \brief Access to the users Smtp Settings for this account

     \fn smtpSettings
     \return QObject - ptr to the Dekko::Settings::SmtpSettings instance for accountId()
    */
    QObject *smtpSettings() const;

    /*! Overwrite current smtp settings

      \param Dekko::Settings::SmtpSettings - Instance configured with same accountId as this Account instance

      \verbatim embed:rst
        ..note::
            There is no need to call save on the new Dekko::Settings::SmtpSettings instance. Save get's called on in it
            when passed here
      \endverbatim
    */
    /*!
     \brief

     \fn setSmtpSettings
     \param smtpSettings
    */
    void setSmtpSettings(QObject *smtpSettings);

    /*! Overwrite current offline settings

      \param Dekko::Settings::OfflineSettings - Instance configured with same accountId as this Account instance

      \verbatim embed:rst
        ..note::
            There is no need to call save on the new Dekko::Settings::OfflineSettings instance. Save get's called on in it
            when passed here
      \endverbatim
    */
    /*!
     \brief

     \fn setOfflineSettings
     \param offlineSettings
    */
    void setOfflineSettings(QObject *offlineSettings);

    QObject *uoaSettings() const;
    void setUoaSettings(QObject *uoaSettings);

    QObject *network() const;

    /*! \returns QString accountId - The currently set accountId */
    /*!
     \brief Current accountId

     \fn accountId
     \return QString
    */
    QString accountId() const;

    /*!
     \brief set a new accountId

     \fn setAccountId
     \param id - Once set this will reload all settings for this account
    */
    void setAccountId(const QString &id);

    /*!
     \brief

     \fn newAccount
     \return bool
    */
    bool newAccount() const;
    /*!
     \brief

     \fn setNewAccount
     \param isNew
    */
    void setNewAccount(const bool isNew);

    /*!
     \brief Forget the currently known ssl certificate for this account

     \fn forgetSslCertificate
    */
    Q_INVOKABLE void forgetSslCertificate() const;

    /*!
     \brief

     \fn passwordMigrated
     \return bool
    */
    bool passwordMigrated() const;
    /*!
     \brief

     \fn setPasswordMigrated
     \param migrated
    */
    void setPasswordMigrated(const bool migrated);

    /*!
     \brief Generates a new account Id

     There shouldn't be any need to call this. If this is a newAccount then it will call this for you

     \fn generateNewAccountId
     \return QString
    */
    QString generateNewAccountId() const;

    bool hasVisibleTasks() const;

    /*!
     \brief Return a human readable representation of the ssl info

     \fn sslInfo
     \return QVariant - map containing "accountId", "title" and "message"
    */
    Q_INVOKABLE QVariant sslInfo() const;
    bool isGmail() const;

    void setCacheDir(const QString &cacheDir);

    Q_INVOKABLE QStringList unfilteredCapabilities() const;

    Q_INVOKABLE void appendExtensionToImapBlacklist(const QString &extension);
    Q_INVOKABLE void removeExtensionFromImapBlackList(const QString &extension);
    Q_INVOKABLE bool isExtensionBlackListed(const QString &extension);

signals:
    /*!
     \brief Emitted when an error occurs

     \fn error
     \param error - message for error
    */
    void error(QString error);
    /*!
     \brief emitted when an accounts settigns were successfully deleted

     \fn accountSettingsDeleted
    */
    void accountSettingsDeleted();
    /*!
     \brief emitted when the Dekko::Settings::AccountProfile changed for this account

     \fn profileChanged
    */
    void profileChanged();
    /*!
     \brief emitted when the Dekko::Settings::ImapSettings changed for this account

     \fn imapSettingsChanged
    */
    void imapSettingsChanged();
    /*!
     \brief emitted when the Imap::Mailbox::Model changed for this account

     \fn modelsChanged
    */
    void modelsChanged();
    /*!
     \brief emitted when the Dekko::Settings::SmtpSettings changed for this account

     \fn smtpSettingsChanged
    */
    void smtpSettingsChanged();

    void uoaSettingsChanged();
    /*!
     \brief emitted when the QSettings file changed for this account

     \fn settingsChanged
    */
    void settingsChanged();
    /*!
     \brief emitted when the accountId changed for this account

     \fn accountIdChanged
    */
    void accountIdChanged();
    /*!
     \brief

     \fn passwordMigrationChanged
    */
    void passwordMigrationChanged();
    /*!
     \brief emitted when a decision is needed on the current ssl policy

     \fn checkSslPolicy
    */
    void checkSslPolicy();
    /*!
     \brief Save everything

     \fn saveAllSettings
    */
    void saveAllSettings();

    void visibleTasksChanged();
    void unfilteredCapabilitiesChanged();

public slots:
    /*!
     \brief Opens the IMAP connection initializing the imapModel instance

     \fn openImapConnection
    */
    void openImapConnection();
    /*!
     \brief Kill the currently open imap connection (This just kills everything. openImapConnection needs calling again after)

     \fn killImapConnection
    */
    void killImapConnection();
    /*!
     \brief Resets the current imap connection.

     It first kills it before reloading all settings then opening a new connection if a connection was previously active

     \fn resetAll
    */
    void resetAll();
    /*!
     \brief invalidate and force reload of all account settings

     \fn invalidateSettings
    */
    void invalidateSettings();
    /*!
     \brief delete all settings related to accountId

     \fn deleteAccountSettings
    */
    void deleteAccountSettings(); // delete account settings
    /*!
     \brief Nuke the cache for accountId

     \fn nukeCurrentCache
    */
    void nukeCurrentCache();
    /*!
     \brief Whether to accept the current ssl policy

     Usuall called after a decision from checkSslPolicy

     \fn setSslPolicy
     \param value
    */
    void setSslPolicy(bool value);
    /*!
     \brief Change the network sessions online status

     \fn setNetworkOnline
     \param online
    */
    void setNetworkOnline(const bool online);

    /*!
     \brief Save just this Account object and not attached Dekko::Settings instances

     \fn save
    */
    void save();


private slots:
    /*!
     \brief Reload it all baby""

     \fn reloadSettings
    */
    void reloadSettings();
    /*!
     \brief

     \fn onCacheError
     \param msg
    */
    void onCacheError(QString msg);
    /*!
     \brief

     \fn imapSomethingGotLogged
     \param parserId
     \param message
    */
    void imapSomethingGotLogged(uint parserId, const Common::LogMessage &message);
    /*! do *something* with signals from the imap model */
    /*!
     \brief

     \fn imapAlertReceived
     \param message
    */
    void imapAlertReceived(const QString &message);
    /*!
     \brief

     \fn imapErrorReceived
     \param message
    */
    void imapErrorReceived(const QString &message);
    /*!
     \brief

     \fn imapNetworkError
     \param message
    */
    void imapNetworkError(const QString &message);
    /*!
     \brief

     \fn imapSslError
     \param sslCertificateChain
     \param sslErrors
    */
    void imapSslError(const QList<QSslCertificate> &sslCertificateChain, const QList<QSslError> &sslErrors);

    void onRequireStartTlsInFuture();

private:
    Imap::Mailbox::Model *m_imapModel; /*!< Our imapModel pointer */
    Imap::Mailbox::MailboxModel *m_mboxModel;
    Imap::Mailbox::SubtreeModelOfMailboxModel *m_mboxTreeModel;
    Imap::Mailbox::ModelWatcher *m_watcher;
    Imap::Mailbox::ThreadingMsgListModel *m_threadModel;
    Imap::Mailbox::MsgListModel *m_msgListModel;
    Imap::Mailbox::NetworkWatcher *m_netWatcher; /*!< Our network watcher pointer */
    Imap::Mailbox::VisibleTasksModel *m_visibleTasksModel;
    QSettings *m_settings; /*!< Our QSettings file pointer */
    Dekko::Settings::AccountProfile *m_profile; /*!< Our Dekko::Settings::AccountProfile pointer */
    Dekko::Settings::ImapSettings *m_imapSettings; /*!< Our Dekko::Settings::ImapSettings pointer  */
    //! Our smtp settings ptr
    Dekko::Settings::SmtpSettings *m_smtpSettings; /*!< Our Dekko::Settings::SmtpSettings pointer  */
    Dekko::Settings::UOASettings *m_uoaSettings;

    //! Our accountId
    QString m_accountId; /*!< Our current account id */

    bool m_newAccount; /*!< Is this a new account */
    QString m_cacheDir; /*!< Our cache location path */
    bool m_pwdMigrated; /*!< Our password migration state. (Need to keep for a couple more releases) */
    QList<QSslCertificate> m_sslChain; /*!< TODO */
    QList<QSslError> m_sslErrors; /*!< TODO */
    QString m_sslInfoTitle; /*!< TODO */
    QString m_sslInfoMessage; /*!< TODO */
    UiUtils::Formatting::IconType m_sslInfoIcon; /*!< TODO */
    bool m_startOffline;

    /*!
     \brief

     \fn getDefaultSignature
     \return QString
     \todo Move to sender identity
    */
    QString getDefaultSignature() const;

};
}
}
#endif // ACCOUNT_H
