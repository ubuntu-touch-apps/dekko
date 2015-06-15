#ifndef ACCOUNTSMANAGER_H
#define ACCOUNTSMANAGER_H

#include <QAbstractListModel>
#include <QList>
#include <QPointer>
#include <QQuickItem>
#include <QQuickView>
#include <QSettings>
#include "Account.h"
#include "PasswordManager.h"
#include "3rdParty/trojita/Imap/Model/NetworkWatcher.h"
#include "Imap/ConnectionState.h"
#include "Imap/Model/MailboxFinder.h"
#include "app/Utils/SearchForSpecialMailbox.h"

extern QQuickView *qQuickViewer; /*!< TODO */

namespace Dekko
{
namespace Accounts
{

/*!
 \brief

 \class AccountsManager AccountsManager.h "src/app/Accounts/AccountsManager.h"
*/
class AccountsManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    /*! This rootView is the MainView object from qml. The accounts manager will be run from this
        view but we need to pass it to the manager to connect up the signals-slots properly.

        Defaults to the QQuickView::rootObject()
    */
    Q_PROPERTY(QObject *rootView READ rootView WRITE setRootView NOTIFY rootViewChanged)
    Q_PROPERTY(bool networkStatus READ networkStatus WRITE setNetworkStatus NOTIFY networkStatusChanged)
    Q_PROPERTY(QList<QAbstractItemModel *> mailboxModels READ mailboxModels NOTIFY countChanged)
    Q_PROPERTY(QList<QAbstractItemModel *> messageListModels READ messageListModels NOTIFY countChanged)
    Q_PROPERTY(QObject *currentAccount READ currentAccount WRITE setCurrentAccount NOTIFY currentAccountChanged)
    Q_PROPERTY(QStringList accountIds READ accountIds NOTIFY countChanged)
    Q_PROPERTY(bool openingFirstAccount READ openingFirstAccount NOTIFY openingFirstAccountChanged)

public:
    /*!
     \brief

     \fn AccountsManager
     \param parent
    */
    explicit AccountsManager(QObject *parent = 0);
    /*!
     \brief

     \fn ~AccountsManager
    */
    ~AccountsManager() {}

    /*!
     \brief

     \typedef AccountsList    */
    typedef QList<Dekko::Accounts::Account *> AccountsList;

    /*!
     \brief

     \enum Roles
    */
    enum Roles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        OrganizationRole,
        DisplayNameRole,
        DomainIconRole
    };

    /*!
     \brief

     \fn roleNames
     \return QHash<int, QByteArray>
    */
    QHash<int, QByteArray> roleNames() const override;

    /*!
     \brief

     \fn data
     \param index
     \param role
     \return QVariant
    */
    QVariant data(const QModelIndex &index, int role) const;
    /*!
     \brief

     \fn rowCount
     \param parent
     \return int
    */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /*!
     \brief

     \fn count
     \return int
    */
    int count() const;
    /*!
     \brief

     \fn indexFromItem
     \param account
     \return QModelIndex
    */
    QModelIndex indexFromItem(Account *account) const;
    /*!
     \brief

     \fn indexFromAccountId
     \param accountId
     \return QModelIndex
    */
    Q_INVOKABLE QModelIndex indexFromAccountId(const QString &accountId);

    /*!
     \brief

     \fn removeRow
     \param row
     \param index
     \return bool
    */
    Q_INVOKABLE bool removeRow(int row, const QModelIndex &index = QModelIndex());
    /*!
     \brief

     \fn removeRows
     \param row
     \param count
     \param index
     \return bool
    */
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &index = QModelIndex());
    // return a ptr to Account at index. This will be used to set the account proxy.
    /*!
     \brief

     \fn get
     \param index
     \return QObject
    */
    Q_INVOKABLE QObject *get(int index);
    Q_INVOKABLE QObject *getFromId(QString accountId);

    /*!
     \brief

     \fn rootView
     \return QObject
    */
    QObject *rootView() const;
    /*!
     \brief

     \fn setRootView
     \param view
    */
    void setRootView(QObject *view);

    bool networkStatus() const;

    QList<QAbstractItemModel *> mailboxModels() const;
    QList<QAbstractItemModel *> messageListModels() const;

    QObject *currentAccount();
    void setCurrentAccount(QObject *account);

    QStringList accountIds();

    bool openingFirstAccount() const;

signals:
    void error(QString msg);
    /*!
     \brief

     \fn countChanged
    */
    void countChanged();
    /*!
     \brief

     \fn rootViewChanged
    */
    void rootViewChanged();
    // Here we pass on any signals that were emitted by all accounts
    /*!
     \brief

     \fn imapError
     \param error
    */
    void imapError(QVariant error);
    /*!
     \brief

     \fn networkError
     \param error
    */
    void networkError(QVariant error);
    /*!
     \brief

     \fn alertReceived
     \param message
    */
    void alertReceived(QVariant message);
    /*!
     \brief

     \fn authRequested
     \param account
    */
    void authRequested(QVariant account);
    /*!
     \brief

     \fn authAttemptFailed
     \param account
    */
    void authAttemptFailed(QVariant account);
    /*!
     \brief

     \fn checkSslPolicy
     \param sslInfo
    */
    void checkSslPolicy(QVariant sslInfo);
    /*!
     \brief

     \fn networkStatusChanged
     \param online
    */
    void networkStatusChanged(bool online);
    void connectionStateChanged();
    void currentAccountChanged();
    void noAccountsConfigured();
    void openingFirstAccountChanged();
    void unreadCountChanged(int count);
    void startedOffline();

public slots:
    /*!
     \brief

     \fn appendRow
     \param account
    */
    void appendRow(Account *account);
    /*!
     \brief

     \fn appendRows
     \param accounts
    */
    void appendRows(QList<Account *> &accounts);
    // This will open up all accounts and open an imap connection.
    /*!
     \brief

     \fn openAccounts
    */
    void openAccounts();
    // This should be called after modifying settings on the account and should we reconnect after reload
    /*!
     \brief

     \fn reloadAccount
     \param accountId
     \param reconnect
     \return bool
    */
    bool reloadAccount(const QString &accountId, const bool reconnect);

    bool closeImapConnection(const QString &accountId);
    /*!
     \brief

     \fn setSslPolicy
     \param accountId
     \param confirm
    */
    void setSslPolicy(const QString &accountId, bool confirm);
    /*!
     \brief

     \fn setPassword
     \param accountId
     \param password
    */
    void setPassword(const QString &accountId, const QString &password);
    /*!
     \brief

     \fn setAccessToken
     \param accountId
     \param token
    */
    void setAccessToken(const uint &uoaAccountId, const QByteArray &token);
    // TODO: support the limited bandwidth.
    /*!
     \brief

     \fn setNetworkStatus
     \param online
    */
    void setNetworkStatus(const bool online);

    void handleUnreadCountPossiblyChanged(const QModelIndex &mbox);

    void openMailbox(const QString &accountId, const QString &mailbox);

private slots:
    /*!
     \brief

     \fn showImapError
     \param error
    */
    void showImapError(QString error);
    /*!
     \brief

     \fn showNetworkError
     \param error
    */
    void showNetworkError(QString error);
    /*!
     \brief

     \fn showAlertReceived
     \param message
    */
    void showAlertReceived(QString message);
    /*!
     \brief

     \fn showAuthRequested
    */
    void showAuthRequested();
    /*!
     \brief

     \fn showAuthAttemptFailed
     \param message
    */
    void showAuthAttemptFailed(const QString &message);
    /*!
     \brief

     \fn showCheckSslPolicy
    */
    void showCheckSslPolicy();

    void connectionStateChanged(QObject *parser, Imap::ConnectionState state);

    void setPreviousMailboxOrINBOX(QString mailbox,QModelIndex index);

    void finishOpeningFirstAccount();
    void findPreviousMailbox();

private:
    QPointer<QObject> m_rootView; /*!< TODO */
    AccountsList m_accountsList; /*!< TODO */
    QList<QAbstractItemModel *> m_mboxModels;
    QList<QAbstractItemModel *> m_msgListModels;
    QPointer<Account> m_currentAccount;
    bool m_networkStatus;
    QString m_prevMboxBeforeReconnect;
    QPointer<Utils::SearchForSpecialMailbox> m_searchForMbox;
    bool m_openingFirstAccount;

    /*!
     \brief

     \fn setupConnections
     \param account
    */
    void setupConnections(Account account);

    Account *getAccountForSender(QObject *);

    friend class PasswordManager; // Needs access to m_accountsList

};
}
}
#endif // ACCOUNTSMANAGER_H
