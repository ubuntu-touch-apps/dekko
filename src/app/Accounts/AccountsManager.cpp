#include "AccountsManager.h"
#include "AccountUtils.h"
#include "3rdParty/trojita/Imap/Model/Model.h"
#include "3rdParty/trojita/Imap/Model/SystemNetworkWatcher.h"
#include "3rdParty/trojita/Imap/Model/DummyNetworkWatcher.h"
#include "Imap/Model/ItemRoles.h"
#include "Imap/Model/MailboxFinder.h"
#include "app/Settings/MailboxSettings.h"
#include <ubuntu/connectivity/networking-status.h>
#include "Common/InvokeMethod.h"
#include <QDebug>


namespace Dekko
{
namespace Accounts
{
class AccountUtils;

AccountsManager::AccountsManager(QObject *parent) :
    QAbstractListModel(parent), m_rootView(0), m_currentAccount(new Account), m_networkStatus(false),
    m_searchForMbox(new Utils::SearchForSpecialMailbox), m_openingFirstAccount(false)
{
    m_rootView = qQuickViewer->rootObject();
}

QHash<int, QByteArray> AccountsManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DescriptionRole] = "description";
    roles[OrganizationRole] = "organization";
    roles[DisplayNameRole] = "displayName";
    roles[DomainIconRole] = "domainIcon";
    return roles;
}

QVariant AccountsManager::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_accountsList.size()) {
        return QVariant();
    }
    Dekko::Accounts::Account *account = m_accountsList[index.row()];
    Q_ASSERT(account);
    switch (role) {
    case NameRole:
        return static_cast<Dekko::Settings::AccountProfile *>(account->profile())->name();
    case DescriptionRole:
        return static_cast<Dekko::Settings::AccountProfile *>(account->profile())->description();
    case OrganizationRole:
        return static_cast<Dekko::Settings::AccountProfile *>(account->profile())->organization();
    case DisplayNameRole:
        return static_cast<Dekko::Settings::AccountProfile *>(account->profile())->displayName();
    case DomainIconRole:
        return AccountUtils::getDomainIcon(static_cast<Dekko::Settings::ImapSettings *>(account->imapSettings())->username());
    }
    return QVariant();
}

int AccountsManager::rowCount(const QModelIndex &) const
{
    return m_accountsList.size();
}

int AccountsManager::count() const
{
    return m_accountsList.count();
}

QModelIndex AccountsManager::indexFromItem(Account *account) const
{
    if (account != NULL) {
        for (int i = 0; i < m_accountsList.size(); i++) {
            if (m_accountsList.at(i) == account) {
                return index(i);
            }
        }
    }
    return QModelIndex();
}

QModelIndex AccountsManager::indexFromAccountId(const QString &accountId)
{
    if (!accountId.isEmpty()) {
        for (int i = 0; i < m_accountsList.size(); i++) {
            if (static_cast<Account *>(m_accountsList.at(i))->accountId() == accountId) {
                return index(i);
            }
        }
    }
    return QModelIndex();
}

bool AccountsManager::removeRow(int row, const QModelIndex &index)
{
    bool isCurrentAccount = false;
    if (row >= 0 || row < m_accountsList.size()) {
        beginRemoveRows(index, row, row);
        QAbstractItemModel *mboxAim = m_mboxModels.takeAt(row);
        Q_ASSERT(mboxAim);
        mboxAim->deleteLater();
        QAbstractItemModel *msgAim = m_msgListModels.takeAt(row);
        Q_ASSERT(msgAim);
        msgAim->deleteLater();
        Account *account = m_accountsList.takeAt(row);
        isCurrentAccount = m_currentAccount == account;
        Q_ASSERT(account);
        account->deleteAccountSettings();
        delete account;
        account = 0;
        endRemoveRows();
        emit countChanged();
        if (isCurrentAccount) {
            if (m_accountsList.size()) {
                m_currentAccount = m_accountsList.takeAt(0);
            }else {
                m_currentAccount.clear();
                m_currentAccount = new Account();
            }
        }
        return true;
    }
    return false;
}

bool AccountsManager::removeRows(int row, int count, const QModelIndex &index)
{
    bool isCurrentAccount = false;
    if (row >= 0 && count > 0 && (row + count) <= m_accountsList.size()) {
        beginRemoveRows(index, row, row + count - 1);
        QAbstractItemModel *mboxAim = m_mboxModels.takeAt(row);
        Q_ASSERT(mboxAim);
        mboxAim->deleteLater();
        QAbstractItemModel *msgAim = m_msgListModels.takeAt(row);
        Q_ASSERT(msgAim);
        msgAim->deleteLater();
        Account *account = m_accountsList.takeAt(row);
        isCurrentAccount = m_currentAccount == account;
        Q_ASSERT(account);
        account->deleteAccountSettings();
        delete account;
        account = 0;
        endRemoveRows();
        emit countChanged();
        if (isCurrentAccount) {
            if (m_accountsList.size()) {
                m_currentAccount = m_accountsList.takeAt(0);
            }else {
                m_currentAccount.clear();
                m_currentAccount = new Account();
            }
        }
        return true;
    }
    return false;
}

QObject *AccountsManager::get(int index)
{
    // This is bad!!! there are no checks on the index validity;
    return m_accountsList.at(index);
}

QObject *AccountsManager::getFromId(QString accountId)
{
    Q_FOREACH(Account *account, m_accountsList) {
        if (account->accountId() == accountId) {
            return account;
        }
    }
    Q_ASSERT(false);
    return 0;
}

QObject *AccountsManager::rootView() const
{
    return m_rootView;
}

void AccountsManager::setRootView(QObject *view)
{
    m_rootView = view;
    emit rootViewChanged();
}

bool AccountsManager::networkStatus() const
{
    return m_networkStatus;
}

QList<QAbstractItemModel *> AccountsManager::mailboxModels() const
{
    return m_mboxModels;
}

QList<QAbstractItemModel *> AccountsManager::messageListModels() const
{
    return m_msgListModels;
}

QObject *AccountsManager::currentAccount()
{
    return m_currentAccount;
}

void AccountsManager::setCurrentAccount(QObject *account)
{
    if (m_currentAccount == qobject_cast<Account *>(account)) {
        return;
    }
    m_currentAccount = qobject_cast<Account *>(account);
    Q_ASSERT(m_currentAccount);
    emit currentAccountChanged();
}

QStringList AccountsManager::accountIds()
{
    QStringList accountIdList;
    for (int i = 0; i < m_accountsList.size(); i++) {
        accountIdList.append(m_accountsList[i]->accountId());
    }
    return accountIdList;
}

bool AccountsManager::openingFirstAccount() const
{
    return m_openingFirstAccount;
}

void AccountsManager::appendRow(Account *account)
{
    if (account != NULL) {
        appendRows(QList<Account *>() << account);
        emit countChanged();
    }
}

void AccountsManager::appendRows(QList<Account *> &accounts)
{
    if (accounts.size() == 0) {
        return;
    }
    beginInsertRows(QModelIndex() , rowCount(), rowCount() + accounts.size() - 1);
    Q_FOREACH(Account *account, accounts) {
        // Ensure any accounts we created in qml don't remain parented in that context.
        account->setParent(this);
        m_accountsList.append(account);
        connect(account->imapModel(),
                SIGNAL(imapError(QString)),
                this,
                SLOT(showImapError(QString)));
        connect(account->imapModel(),
                SIGNAL(networkError(QString)),
                this,
                SLOT(showNetworkError(QString)));
        connect(account->imapModel(),
                SIGNAL(alertReceived(QString)),
                this,
                SLOT(showAlertReceived(QString)));
        connect(account->imapModel(),
                SIGNAL(authRequested()),
                this,
                SLOT(showAuthRequested()));
        connect(account->imapModel(),
                SIGNAL(connectionStateChanged(QObject*, Imap::ConnectionState)),
                this,
                SLOT(connectionStateChanged(QObject*,Imap::ConnectionState)));
        connect(account->imapModel(),
                SIGNAL(authAttemptFailed(QString)),
                this,
                SLOT(showAuthAttemptFailed(QString)));
        connect(account,
                SIGNAL(checkSslPolicy()),
                this,
                SLOT(showCheckSslPolicy()));
        connect(account->imapModel(), SIGNAL(messageCountPossiblyChanged(const QModelIndex)), this, SLOT(handleUnreadCountPossiblyChanged(const QModelIndex)));
        m_mboxModels.append(qobject_cast<QAbstractItemModel *>(account->mailboxModel()));
        m_msgListModels.append(qobject_cast<QAbstractItemModel *>(account->threadingModel()));
    }
    endInsertRows();
    emit countChanged();
}

void AccountsManager::openAccounts()
{
    if (rowCount() > 0) {
        qDebug() << "Oi!! Don't do this... The accounts are already OPEN!!"
                 << " I doubt very much you actually want to do this. \
                    Either grab an account or reload it";
                    return;
    }
    QSettings m_settings;
    if (QFile::exists(m_settings.fileName())) {
        Dekko::Accounts::AccountUtils::checkSettingsFilePermissions(&m_settings);
    } else {
        qDebug("Account settings don't exist yet no need to check the permissions");
    }

    QStringList groups;
    m_settings.beginGroup("Accounts");
    groups = m_settings.childGroups();
    m_settings.endGroup();
    if (!groups.length()) {
        emit noAccountsConfigured();
        return;
    }
    using ubuntu::connectivity::NetworkingStatus;
    NetworkingStatus *networkStatus = new NetworkingStatus(this);
    m_networkStatus = networkStatus->status() == NetworkingStatus::Online;

    Settings::OfflineSettings *prefs = new Settings::OfflineSettings();
    prefs->setAccountId(Dekko::Settings::Id::globalId);
    Q_FOREACH(const QString &group, groups) {
        Account *account = new Account(this);
        account->setAccountId(group);
        // While we are here let's check if we need to migrate the submission settings
        // for MailboxSettings.h
        Settings::SmtpSettings *smtpSettings = static_cast<Settings::SmtpSettings *>(account->smtpSettings());
        if (!smtpSettings->migratedUserPrefs()) {
            auto migrated = [=]()->bool{
                Dekko::Settings::MailboxSettings *settings = new Settings::MailboxSettings(this);
                settings->setAccount(account);
                settings->createDefaults();
                QVariantMap keys = settings->keys();
                settings->write(keys.value("saveToImap").toString(), smtpSettings->saveToImap());
                settings->write(keys.value("sentMailbox").toString(), smtpSettings->sentMailboxName());
                return true;
            };
            smtpSettings->setMigrateUserPrefs(migrated());
        }
        if (!static_cast<Dekko::Settings::ImapSettings *>(
                    account->imapSettings())->server().isEmpty()) {
            account->openImapConnection();
            if ((m_currentAccount.data()->accountId().isEmpty() && prefs->startOffline()) ||
                    (m_currentAccount.data()->accountId().isEmpty() && !m_networkStatus)) {
                if (m_currentAccount.data()->accountId().isEmpty()) {
                    m_currentAccount = account;
                    qDebug() << "OPENING IN OFFLINE MODE";
                    QTimer::singleShot(1500, this, SLOT(finishOpeningFirstAccount()));
                    emit startedOffline();
                }
            }
        }
        appendRow(account);
    }
}

bool AccountsManager::reloadAccount(const QString &accountId, const bool reconnect)
{
    if (!accountId.isEmpty()) {
        for (int i = 0; i < m_accountsList.size(); i++) {
            if (static_cast<Account *>(m_accountsList.at(i))->accountId() == accountId) {
                Account *account = static_cast<Account *>(m_accountsList.at(i));
                Q_ASSERT(account);
                account->killImapConnection();
                account->invalidateSettings();
                if (reconnect) {
                    account->openImapConnection();
                }
                return true;
            }
        }
    }
    return false;
}

bool AccountsManager::closeImapConnection(const QString &accountId)
{
    if (!accountId.isEmpty()) {
        for (int i = 0; i < m_accountsList.size(); i++) {
            if (static_cast<Account *>(m_accountsList.at(i))->accountId() == accountId) {
                Account *account = static_cast<Account *>(m_accountsList.at(i));
                Q_ASSERT(account);
                static_cast<Imap::Mailbox::NetworkWatcher *>(account->network())->setNetworkOffline();
                return true;
            }
        }
    }
    return false;
}

void AccountsManager::setSslPolicy(const QString &accountId, bool confirm)
{
    Q_FOREACH(Account *account, m_accountsList) {
        if (account->accountId() == accountId) {
            account->setSslPolicy(confirm);
            return;
        }
    }
}

void AccountsManager::setPassword(const QString &accountId, const QString &password)
{
    Q_FOREACH(Account *account, m_accountsList) {
        if (account->accountId() == accountId) {
            static_cast<Imap::Mailbox::Model *>(account->imapModel())->setImapPassword(password);
            return;
        }
    }
}

void AccountsManager::setAccessToken(const uint &uoaAccountId, const QByteArray &token)
{
    Q_FOREACH(Account *account, m_accountsList) {
        if (static_cast<Dekko::Settings::UOASettings *>(account->uoaSettings())->uoaAccountId() == uoaAccountId) {
            static_cast<Imap::Mailbox::Model *>(account->imapModel())->setOauthAccessToken(token);
            return;
        }
    }
    emit error(QString("Could not find an account which matches the accountId for this online account"));
}

void AccountsManager::setNetworkStatus(const bool online)
{
    Q_FOREACH(Account *account, m_accountsList) {
        account->setNetworkOnline(online);
    }
    m_networkStatus = online;
    emit networkStatusChanged(online);
}

void AccountsManager::handleUnreadCountPossiblyChanged(const QModelIndex &mbox)
{
    // This will get called for any mailbox the imapmodel currently has open i.e SELECT'ed
    // So we ignore the sent mailbox and instead, loop over each account, pick out the toplevel mailbox
    // and walk till we find INBOX
    Q_UNUSED(mbox);
    int count = 0;
    Q_FOREACH(Account *account, m_accountsList) {
        Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(account->imapModel());
        Imap::Mailbox::TreeItemMailbox *mailboxItem = model->getToplevelMailboxItem();
        if (mailboxItem->fetched()) {
            for (int i = 0; i < model->rowCount(QModelIndex()); i++) {
                QModelIndex mailbox = model->index(i, 0, QModelIndex());
                if (mailbox.data(Imap::Mailbox::RoleMailboxIsINBOX).toBool()) {
                    count += mailbox.data(Imap::Mailbox::RoleUnreadMessageCount).toInt();
                    break;
                }
            }
        }
    }
    emit unreadCountChanged(count);
}

void AccountsManager::openMailbox(const QString &accountId, const QString &mailbox)
{
    Q_FOREACH(Account *account, m_accountsList) {
        if ( account->accountId() == accountId) {
            setCurrentAccount(account);
            static_cast<Imap::Mailbox::MsgListModel *>(account->msgListModel())->setMailbox(mailbox);
        }
    }
}

void AccountsManager::showImapError(QString error)
{
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(Account *account, m_accountsList) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            QMap<QString, QVariant> imapErr;
            imapErr.insert(QString("username"), model->imapUser());
            imapErr.insert(QString("error"), error);
            emit imapError(imapErr);
            return;
        }
    }
    Q_ASSERT(false); // Shouldn't be reachable
}

void AccountsManager::showNetworkError(QString error)
{
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(Account *account, m_accountsList) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            QMap<QString, QVariant> netErr;
            netErr.insert(QString("username"), model->imapUser());
            netErr.insert(QString("error"), error);
            emit networkError(netErr);
            return;
        }
    }
    Q_ASSERT(false); // Shouldn't be reachable
}

void AccountsManager::showAlertReceived(QString message)
{
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(Account *account, m_accountsList) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            QMap<QString, QVariant> alert;
            alert.insert(QString("username"), model->imapUser());
            alert.insert(QString("message"), message);
            emit alertReceived(alert);
            return;
        }
    }
    Q_ASSERT(false); // Shouldn't be reachable
}

void AccountsManager::showAuthRequested()
{
    //Work out which account sent this and send the accountId through to the qml layer
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(Account *account, m_accountsList) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            QMap<QString, QVariant> accountInfo;
            accountInfo.insert(QString("accountId"), account->accountId());
            accountInfo.insert(QString("username"), model->imapUser());
            accountInfo.insert(QString("authType"),
                               static_cast<Dekko::Settings::ImapSettings *>(account->imapSettings())->authenticationMethod());
            accountInfo.insert(QString("server"),
                               static_cast<Dekko::Settings::ImapSettings *>(account->imapSettings())->server());
            emit authRequested(accountInfo);
            return;
        }
    }
    Q_ASSERT(false); // Shouldn't be reachable
}

void AccountsManager::showAuthAttemptFailed(const QString &message)
{
    //Work out which account sent this and send the accountId through to the qml layer
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(Account *account, m_accountsList) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            QMap<QString, QVariant> accountInfo;
            accountInfo.insert(QString("accountId"), account->accountId());
            accountInfo.insert(QString("username"), model->imapUser());
            accountInfo.insert(QString("authType"),
                               static_cast<Dekko::Settings::ImapSettings *>(account->imapSettings())->authenticationMethod());
            accountInfo.insert(QString("server"),
                               static_cast<Dekko::Settings::ImapSettings *>(account->imapSettings())->server());
            accountInfo.insert(QString("authError"), message);
            emit authAttemptFailed(accountInfo);
            return;
        }
    }
    Q_ASSERT(false); // Shouldn't be reachable
}

void AccountsManager::showCheckSslPolicy()
{
    Account *sentAccount = static_cast<Account *>(sender());
    Q_FOREACH(Account *account, m_accountsList) {
        if (account->accountId() == sentAccount->accountId()) {
            emit checkSslPolicy(sentAccount->sslInfo());
            return;
        }
    }
    Q_ASSERT(false);
}

void AccountsManager::connectionStateChanged(QObject *parser, Imap::ConnectionState state)
{
    Q_UNUSED(parser);
    emit connectionStateChanged();
    qDebug() << Imap::connectionStateToString(state);
    Account *account = 0;
    Imap::Mailbox::Model *model = static_cast<Imap::Mailbox::Model *>(sender());
    Q_FOREACH(account, m_accountsList) {
        if (static_cast<Imap::Mailbox::Model *>(account->imapModel())->imapUser() == model->imapUser()) {
            break;
        }
    }
    Q_ASSERT(account);
    using namespace Imap;
    switch (state) {
    case CONN_STATE_NONE:
    case CONN_STATE_HOST_LOOKUP:
    case CONN_STATE_CONNECTING:
    case CONN_STATE_SSL_HANDSHAKE:
    case CONN_STATE_SSL_VERIFYING:
    case CONN_STATE_CONNECTED_PRETLS_PRECAPS:
    case CONN_STATE_CONNECTED_PRETLS:
    case CONN_STATE_STARTTLS_ISSUED:
    case CONN_STATE_STARTTLS_HANDSHAKE:
    case CONN_STATE_STARTTLS_VERIFYING:
    case CONN_STATE_ESTABLISHED_PRECAPS:
    case CONN_STATE_LOGIN:
    case CONN_STATE_POSTAUTH_PRECAPS:
    case CONN_STATE_COMPRESS_DEFLATE:
        break;
    case CONN_STATE_AUTHENTICATED: /**< @short Logged in */
        // If the currentAccount is not set i.e no accountId yet then this
        // is the first account we have processed on opening all connections, so we set this
        // as our current account and open it's inbox
        if (m_currentAccount.data()->accountId().isEmpty()) {
            // This will inform the UI progress bar that although there are no visible tasks actually
            // running while we wait for the finishOpeningFirstAccount to fire that there is still
            // a process in flight.
            m_openingFirstAccount = true;
            emit openingFirstAccountChanged();
            setCurrentAccount(account);
            // although the account is *connected at this stage. It seems we need to give a short break to
            // settle down before we can actually set the initial INBOX or last opened folder (once done),
            // otherwise we end up with an invalid msgList ptr when setting mailbox.
            // So this timer adds a short wait before we finish opening this first account.
            QTimer::singleShot(4000, this, SLOT(finishOpeningFirstAccount()));
        } else if (!qobject_cast<Imap::Mailbox::MsgListModel *>(m_currentAccount.data()->msgListModel())->mailboxName().isEmpty()) {
            // If the current account already has a mailbox open and we are now reconnecting again
            // We need to store the previous mailbox while the models reset, and re-open once the models
            // are ready again. This otherwise results in blank listviews after a model reset
            m_openingFirstAccount = true;
            emit openingFirstAccountChanged();
            m_prevMboxBeforeReconnect = qobject_cast<Imap::Mailbox::MsgListModel *>(account->msgListModel())->mailboxName();
            // FIXME: find a better way here rather than just waiting!!
            // The trick here atm is to allow sufficient time for the updated mailboxes to become available
            // At this exact moment the msglistmodel still holds pointers to the old model indexes.
            // By the time we have called findPreviousMailbox they will be ready for the mailboxfidner
            // to iterate over. Rather strange bug!! :-/
            QTimer::singleShot(3000, this, SLOT(findPreviousMailbox()));
        }
        return;
    case CONN_STATE_SELECTING_WAIT_FOR_CLOSE:
    case CONN_STATE_SELECTING:
    case CONN_STATE_SYNCING:
    case CONN_STATE_SELECTED:
        if (m_openingFirstAccount) {
            qDebug() << "FIRST ACCOUNT SELECTED";
            // Let the UI know we are done.
            m_openingFirstAccount = false;
            emit openingFirstAccountChanged();
        }
    case CONN_STATE_FETCHING_PART:
    case CONN_STATE_FETCHING_MSG_METADATA:
    case CONN_STATE_LOGOUT:
        break;
    }
}
// FIXME: the logic of this function and finishOpeningFirstAccount() is not complete, it's just to get us to a useable msglist for the accountsdrawer
// changes. THis will be fixed in a subsequent MP to this one.
void AccountsManager::setPreviousMailboxOrINBOX(QString mailbox, QModelIndex index)
{
    disconnect(m_searchForMbox.data(), SIGNAL(inboxFound(QString,QModelIndex)), this, SLOT(setPreviousMailboxOrINBOX(QString,QModelIndex)));
    if (mailbox == index.data(Imap::Mailbox::RoleMailboxName)) {
        if (index.isValid()) {
            Imap::Mailbox::MsgListModel *msgList = static_cast<Imap::Mailbox::MsgListModel *>(m_currentAccount.data()->msgListModel());
            if (msgList) {
                msgList->setMailbox(index);
            } else {
                finishOpeningFirstAccount();
            }
            if (!m_prevMboxBeforeReconnect.isEmpty()) {
                m_prevMboxBeforeReconnect = QString();
            }
        } else {
            qDebug() << "Model index no longer valid, trying again";
            findPreviousMailbox();
        }
    }
}

void AccountsManager::finishOpeningFirstAccount()
{
    m_searchForMbox.data()->setModel(qobject_cast<Imap::Mailbox::Model *>(m_currentAccount.data()->imapModel()));
    connect(m_searchForMbox.data(), SIGNAL(inboxFound(QString,QModelIndex)), this, SLOT(setPreviousMailboxOrINBOX(QString,QModelIndex)));
    m_searchForMbox.data()->start();
}

void AccountsManager::findPreviousMailbox()
{
    Imap::Mailbox::MailboxFinder *mboxFinder = new Imap::Mailbox::MailboxFinder(this, static_cast<Imap::Mailbox::Model *>(m_currentAccount.data()->imapModel()));
    connect(mboxFinder, SIGNAL(mailboxFound(QString,QModelIndex)), this, SLOT(setPreviousMailboxOrINBOX(QString,QModelIndex)));
    mboxFinder->addMailbox(m_prevMboxBeforeReconnect);
}

}
}
