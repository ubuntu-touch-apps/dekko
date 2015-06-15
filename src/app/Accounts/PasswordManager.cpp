#include "PasswordManager.h"

#include <QSettings>
#include "app/Settings/SettingsNames.h"
#include "Plugins/PluginManager.h"


namespace Dekko
{
namespace Accounts
{

PasswordManager::PasswordManager(QObject *parent) :
    QObject(parent), m_manager(0), m_account(0), m_pluginManager(0), m_type(PasswordType::IMAP),
    m_jobsInProgress(0)
{
    QSettings s;
    m_pluginManager = new Plugins::PluginManager(0, &s, Dekko::SettingsNames::addressbookPlugin,
                                                 Dekko::SettingsNames::passwordPlugin);
}

QObject *PasswordManager::accountsManager()
{
    return m_manager;
}

void PasswordManager::setAccountsManager(QObject *manager)
{
    m_manager = qobject_cast<AccountsManager *>(manager);
    emit accountsManagerChanged();
}

QObject *PasswordManager::account() const
{
    return m_account;
}

void PasswordManager::setAccount(QObject *account)
{
    if (m_jobsInProgress) {
        emit error(QString("Cannot set an account while a password job is in progress. Either wait for it to \
                      finish or create an accountManager so you can do multiple requests."
                      ));
        return;
    }
    delete m_account;
    m_account = 0;
    m_account = qobject_cast<Account *>(account);
    Q_ASSERT(m_account);
    if (m_account->accountId() != m_accountId) {
        m_accountId = m_account->accountId();
    }
    emit accountChanged();
}

QString PasswordManager::accountId() const
{
    return m_accountId;
}

void PasswordManager::setAccountId(const QString &accountId)
{
    if (m_jobsInProgress) {
        emit error(QString("Cannot set an accountId while a password job is in progress. Either wait for it to \
                      finish or create an accountManager so you can do multiple requests."
                      ));
        return;
    }
    if (accountId == m_accountId) {
        return;
    }
    m_accountId = accountId;
    Account *newAccount = new Account(this);
    newAccount->setAccountId(m_accountId);
    setAccount(newAccount);
}

PasswordManager::PasswordType PasswordManager::type() const
{
    return m_type;
}

void PasswordManager::setType(const PasswordManager::PasswordType type)
{
    if (m_jobsInProgress) {
        emit error(QString("Cannot set a PasswordType while a password job is in progress. Either wait for it to \
                      finish or create an accountManager so you can do multiple requests."
                      ));
        return;
    }
    m_type = type;
    emit typeChanged();
}

int PasswordManager::jobsInProgress() const
{
    return m_jobsInProgress;
}

void PasswordManager::requestPassword()
{
    if (m_manager) {
        emit error(QString("Cannot call requestPassword() when accountsManager is set. Use requestPassword(accountId, type) instead."));
        return;
    }
    if (m_type != PasswordType::INVALID) {
        requestPasswordJob(m_accountId, m_type);
    } else {
        emit error(QString("Invalid configuration"));
    }
}

void PasswordManager::requestPassword(const QString &accountId, const PasswordManager::PasswordType type)
{
    if (!m_manager) {
        emit error(QString("Please set an accountsManager before calling this"));
        return;
    }
    if (type != PasswordType::INVALID) {
        requestPasswordJob(accountId, type);
    } else {
        emit error(QString("Cannot request a password of type INVALID"));
    }
}

void PasswordManager::setPassword(const QString &password)
{
    if (m_manager) {
        emit error(QString("Cannot call setPassword() when accountsManager is set. Use setPassword(accountId, type, password) instead."));
        return;
    }
    if (m_type != PasswordType::INVALID) {
        setPasswordJob(m_accountId, m_type, password);
    } else {
        emit error(QString("Invalid configuration"));
    }
}

void PasswordManager::setPassword(const QString &accountId, const PasswordManager::PasswordType type, const QString &password)
{
    if (!m_manager) {
        emit error(QString("Please set an accountsManager before calling this"));
        return;
    }
    if (!accountId.isEmpty() && type != PasswordType::INVALID) {
        setPasswordJob(accountId, type, password);
    } else {
        emit error(QString("Cannot request a password of type INVALID"));
    }
}

// TODO: refactor this!!!
void PasswordManager::requestPasswordJob(const QString &accountId, const PasswordManager::PasswordType type)
{
    if (m_manager) {
        Q_FOREACH(Account *account, m_manager->m_accountsList) {
            if (accountId == account->accountId()) {
                switch (type) {
                case PasswordType::IMAP:
                    if (static_cast<Dekko::Settings::ImapSettings *>(account->imapSettings())->authenticationMethod() ==
                            Dekko::Settings::ImapSettings::AuthType::OAUTH) {
                        QMap<QString, QVariant> authObj;
                        authObj.insert(QString("accountId"), accountId);
                        // TODO: setup uoaAccountId in ImapSettings
                        authObj.insert(QString("uoaAccountId"), static_cast<Dekko::Settings::UOASettings *>(
                                           account->uoaSettings())->uoaAccountId());
                        authObj.insert(QString("OAuth"), true);
                        emit accountUsesOnlineAccounts(authObj);
                        return;

                    }
                    break;
                case PasswordType::SMTP:
                    if (static_cast<Dekko::Settings::SmtpSettings *>(account->smtpSettings())->authenticationMethod() ==
                            Dekko::Settings::SmtpSettings::AuthType::OAUTH) {
                        QMap<QString, QVariant> authObj;
                        authObj.insert(QString("accountId"), accountId);
                        // TODO: setup uoaAccountId in SmtpSettings
                        authObj.insert(QString("uoaAccountId"), static_cast<Dekko::Settings::UOASettings *>(
                                           account->uoaSettings())->uoaAccountId());
                        authObj.insert(QString("OAuth"), true);
                        emit accountUsesOnlineAccounts(authObj);
                        return;
                    }
                    break;
                case PasswordType::INVALID:
                    return;
                }

            }
        }
    } else if (m_account) {
        if (accountId == m_account->accountId()) {
            switch (type) {
            case PasswordType::IMAP:
                if (static_cast<Dekko::Settings::ImapSettings *>(m_account->imapSettings())->authenticationMethod() ==
                        Dekko::Settings::ImapSettings::AuthType::OAUTH) {
                    QMap<QString, QVariant> authObj;
                    authObj.insert(QString("accountId"), accountId);
                    // TODO: setup uoaAccountId in ImapSettings
                    authObj.insert(QString("uoaAccountId"), static_cast<Dekko::Settings::UOASettings *>(
                                       m_account->uoaSettings())->uoaAccountId());
                    authObj.insert(QString("OAuth"), true);
                    emit accountUsesOnlineAccounts(authObj);
                    return;

                }
                break;
            case PasswordType::SMTP:
                if (static_cast<Dekko::Settings::SmtpSettings *>(m_account->smtpSettings())->authenticationMethod() ==
                        Dekko::Settings::SmtpSettings::AuthType::OAUTH) {
                    QMap<QString, QVariant> authObj;
                    authObj.insert(QString("accountId"), accountId);
                    // TODO: setup uoaAccountId in SmtpSettings
                    authObj.insert(QString("uoaAccountId"), static_cast<Dekko::Settings::UOASettings *>(
                                       m_account->uoaSettings())->uoaAccountId());
                    authObj.insert(QString("OAuth"), true);
                    emit accountUsesOnlineAccounts(authObj);
                    return;
                }
                break;
            case PasswordType::INVALID:
                return;
            }
        } else {
            emit error(QString("Hmmm the Account and accountId don't match. You only need to set one of them! :-D"));
            return;
        }
    }
    // If we reached here then we are fetching from local storage
    // so lets start a new job
    startNewJob(JobType::READ, accountId, type);
}

void PasswordManager::setPasswordJob(const QString &accountId, const PasswordManager::PasswordType type, const QString &password)
{
    bool isOnlineAccount = false;
    if (m_manager) {
        Q_FOREACH(Account *account, m_manager->m_accountsList) {
            if (accountId == account->accountId()) {
                switch (type) {
                case PasswordType::IMAP:
                    if (static_cast<Dekko::Settings::ImapSettings *>(account->imapSettings())->authenticationMethod() ==
                            Dekko::Settings::ImapSettings::AuthType::OAUTH) {
                        isOnlineAccount = true;

                    }
                    break;
                case PasswordType::SMTP:
                    if (static_cast<Dekko::Settings::SmtpSettings *>(account->smtpSettings())->authenticationMethod() ==
                            Dekko::Settings::SmtpSettings::AuthType::OAUTH) {
                        isOnlineAccount = true;
                    }
                    break;
                case PasswordType::INVALID:
                    return;
                }
            }
        }
    } else if (m_account) {
        if (accountId == m_account->accountId()) {
            switch (type) {
            case PasswordType::IMAP:
                if (static_cast<Dekko::Settings::ImapSettings *>(m_account->imapSettings())->authenticationMethod() ==
                        Dekko::Settings::ImapSettings::AuthType::OAUTH) {
                    isOnlineAccount = true;
                }
                break;
            case PasswordType::SMTP:
                if (static_cast<Dekko::Settings::SmtpSettings *>(m_account->smtpSettings())->authenticationMethod() ==
                        Dekko::Settings::SmtpSettings::AuthType::OAUTH) {
                    isOnlineAccount = true;
                }
                break;
            case PasswordType::INVALID:
                return;
            }
        } else {
            emit error(QString("Hmmm the Account and accountId don't match. You only need to set one of them! :-D"));
            return;
        }
    }
    if (isOnlineAccount) {
        emit setPasswordInOnlineAccounts();
        return;
    }
    // If we reached here then we are fetching from local storage
    // so lets start a new job
    startNewJob(JobType::WRITE, accountId, type, password);
}

void PasswordManager::startNewJob(const JobType &jobType, const QString &accountId, const PasswordManager::PasswordType type, const QString &password)
{
    ++m_jobsInProgress;
    switch (jobType) {
    case JobType::READ:
        if (Plugins::PasswordPlugin *plugin = m_pluginManager->password()) {
            Plugins::PasswordJob *pwdJob = 0;
            switch (type) {
            case PasswordType::IMAP:
                pwdJob = plugin->requestPassword(accountId, QString("imap"));
                break;
            case PasswordType::SMTP:
                pwdJob = plugin->requestPassword(accountId, QString("smtp"));
                break;
            case PasswordType::INVALID:
                return;
            }
            connect(pwdJob, SIGNAL(passwordAvailable(QString,QString,QString)), this, SLOT(showPasswordAvailable(QString,QString,QString)));
            connect(pwdJob, SIGNAL(error(Plugins::PasswordJob::Error,QString)), this,
                    SLOT(passwordReadingFailed(Plugins::PasswordJob::Error,QString)));
            pwdJob->setAutoDelete(true);
            pwdJob->start();
        } else {
            emit error(QString("No password plugin available... that's not good!! :-/"));
        }
        break;
    case JobType::WRITE:
        if (Plugins::PasswordPlugin *plugin = m_pluginManager->password()) {
            Plugins::PasswordJob *pwdJob = 0;
            switch (type) {
            case PasswordType::IMAP:
                pwdJob = plugin->storePassword(accountId, QString("imap"), password);
                break;
            case PasswordType::SMTP:
                pwdJob = plugin->storePassword(accountId, QString("smtp"), password);
                break;
            case PasswordType::INVALID:
                return;
            }
            connect(pwdJob, SIGNAL(passwordStored(QString)), this, SLOT(passwordWritten(QString)));
            connect(pwdJob, SIGNAL(error(Plugins::PasswordJob::Error,QString)), this,
                    SLOT(passwordWritingFailed(Plugins::PasswordJob::Error,QString)));
            pwdJob->setAutoDelete(true);
            pwdJob->start();
        } else {
            emit error(QString("No password plugin available... that's not good!! :-/"));
        }
        break;
    case JobType::DELETE: // Not implemented yet, and it's get's nuked with the account anyway
        break;
    }
    emit jobCountChanged();
}

void PasswordManager::showPasswordAvailable(const QString &accountId, const QString &accountType, const QString &password)
{
    QMap<QString, QVariant> passwordObj;
    passwordObj.insert(QString("accountId"), accountId);
    passwordObj.insert(QString("type"), accountType);
    passwordObj.insert(QString("password"), password);
    emit passwordNowAvailable(passwordObj);
    --m_jobsInProgress;
    emit jobCountChanged();
}

void PasswordManager::passwordReadingFailed(const Plugins::PasswordJob::Error errorCode, const QString &errorMessage)
{
    --m_jobsInProgress;
    switch (errorCode) {
    case Plugins::PasswordJob::NoSuchPassword:
        emit error(QString("No password saved for this account"));
        break;
    case Plugins::PasswordJob::Stopped:
    case Plugins::PasswordJob::UnknownError:
    {
        QString msg = QString("Password reading failed: %1").arg(errorMessage);
        emit readingFailed(msg);
        break;
    }
    }
    emit jobCountChanged();
}

void PasswordManager::passwordWritten(const QString &accountId)
{
    --m_jobsInProgress;
    emit passwordStored(accountId);
}

void PasswordManager::passwordWritingFailed(const Plugins::PasswordJob::Error errorCode, const QString &errorMessage)
{
    --m_jobsInProgress;
    switch (errorCode) {
    case Plugins::PasswordJob::NoSuchPassword:
        Q_ASSERT(false);
        break;
    case Plugins::PasswordJob::Stopped:
    case Plugins::PasswordJob::UnknownError:
    {
        QString msg = QString("Password writing failed: %1").arg(errorMessage);
        emit writingFailed(msg);
        break;
    }
    }
    emit jobCountChanged();
}

}

}
