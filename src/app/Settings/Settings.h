#ifndef IMAPSETTINGS_H
#define IMAPSETTINGS_H

#include <QObject>
#include <QStringList>
#include <QSettings>
#include <QStandardPaths>
#include "app/Utils/Helpers.h"

namespace Dekko
{
namespace Settings
{

struct Id {
    static const QString globalId;
};

/*!
 \brief

 \class AbstractSettings Settings.h "src/app/Settings/Settings.h"
*/
class AbstractSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(QSettings *settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    /*!
     \brief

     \fn AbstractSettings
     \param parent
    */
    explicit AbstractSettings(QObject *parent = 0);
    /*!
     \brief

     \fn ~AbstractSettings
    */
    virtual ~AbstractSettings() {}

    /*!
     \brief

     \fn accountId
     \return QString
    */
    QString accountId() const;

    // We can specify a different QSettings file otherwise we just use
    // the default QSettings constructor (preffered) if no file is given. This may help
    // in the case we may want to support switching profiles.
    /*!
     \brief

     \fn settings
     \return QSettings
    */
    QSettings *settings() const;
    /*!
     \brief

     \fn setSettings
     \param settings
    */
    void setSettings(QSettings *settings);

public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    virtual bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    virtual bool reload();
    /*!
     \brief

     \fn setAccountId
     \param id
    */
    void setAccountId(const QString &id);

signals:
    /*!
     \brief

     \fn settingsChanged
    */
    void settingsChanged();
    /*!
     \brief

     \fn accountIdChanged
    */
    void accountIdChanged();
    /*!
     \brief

     \fn error
     \param error
    */
    void error(QString error);

protected:
    QString m_accountId; /*!< TODO */
    QSettings *m_settings; /*!< TODO */
};

/*!
 \brief

 \class AccountProfile Settings.h "src/app/Settings/Settings.h"
*/
class AccountProfile : public AbstractSettings
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString organization READ organization WRITE setOrganization NOTIFY organizationChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY descriptionChanged)
    Q_PROPERTY(QString domain READ domain CONSTANT)
    Q_PROPERTY(QString domainIcon READ domainIcon CONSTANT)

public:
    /*!
     \brief

     \fn AccountProfile
     \param parent
    */
    explicit AccountProfile(QObject *parent = 0);

    /*!
     \brief

     \fn name
     \return QString
    */
    QString name() const;
    /*!
     \brief

     \fn setName
     \param name
    */
    void setName(const QString &name);
    /*!
     \brief

     \fn organization
     \return QString
    */
    QString organization() const;
    /*!
     \brief

     \fn setOrganization
     \param organization
    */
    void setOrganization(const QString &organization);
    /*!
     \brief

     \fn description
     \return QString
    */
    QString description() const;
    /*!
     \brief

     \fn setDescription
     \param description
    */
    void setDescription(const QString &description);

    QString displayName() const;

    /*!
     \brief

     \fn domain
     \return QString
    */
    QString domain() const;
    /*!
     \brief

     \fn domainIcon
     \return QString
    */
    QString domainIcon() const;

signals:
    /*!
     \brief

     \fn nameChanged
    */
    void nameChanged();
    /*!
     \brief

     \fn organizationChanged
    */
    void organizationChanged();
    /*!
     \brief

     \fn descriptionChanged
    */
    void descriptionChanged();

public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    bool reload();

private:
    QString m_name; /*!< TODO */
    QString m_organization; /*!< TODO */
    QString m_description; /*!< TODO */

    /*!
     \brief

     \fn getDomainFromEmailAddress
     \return QString
    */
    QString getDomainFromEmailAddress() const;
};

/* This class contains the Imap settings for the given accountId
 *
 * To be used in qml like so. You can either instantiate an ImapSettings{} component in qml or
 * use the attached imapSettings property of the Account object.
 *
 * NOTE: If using this from qml you either need to call save on the object or pass the object to
 * Account::imapSettings() property which will save it for you
 */
/*!
 \brief

 \class ImapSettings Settings.h "src/app/Settings/Settings.h"
*/
class ImapSettings : public AbstractSettings
{
    Q_OBJECT
    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(ConnectionMethod connectionMethod READ connectionMethod WRITE setConnectionMethod NOTIFY connMethodChanged)
    Q_PROPERTY(AuthType authenticationMethod READ authenticationMethod WRITE setAuthenticationMethod NOTIFY authenticationMethodChanged)
    Q_PROPERTY(QString portWarning READ portWarning NOTIFY portWarningChanged)
    Q_PROPERTY(QByteArray publicKey READ publicKey WRITE setPublicKey NOTIFY publicKeyChanged)
    Q_PROPERTY(bool needsNetwork READ needsNetwork WRITE setNeedsNetwork NOTIFY needsNetworkChanged)
    Q_PROPERTY(bool useSystemProxy READ useSystemProxy WRITE setUseSystemProxy NOTIFY useSystemProxyChanged)
    Q_PROPERTY(QStringList extensionBlackList READ imapExtensionBlackList WRITE setImapExtensionBlackList NOTIFY extensionBlackListChanged)
    Q_ENUMS(ConnectionMethod)
    Q_ENUMS(DefaultPorts)
    Q_ENUMS(AuthType)

public:
    /*!
     \brief

     \fn ImapSettings
     \param parent
    */
    explicit ImapSettings(QObject *parent = 0);
    /*!
     \brief

     \enum ConnectionMethod
    */
    enum ConnectionMethod {
        NONE, // Cleartext session
        STARTTLS, // StartTLS session
        SSL, // Encrypted SSL/TLS session
        PROCESS, // Local process
        INVALID // Invalid connectionMethod we will do nothing on this
    };
    /*!
     \brief

     \enum AuthType
    */
    enum AuthType {
        PLAIN,
        LOGIN,
        OAUTH,
    };
    /*!
     \brief

     \enum DefaultPorts
    */
    enum DefaultPorts {
        PORT_IMAP=143,
        PORT_IMAPS=993
    };

    /*!
     \brief

     \fn server
     \return QString
    */
    QString server() const;
    /*!
     \brief

     \fn setServer
     \param server
    */
    void setServer(const QString &server);
    /*!
     \brief

     \fn port
     \return int
    */
    int port() const;
    /*!
     \brief

     \fn setPort
     \param port
    */
    void setPort(const int &port);
    /*!
     \brief

     \fn username
     \return QString
    */
    QString username() const;
    /*!
     \brief

     \fn setUsername
     \param username
    */
    void setUsername(const QString &username);
    /*!
     \brief

     \fn connectionMethod
     \return ConnectionMethod
    */
    ConnectionMethod connectionMethod() const;
    /*!
     \brief

     \fn setConnectionMethod
     \param method
    */
    void setConnectionMethod(const ConnectionMethod method);
    /*!
     \brief

     \fn authenticationMethod
     \return AuthType
    */
    AuthType authenticationMethod() const;
    /*!
     \brief

     \fn setAuthenticationMethod
     \param type
    */
    void setAuthenticationMethod(const AuthType type);

    /*!
     \brief

     \fn portWarning
     \return QString
    */
    QString portWarning() const;

    /*!
     \brief

     \fn publicKey
     \return QByteArray
    */
    QByteArray publicKey() const;
    /*!
     \brief

     \fn setPublicKey
     \param key
    */
    void setPublicKey(const QByteArray &key);
    /*!
     \brief

     \fn needsNetwork
     \return bool
    */
    bool needsNetwork() const;
    /*!
     \brief

     \fn setNeedsNetwork
     \param needsNetwork
    */
    void setNeedsNetwork(const bool needsNetwork);
    /*!
     \brief

     \fn useSystemProxy
     \return bool
    */
    bool useSystemProxy() const;
    /*!
     \brief

     \fn setUseSystemProxy
     \param useSysProxy
    */
    void setUseSystemProxy(const bool useSysProxy);

    QStringList imapExtensionBlackList() const;
    void setImapExtensionBlackList(const QStringList &extList);

signals:
    /*!
     \brief

     \fn serverChanged
    */
    void serverChanged();
    /*!
     \brief

     \fn portChanged
    */
    void portChanged();
    /*!
     \brief

     \fn usernameChanged
    */
    void usernameChanged();
    /*!
     \brief

     \fn connMethodChanged
    */
    void connMethodChanged();
    /*!
     \brief

     \fn authenticationMethodChanged
    */
    void authenticationMethodChanged();
    /*!
     \brief

     \fn portWarningChanged
    */
    void portWarningChanged();
    /*!
     \brief

     \fn publicKeyChanged
    */
    void publicKeyChanged();
    /*!
     \brief

     \fn needsNetworkChanged
    */
    void needsNetworkChanged();
    /*!
     \brief

     \fn useSystemProxyChanged
    */
    void useSystemProxyChanged();
    void extensionBlackListChanged();

public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    bool reload();
//    /*!
//     \brief

//     \fn appendExtensionToImapBlacklist
//     \param extension
//    */
//    void appendExtensionToImapBlacklist(const QString &extension);
//    /*!
//     \brief

//     \fn removeExtensionFromBlacklist
//     \param extension
//    */
//    void removeExtensionFromBlacklist(const QString &extension);

private slots:
    /*!
     \brief

     \fn maybeShowPortWarning
    */
    void maybeShowPortWarning();

private:
    QString m_username; /*!< TODO */
    QString m_server; /*!< TODO */
    int m_port; /*!< TODO */
    ConnectionMethod m_connectionMethod; /*!< TODO */
    QString m_portWarning; /*!< TODO */
    QByteArray m_publicKey; /*!< TODO */
    bool m_needsNetwork; /*!< TODO */
    bool m_useSysProxy; /*!< TODO */
    AuthType m_authType; /*!< TODO */
    QStringList m_extBlackList; /*!< TODO */
};

/* This class contains the Smtp settings for the given accountId
 *
 * To be used in qml like so. You can either instantiate an SmtpSettings{} component in qml or
 * use the attached smtpSettings property of the Account object.
 */
/*!
 \brief

 \class SmtpSettings Settings.h "src/app/Settings/Settings.h"
*/
class SmtpSettings : public AbstractSettings
{
    Q_OBJECT
    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(SubmissionMethod submissionMethod READ submissionMethod WRITE setSubmissionMethod NOTIFY submissionMethodChanged)
    Q_PROPERTY(AuthType authenticationMethod READ authenticationMethod WRITE setAuthenticationMethod NOTIFY authenticationMethodChanged)
    Q_PROPERTY(QString portWarning READ portWarning NOTIFY portWarningChanged)
    Q_PROPERTY(bool saveToImap READ saveToImap WRITE setSaveToImap NOTIFY saveToImapChanged)
    Q_PROPERTY(QString sentMailboxName READ sentMailboxName WRITE setSentMailboxName NOTIFY sentMailboxNameChanged)
    Q_PROPERTY(bool useBurl READ useBurl WRITE setUseBurl NOTIFY useBurlChanged)
    Q_ENUMS(SubmissionMethod)
    Q_ENUMS(DefaultPorts)
    Q_ENUMS(AuthType)

public:
    /*!
     \brief

     \fn SmtpSettings
     \param parent
    */
    explicit SmtpSettings(QObject *parent = 0);

    /*!
     \brief

     \enum SubmissionMethod
    */
    enum SubmissionMethod {
        SMTP, /**< Non-encrypted connection over network */
        SSMTP, /**< Encrypted network connection from the start */
        SMTP_STARTTLS, /**< Starts with a non-encrypted network connection. Which gets upgraded using STARTTLS */
        SENDMAIL, /**< Use a local Sendmail compatible transfer agent */
        IMAP_SENDMAIL, /**< Uses the IMAP SENDMAIL Extension */
    };
    /*!
     \brief

     \enum AuthType
    */
    enum AuthType {
        PLAIN,
        LOGIN,
        OAUTH,
    };
    /*!
     \brief

     \enum DefaultPorts
    */
    enum DefaultPorts {
        PORT_SMTP_SUBMISSION=587,
        PORT_SMTP_SSL=465
    };

    /*!
     \brief

     \fn server
     \return QString
    */
    QString server() const;
    /*!
     \brief

     \fn setServer
     \param server
    */
    void setServer(const QString &server);
    /*!
     \brief

     \fn port
     \return int
    */
    int port() const;
    /*!
     \brief

     \fn setPort
     \param port
    */
    void setPort(const int &port);
    /*!
     \brief

     \fn username
     \return QString
    */
    QString username() const;
    /*!
     \brief

     \fn setUsername
     \param username
    */
    void setUsername(const QString &username);
    /*!
     \brief

     \fn submissionMethod
     \return SubmissionMethod
    */
    SubmissionMethod submissionMethod() const;
    /*!
     \brief

     \fn setSubmissionMethod
     \param method
    */
    void setSubmissionMethod(const SubmissionMethod method);
    /*!
     \brief

     \fn authenticationMethod
     \return AuthType
    */
    AuthType authenticationMethod() const;
    /*!
     \brief

     \fn setAuthenticationMethod
     \param type
    */
    void setAuthenticationMethod(const AuthType type);

    /*!
     \brief

     \fn portWarning
     \return QString
    */
    QString portWarning() const;

    /*!
     \brief

     \fn saveToImap
     \return bool
    */
    bool saveToImap() const;
    /*!
     \brief

     \fn setSaveToImap
     \param shouldSave
    */
    void setSaveToImap(const bool shouldSave);
    /*!
     \brief

     \fn sentMailboxName
     \return QString
    */
    QString sentMailboxName() const;
    /*!
     \brief

     \fn setSentMailboxName
     \param mboxName
    */
    void setSentMailboxName(const QString &mboxName);
    /*!
     \brief

     \fn useBurl
     \return bool
    */
    bool useBurl() const;
    /*!
     \brief

     \fn setUseBurl
     \param shouldUse
    */
    void setUseBurl(const bool shouldUse);

    bool migratedUserPrefs() const;
    void setMigrateUserPrefs(const bool migrated);

signals:
    /*!
     \brief

     \fn serverChanged
    */
    void serverChanged();
    /*!
     \brief

     \fn portChanged
    */
    void portChanged();
    /*!
     \brief

     \fn usernameChanged
    */
    void usernameChanged();
    /*!
     \brief

     \fn submissionMethodChanged
    */
    void submissionMethodChanged();
    /*!
     \brief

     \fn authenticationMethodChanged
    */
    void authenticationMethodChanged();
    /*!
     \brief

     \fn portWarningChanged
    */
    void portWarningChanged();
    /*!
     \brief

     \fn saveToImapChanged
    */
    void saveToImapChanged();
    /*!
     \brief

     \fn sentMailboxNameChanged
    */
    void sentMailboxNameChanged();
    /*!
     \brief

     \fn useBurlChanged
    */
    void useBurlChanged();

public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    bool reload();

private slots:
    /*!
     \brief

     \fn maybeShowPortWarning
    */
    void maybeShowPortWarning();

private:
    QString m_username; /*!< TODO */
    QString m_server; /*!< TODO */
    int m_port; /*!< TODO */
    QString m_portWarning; /*!< TODO */
    bool m_saveToImap; /*!< TODO */
    QString m_sentMboxName; /*!< TODO */
    bool m_useBurl; /*!< TODO */
    SubmissionMethod m_submissionMethod; /*!< TODO */
    AuthType m_authType; /*!< TODO */

};

/* This class contains the Preferences for the given accountId
 *
 * To be used in qml like so. You can either instantiate a Preferences{} component in qml or
 * use the attached preferences property of the Account object.
 */
/*!
 \brief

 \class Preferences Settings.h "src/app/Settings/Settings.h"
*/
class Preferences : public AbstractSettings
{
    Q_OBJECT
    Q_PROPERTY(bool autoLoadImages READ autoLoadImages WRITE setAutoLoadImages NOTIFY preferencesChanged)
    Q_PROPERTY(bool preferPlainText READ preferPlainText WRITE setPreferPlainText NOTIFY preferencesChanged)
    Q_PROPERTY(bool autoExpungeOnDelete READ autoExpungeOnDelete WRITE setAutoExpungeOnDelete NOTIFY preferencesChanged)
    Q_PROPERTY(bool hideMarkedDeleted READ hideMarkedDeleted WRITE setHideMarkedDeleted NOTIFY preferencesChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY preferencesChanged)
    // We don't actually use these 2 at the moment so disable till supported
//    Q_PROPERTY(QStringList imapProcessArgs READ imapProcessArgs WRITE setImapProcessArgs NOTIFY imapProcessArgsChanged)
    Q_PROPERTY(bool imapEnableId READ imapEnableId WRITE setImapEnableId NOTIFY preferencesChanged)
    Q_PROPERTY(MarkAsRead markAsRead READ markAsRead WRITE setMarkAsRead NOTIFY preferencesChanged)
    Q_PROPERTY(int markAsReadAfter READ markAsReadAfter WRITE setMarkAsReadAfter NOTIFY preferencesChanged)
    Q_PROPERTY(bool gravatarAllowed READ gravatar WRITE setGravatar NOTIFY preferencesChanged)
    Q_PROPERTY(int previewLines READ previewLines WRITE setPreviewLines NOTIFY preferencesChanged)
    Q_ENUMS(MarkAsRead)

public:
    /*!
     \brief

     \fn Preferences
     \param parent
    */
    explicit Preferences(QObject *parent = 0);

    enum MarkAsRead {
        NEVER,
        AFTER_X_SECS,
        IMMEDIATELY
    };

    /*!
     \brief

     \fn autoLoadImages
     \return bool
    */
    bool autoLoadImages() const;
    /*!
     \brief

     \fn setAutoLoadImages
     \param autoLoad
    */
    void setAutoLoadImages(const bool autoLoad);
    /*!
     \brief

     \fn preferPlainText
     \return bool
    */
    bool preferPlainText() const;
    /*!
     \brief

     \fn setPreferPlainText
     \param prefer
    */
    void setPreferPlainText(const bool prefer);
    /*!
     \brief

     \fn autoExpungeOnDelete
     \return bool
    */
    bool autoExpungeOnDelete() const;
    /*!
     \brief

     \fn setAutoExpungeOnDelete
     \param shouldExpunge
    */
    void setAutoExpungeOnDelete(const bool shouldExpunge);

    QString theme() const;
    void setTheme(const QString &theme);

    bool imapEnableId() const;
    void setImapEnableId(const bool enable);

    MarkAsRead markAsRead() const;
    void setMarkAsRead(MarkAsRead value);

    int markAsReadAfter() const;
    void setMarkAsReadAfter(const int &secs);

    bool hideMarkedDeleted() const;
    void setHideMarkedDeleted(const bool hide);
    bool gravatar() const;
    void setGravatar(const bool allow);
    int previewLines() const;
    void setPreviewLines(const int &lines);

signals:

    void preferencesChanged();
public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    bool reload();


private:
    bool m_autoLoadImages; /*!< TODO */
    bool m_preferPlainText; /*!< TODO */
    bool m_autoExpunge; /*!< TODO */
    bool m_hideDeleted;
    QString m_theme;
    bool m_enableId;
    MarkAsRead m_markAsRead;
    int m_markAsReadAfter;
    bool m_gravatarAllowed;
    int m_previewLines;
};

/*!
 \brief

 \class OfflineSettings Settings.h "src/app/Settings/Settings.h"
*/
class OfflineSettings : public AbstractSettings
{
    Q_OBJECT
    Q_PROPERTY(bool startOffline READ startOffline WRITE setStartOffline NOTIFY offlineSettingsChanged)
    Q_PROPERTY(CacheOffline cacheOffline READ cacheOffline WRITE setCacheOffline NOTIFY offlineSettingsChanged)
    Q_PROPERTY(int cacheOfflineNumberOfDays READ cacheOfflineNumberOfDays WRITE setCacheOfflineNumberOfDays NOTIFY offlineSettingsChanged)
    Q_ENUMS(CacheOffline)

public:
    /*!
     \brief

     \fn OfflineSettings
     \param parent
    */
    explicit OfflineSettings(QObject *parent = 0);
    /*!
     \brief

     \enum CacheOffline
    */
    enum CacheOffline {
        NONE, // Does not cache offline
        NUM_DAYS, // Cache for number of days set by cacheOfflineNubmerOfDays()
        ALL // Cache *everything* forever
    };

    /*!
     \brief

     \fn startOffline
     \return bool
    */
    bool startOffline() const;
    /*!
     \brief

     \fn setStartOffline
     \param startOffline
    */
    void setStartOffline(const bool startOffline);
    /*!
     \brief

     \fn cacheOffline
     \return CacheOffline
    */
    CacheOffline cacheOffline() const;
    /*!
     \brief

     \fn setCacheOffline
     \param val
    */
    void setCacheOffline(const CacheOffline &val);
    /*!
     \brief

     \fn cacheOfflineNumberOfDays
     \return int
    */
    int cacheOfflineNumberOfDays() const;
    /*!
     \brief

     \fn setCacheOfflineNumberOfDays
     \param numDays
    */
    void setCacheOfflineNumberOfDays(const int &numDays);

signals:

    void offlineSettingsChanged();

public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    bool reload();

private:
    bool m_startOffline; /*!< TODO */
    CacheOffline m_cacheOffline; /*!< TODO */
    int m_cacheNumDays; /*!< TODO */
};
/*!
 \brief UOASettings

 \class UOASettings Settings.h "src/app/Settings/Settings.h"
*/
class UOASettings : public AbstractSettings
{
    Q_OBJECT
    Q_PROPERTY(uint uoaAccountId READ uoaAccountId WRITE setUoaAccountId NOTIFY uoaAccountIdChanged)
    Q_ENUMS(Provider provider READ provider WRITE setProvider NOTIFY providerChanged)

public:

    explicit UOASettings(QObject *parent = 0);
    /*! Enum representing supported providers. They need to be added here as we support more */
    enum Provider {
        DEKKO, /*< This will be our local dekko Online Accounts provider */
        GMAIL /*< You guessed it!!, a Gmail online accounts provider */
    };

    uint uoaAccountId() const;
    void setUoaAccountId(const uint &id);

    Provider provider() const;
    void setProvider(const Provider provider);

signals:
    void uoaAccountIdChanged();
    void providerChanged();

public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    bool reload();

private:
    uint m_uoaAccountId;
    Provider m_provider;
};

/*!
 \brief

 \class DeveloperSettings Settings.h "src/app/Settings/Settings.h"
*/
class DeveloperSettings : public AbstractSettings
{
    Q_OBJECT
    Q_PROPERTY(bool developerModeEnabled READ developerModeEnabled WRITE setDeveloperModeEnabled NOTIFY developerModeEnabledChanged)
    Q_PROPERTY(bool enableImapModelLogging READ enableImapModelLogging WRITE setEnableImapModelLogging NOTIFY enableImapModelLoggingChanged)
    Q_PROPERTY(bool enableThreading READ enableThreading WRITE setEnableThreading NOTIFY enableThreadingChanged)
    Q_PROPERTY(bool enableContacts READ enableContacts WRITE setEnableContacts NOTIFY enableContactsChanged)
    QML_WRITABLE_PROPERTY(bool, enableNotifications)

public:
    /*!
     \brief

     \fn DeveloperSettings
     \param parent
    */
    explicit DeveloperSettings(QObject *parent = 0);

    /*!
     \brief

     \fn developerModeEnabled
     \return bool
    */
    bool developerModeEnabled() const;
    /*!
     \brief

     \fn setDeveloperModeEnabled
     \param enabled
    */
    void setDeveloperModeEnabled(const bool enabled);
    /*!
     \brief

     \fn enableImapModelLogging
     \return bool
    */
    bool enableImapModelLogging() const;
    /*!
     \brief

     \fn setEnableImapModelLogging
     \param enable
    */
    void setEnableImapModelLogging(const bool enable);
    /*!
     \brief

     \fn enableThreading
     \return bool
    */
    bool enableThreading() const;
    /*!
     \brief

     \fn setEnableThreading
     \param enable
    */
    void setEnableThreading(const bool enable);
    /*!
     \brief

     \fn enableContacts
     \return bool
    */
    bool enableContacts() const;
    /*!
     \brief

     \fn setEnableContacts
     \param enable
    */
    void setEnableContacts(const bool enable);

signals:
    /*!
     \brief

     \fn developerModeEnabledChanged
    */
    void developerModeEnabledChanged();
    /*!
     \brief

     \fn enableImapModelLoggingChanged
    */
    void enableImapModelLoggingChanged();
    /*!
     \brief

     \fn enableThreadingChanged
    */
    void enableThreadingChanged();
    /*!
     \brief

     \fn enableContactsChanged
    */
    void enableContactsChanged();

public slots:
    /*!
     \brief

     \fn save
     \return bool
    */
    bool save();
    /*!
     \brief

     \fn reload
     \return bool
    */
    bool reload();

private:
    bool m_devModeEnabled; /*!< TODO */
    bool m_imapLogEnabled; /*!< TODO */
    bool m_enableThreading; /*!< TODO */
    bool m_enableContacts; /*!< TODO */

};
}
}
#endif // IMAPSETTINGS_H
