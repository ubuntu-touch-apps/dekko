#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QObject>
#include "app/Accounts/AccountsManager.h"
#include "app/Accounts/Account.h"
#include "app/Settings/Settings.h"
#include "plugins/SimpleCryptPassword/SimpleCrypt.h"
#include "Plugins/PluginManager.h"

namespace Dekko
{
namespace Accounts
{
class AccountsManager;

/*!
 \brief

 \class PasswordManager PasswordManager.h "src/app/Accounts/PasswordManager.h"
*/
class PasswordManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *accountsManager READ accountsManager WRITE setAccountsManager NOTIFY accountsManagerChanged)
    Q_PROPERTY(QObject *account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(PasswordType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int jobsInProgress READ jobsInProgress NOTIFY jobCountChanged)
    Q_ENUMS(PasswordType)

public:
    /*!
     \brief

     \fn PasswordManager
     \param parent
    */
    explicit PasswordManager(QObject *parent = 0);

    /*!
     \brief

     \enum PasswordType
    */
    enum PasswordType {
        IMAP,
        SMTP,
        INVALID
    };

    /*!
     \brief

     \enum JobType
    */
    enum JobType {
        READ,
        WRITE,
        DELETE
    };

    /*!
     \brief

     \fn accountsManager
     \return QObject
    */
    QObject *accountsManager();
    /*!
     \brief

     \fn setAccountsManager
     \param manager
    */
    void setAccountsManager(QObject *manager);

    /*!
     \brief

     \fn account
     \return QObject
    */
    QObject *account() const;
    /*!
     \brief

     \fn setAccount
     \param account
    */
    void setAccount(QObject *account);

    /*!
     \brief

     \fn accountId
     \return QString
    */
    QString accountId() const;
    /*!
     \brief

     \fn setAccountId
     \param accountId
    */
    void setAccountId(const QString &accountId);

    /*!
     \brief

     \fn type
     \return PasswordType
    */
    PasswordType type() const;
    /*!
     \brief

     \fn setType
     \param type
    */
    void setType(const PasswordType type);

    /*!
     \brief

     \fn jobsInProgress
     \return int
    */
    int jobsInProgress() const;


signals:
    // Emits a variant containing
    // - AccountId
    // - PasswordType - QString either "imap" or "smtp"
    // - Password
    /*!
     \brief

     \fn passwordNowAvailable
     \param passwordObject
    */
    void passwordNowAvailable(QVariant passwordObject);
    // We will emit our stored UOA accountid here so the we can use Manager.loadAccount() to create an
    // Account instance in qml to then authenticate and get password/token.
    // I would much prefer to do that here though :-(
    // Emits a variant containing
    // - AccountId <-- Our one
    // - UOAAccountId
    // - OAuth - bool to determine if UOA is going to return a pwd or token for UOAAccountId
    // We will need to keep a hold of this object in qml while we wait for
    // the OA to authenticate so we can set the result on the correct account.
    /*!
     \brief

     \fn accountUsesOnlineAccounts
     \param accountObject
    */
    void accountUsesOnlineAccounts(QVariant accountObject);
    // Use as a hook to open online accounts.
    /*!
     \brief

     \fn setPasswordInOnlineAccounts
    */
    void setPasswordInOnlineAccounts();
    /*!
     \brief

     \fn accountsManagerChanged
    */
    void accountsManagerChanged();
    /*!
     \brief

     \fn accountChanged
    */
    void accountChanged();
    /*!
     \brief

     \fn accountIdChanged
    */
    void accountIdChanged();
    /*!
     \brief

     \fn typeChanged
    */
    void typeChanged();
    /*!
     \brief

     \fn error
     \param msg
    */
    void error(const QString msg);
    /*!
     \brief

     \fn readingFailed
     \param message
    */
    void readingFailed(const QString &message);
    /*!
     \brief

     \fn writingFailed
     \param message
    */
    void writingFailed(const QString &message);
    /*!
     \brief

     \fn jobCountChanged
    */
    void jobCountChanged();
    /*!
     \brief

     \fn passwordStored
     \param id
    */
    void passwordStored(const QString &id);

public slots:
    // Can be used for a single request when either Account or AccountId and PasswordType have been set
    // Trying to do multiple requests here will return null while any previous job is still in progress.
    // So only use when working on a single account.
    /*!
     \brief

     \fn requestPassword
    */
    void requestPassword();
    // Can be used when an accountsManager has been set. Use this method for doing multiple async requests
    // The accountsManager takes precedence over setting Account & AccountId so they will be ignored if set.
    /*!
     \brief

     \fn requestPassword
     \param accountId
     \param type
    */
    void requestPassword(const QString &accountId, const PasswordType type);

    /*!
     \brief

     \fn setPassword
     \param password
    */
    void setPassword(const QString &password);
    /*!
     \brief

     \fn setPassword
     \param accountId
     \param type
     \param password
    */
    void setPassword(const QString &accountId, const PasswordType type, const QString &password);

private slots:
    /*!
     \brief

     \fn requestPasswordJob
     \param accountId
     \param type
    */
    void requestPasswordJob(const QString &accountId, const PasswordType type);
    /*!
     \brief

     \fn setPasswordJob
     \param accountId
     \param type
     \param password
    */
    void setPasswordJob(const QString &accountId, const PasswordType type, const QString &password);
    /*!
     \brief

     \fn startNewJob
     \param jobType
     \param accountId
     \param type
     \param password
    */
    void startNewJob(const JobType &jobType, const QString &accountId, const PasswordType type, const QString &password = QString());
    /*!
     \brief

     \fn showPasswordAvailable
     \param accountId
     \param accountType
     \param password
    */
    void showPasswordAvailable(const QString &accountId, const QString &accountType, const QString &password);
    /*!
     \brief

     \fn passwordReadingFailed
     \param errorCode
     \param errorMessage
    */
    void passwordReadingFailed(const Plugins::PasswordJob::Error errorCode, const QString &errorMessage);
    /*!
     \brief

     \fn passwordWritten
     \param accountId
    */
    void passwordWritten(const QString &accountId);
    /*!
     \brief

     \fn passwordWritingFailed
     \param errorCode
     \param errorMessage
    */
    void passwordWritingFailed(const Plugins::PasswordJob::Error errorCode, const QString &errorMessage);

private:
    Dekko::Accounts::AccountsManager *m_manager; /*!< TODO */
    Account *m_account; /*!< TODO */
    Plugins::PluginManager *m_pluginManager; /*!< TODO */
    PasswordType m_type; /*!< TODO */
    QString m_accountId; /*!< TODO */
    int m_jobsInProgress; /*!< TODO */

};
}
}

#endif // PASSWORDMANAGER_H
