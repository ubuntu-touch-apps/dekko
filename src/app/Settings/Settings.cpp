#include "Settings.h"
#include <QFile>
#include "SettingsNames.h"
#include "app/Accounts/AccountUtils.h"
namespace Dekko
{
namespace Settings
{

const QString Id::globalId = QLatin1String("global");

AbstractSettings::AbstractSettings(QObject *parent) :
    QObject(parent), m_settings(new QSettings())
{
}

bool AbstractSettings::save()
{
    emit error(QString("Saving settings is not supported by AbstractSettings. And should be done in the subclass"));
    return false;
}

bool AbstractSettings::reload()
{
    emit error(QString("Reloading settings is not supported by AbstractSettings. And should be done in the subclass"));
    return false;
}

QString AbstractSettings::accountId() const
{
    return m_accountId;
}

void AbstractSettings::setAccountId(const QString &id)
{
    if (m_accountId == id) {
        return;
    }
    m_accountId = id;
    emit accountIdChanged();
}

QSettings *AbstractSettings::settings() const
{
    return m_settings;
}

void AbstractSettings::setSettings(QSettings *settings)
{
    m_settings = settings;
    emit settingsChanged();
}

AccountProfile::AccountProfile(QObject *parent) :
    AbstractSettings(parent)
{
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reload()));
}

QString AccountProfile::name() const
{
    return m_name;
}

void AccountProfile::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }
    m_name = name;
    emit nameChanged();
}

QString AccountProfile::organization() const
{
    return m_organization;
}

void AccountProfile::setOrganization(const QString &organization)
{
    if (m_organization == organization) {
        return;
    }
    m_organization = organization;
    emit organizationChanged();
}

QString AccountProfile::description() const
{
    return m_description;
}

void AccountProfile::setDescription(const QString &description)
{
    if (m_description == description) {
        return;
    }
    m_description = description;
    emit descriptionChanged();
}

QString AccountProfile::displayName() const
{
    return !m_organization.isEmpty() ? m_description + QString(" (%1)").arg(m_organization) : m_description;
}

QString AccountProfile::domain() const
{
    return getDomainFromEmailAddress();
}

QString AccountProfile::domainIcon() const
{
    return Dekko::Accounts::AccountUtils::getDomainIcon(m_settings->value(Dekko::SettingsNames::imapUserKey, QVariant()).toString());
}

bool AccountProfile::save()
{
    if (m_accountId.isEmpty()) {
        emit error(QString("No account id has been set. Cannot save without one"));
        return false;
    }
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_settings->setValue(Dekko::SettingsNames::profileName, m_name);
    m_settings->setValue(Dekko::SettingsNames::profileDescription, m_description);
    m_settings->setValue(Dekko::SettingsNames::profileOrganization, m_organization);
    // Try to make sure we always drop back to root group when done
    m_settings->endGroup();
    // ensure we sync this back to the settings file so other instances see these new values on reload
    m_settings->sync();
    return true;
}

bool AccountProfile::reload()
{
    if (m_accountId.isEmpty()) {
        emit error(QString("No account id has been set. Cannot save without one"));
        return false;
    }
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    setName(m_settings->value(Dekko::SettingsNames::profileName).toString());
    setDescription(m_settings->value(Dekko::SettingsNames::profileDescription).toString());
    setOrganization(m_settings->value(Dekko::SettingsNames::profileOrganization).toString());
    m_settings->endGroup();
    return true;
}

QString AccountProfile::getDomainFromEmailAddress() const
{
    QString addr = m_settings->value(Dekko::SettingsNames::imapUserKey, QVariant()).toString();
    QString domain;
    if (addr.contains("@", Qt::CaseInsensitive)) {
        domain = addr.split("@")[1];
    } else {
        domain = QString();
    }
    return domain;
}

ImapSettings::ImapSettings(QObject *parent) :
    AbstractSettings(parent), m_port(0), m_connectionMethod(ConnectionMethod::NONE),
    m_needsNetwork(true), m_useSysProxy(false), m_authType(AuthType::PLAIN)
{
    // We want to reload when ever the accountId changed
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reload()));
    connect(this, SIGNAL(portChanged()), this, SLOT(maybeShowPortWarning()));
}

QString ImapSettings::server() const
{
    return m_server;
}

void ImapSettings::setServer(const QString &server)
{
    if (m_server == server) {
        return;
    }
    m_server = server;
    emit serverChanged();
}

int ImapSettings::port() const
{
    return m_port;
}

void ImapSettings::setPort(const int &port)
{
    if (m_port == port) {
        return;
    }
    m_port = port;
    emit portChanged();
}

QString ImapSettings::username() const
{
    return m_username;
}

void ImapSettings::setUsername(const QString &username)
{
    if (m_username == username) {
        return;
    }
    m_username = username;
    emit usernameChanged();
}

ImapSettings::ConnectionMethod ImapSettings::connectionMethod() const
{
    return m_connectionMethod;
}

void ImapSettings::setConnectionMethod(const ImapSettings::ConnectionMethod method)
{
    if (m_connectionMethod == method) {
        return;
    }
    m_connectionMethod = method;
    emit connMethodChanged();
}

ImapSettings::AuthType ImapSettings::authenticationMethod() const
{
    return m_authType;
}

void ImapSettings::setAuthenticationMethod(const ImapSettings::AuthType type)
{
    if (type == m_authType) {
        return;
    }
    m_authType = type;
    emit authenticationMethodChanged();
}

QString ImapSettings::portWarning() const
{
    return m_portWarning;
}

QByteArray ImapSettings::publicKey() const
{
    return m_publicKey;
}

void ImapSettings::setPublicKey(const QByteArray &key)
{
    if (key == m_publicKey) {
        return;
    }
    m_publicKey = key;
    emit publicKeyChanged();
}

bool ImapSettings::needsNetwork() const
{
    return m_needsNetwork;
}

void ImapSettings::setNeedsNetwork(const bool needsNetwork)
{
    if (needsNetwork == m_needsNetwork) {
        return;
    }
    m_needsNetwork = needsNetwork;
    emit needsNetworkChanged();
}

bool ImapSettings::useSystemProxy() const
{
    return m_useSysProxy;
}

void ImapSettings::setUseSystemProxy(const bool useSysProxy)
{
    if (useSysProxy == m_useSysProxy) {
        return;
    }
    m_useSysProxy = useSysProxy;
    emit useSystemProxyChanged();
}

QStringList ImapSettings::imapExtensionBlackList() const
{
    return m_extBlackList;
}

void ImapSettings::setImapExtensionBlackList(const QStringList &extList)
{
    if (extList == m_extBlackList) {
        return;
    }
    m_extBlackList = extList;
    emit extensionBlackListChanged();
}

bool ImapSettings::save()
{
    if (m_accountId.isEmpty()) {
        emit error(QString("No account id has been set. Cannot save without one"));
        return false;
    }
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_settings->setValue(Dekko::SettingsNames::imapHostKey, m_server);
    m_settings->setValue(Dekko::SettingsNames::imapUserKey, m_username);
    m_settings->setValue(Dekko::SettingsNames::imapPortKey, m_port);
    switch (m_connectionMethod) {
    case ConnectionMethod::INVALID:
        break;
    case ConnectionMethod::NONE:
    case ConnectionMethod::STARTTLS:
        m_settings->setValue(Dekko::SettingsNames::imapMethodKey, Dekko::SettingsNames::methodTCP);
        m_settings->setValue(Dekko::SettingsNames::imapStartTlsKey, m_connectionMethod == ConnectionMethod::STARTTLS);
        break;
    case ConnectionMethod::SSL:
        m_settings->setValue(Dekko::SettingsNames::imapMethodKey, Dekko::SettingsNames::methodSSL);
        // Trying to communicate the fact that this is going to be an encrypted connection, even though
        // that settings bit is not actually used
        m_settings->setValue(Dekko::SettingsNames::imapStartTlsKey, true);
        break;
    case ConnectionMethod::PROCESS:
        m_settings->setValue(Dekko::SettingsNames::imapMethodKey, Dekko::SettingsNames::methodProcess);
        break;
    }

    switch (m_authType) {
    case AuthType::PLAIN:
        m_settings->setValue(Dekko::SettingsNames::imapAuthType, Dekko::SettingsNames::plainAuth);
        break;
    case AuthType::LOGIN:
        m_settings->setValue(Dekko::SettingsNames::imapAuthType, Dekko::SettingsNames::loginAuth);
        break;
    case AuthType::OAUTH:
        m_settings->setValue(Dekko::SettingsNames::imapAuthType, Dekko::SettingsNames::oAuth);
        break;
    }
    m_settings->setValue(Dekko::SettingsNames::imapSslPemPubKey, m_publicKey);
    m_settings->setValue(Dekko::SettingsNames::imapNeedsNetwork, m_needsNetwork);
    m_settings->setValue(Dekko::SettingsNames::imapUseSystemProxy, m_useSysProxy);
    m_settings->setValue(Dekko::SettingsNames::imapBlacklistedCapabilities, m_extBlackList);
    // Try to make sure we always drop back to root group when done
    m_settings->endGroup();
    // ensure we sync this back to the settings file so other instances see these new values on reload
    m_settings->sync();
    return true;
}

bool ImapSettings::reload()
{
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    // FIXME: do some logic or find a quick way to drop to root. i.e ""
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    setServer(m_settings->value(Dekko::SettingsNames::imapHostKey).toString());
    setUsername(m_settings->value(Dekko::SettingsNames::imapUserKey).toString());
    if (m_settings->value(Dekko::SettingsNames::imapMethodKey).toString() == Dekko::SettingsNames::methodSSL) {
        setConnectionMethod(ConnectionMethod::SSL);
    } else if (m_settings->value(Dekko::SettingsNames::imapMethodKey).toString() == Dekko::SettingsNames::methodTCP) {
        setConnectionMethod(m_settings->value(Dekko::SettingsNames::imapStartTlsKey).toBool() ?
                    ConnectionMethod::STARTTLS : ConnectionMethod::NONE);
    } else if (m_settings->value(Dekko::SettingsNames::imapMethodKey).toString() == Dekko::SettingsNames::methodProcess) {
        setConnectionMethod(ConnectionMethod::PROCESS);
    } else {
        setConnectionMethod(ConnectionMethod::NONE);
    }
    m_port = m_settings->value(Dekko::SettingsNames::imapPortKey, QVariant(0)).toInt();
    if (!m_port) {
        switch (m_connectionMethod) {
        case ConnectionMethod::NONE:
        case ConnectionMethod::STARTTLS:
            setPort(PORT_IMAP);
            break;
        case ConnectionMethod::SSL:
            setPort(PORT_IMAPS);
            break;
        case ConnectionMethod::PROCESS:
        case ConnectionMethod::INVALID:
            // do nothing
            break;
        }
    }
    if (m_settings->value(Dekko::SettingsNames::imapAuthType).toString() == Dekko::SettingsNames::plainAuth) {
        setAuthenticationMethod(AuthType::PLAIN);
    } else if (m_settings->value(Dekko::SettingsNames::imapAuthType).toString() == Dekko::SettingsNames::loginAuth) {
        setAuthenticationMethod(AuthType::LOGIN);
    } else if (m_settings->value(Dekko::SettingsNames::imapAuthType).toString() == Dekko::SettingsNames::oAuth) {
        setAuthenticationMethod(AuthType::OAUTH);
    } else {
        setAuthenticationMethod(AuthType::PLAIN);
    }

    setPublicKey(m_settings->value(Dekko::SettingsNames::imapSslPemPubKey).toByteArray());
    setNeedsNetwork(m_settings->value(Dekko::SettingsNames::imapNeedsNetwork, true).toBool());
    setUseSystemProxy(m_settings->value(Dekko::SettingsNames::imapUseSystemProxy, true).toBool());
    setImapExtensionBlackList(m_settings->value(Dekko::SettingsNames::imapBlacklistedCapabilities, QVariant()).toStringList());
    // Try to make sure we always drop back to root group when done
    m_settings->endGroup();
    return true;
}

void ImapSettings::maybeShowPortWarning()
{
    QString errorMsg;
    switch (m_connectionMethod) {
    case ConnectionMethod::NONE:
    case ConnectionMethod::STARTTLS:
        if (m_port != PORT_IMAP) {
            errorMsg = QString("This port is nonstandard. The default port is %1").arg(QVariant(PORT_IMAP).toString());
        }
        break;
    case ConnectionMethod::SSL:
        if (m_port != PORT_IMAPS) {
            errorMsg = QString("This port is nonstandard. The default port is %1").arg(QVariant(PORT_IMAPS).toString());
        }
        break;
    case ConnectionMethod::PROCESS:
    case ConnectionMethod::INVALID:
        // do nothing
        break;
    }
    m_portWarning = errorMsg;
    emit portWarningChanged();
}



SmtpSettings::SmtpSettings(QObject *parent) :
    AbstractSettings(parent), m_port(0), m_saveToImap(false), m_useBurl(false), m_submissionMethod(SubmissionMethod::SMTP),
    m_authType(AuthType::PLAIN)
{
    // We want to reload when ever the accountId changed
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reload()));
    connect(this, SIGNAL(portChanged()), this, SLOT(maybeShowPortWarning()));
}

QString SmtpSettings::server() const
{
    return m_server;
}

void SmtpSettings::setServer(const QString &server)
{
    if (server == m_server) {
        return;
    }
    m_server = server;
    emit serverChanged();
}

int SmtpSettings::port() const
{
    return m_port;
}

void SmtpSettings::setPort(const int &port)
{
    if (port == m_port) {
        return;
    }
    m_port = port;
    emit portChanged();
}

QString SmtpSettings::username() const
{
    return m_username;
}

void SmtpSettings::setUsername(const QString &username)
{
    if (username == m_username) {
        return;
    }
    m_username = username;
    emit usernameChanged();
}

SmtpSettings::SubmissionMethod SmtpSettings::submissionMethod() const
{
    return m_submissionMethod;
}

void SmtpSettings::setSubmissionMethod(const SubmissionMethod method)
{
    if (m_submissionMethod == method) {
        return;
    }
    m_submissionMethod = method;
    emit submissionMethodChanged();
}

SmtpSettings::AuthType SmtpSettings::authenticationMethod() const
{
    return m_authType;
}

void SmtpSettings::setAuthenticationMethod(const SmtpSettings::AuthType type)
{
    if (type == m_authType) {
        return;
    }
    m_authType = type;
    emit authenticationMethodChanged();
}

QString SmtpSettings::portWarning() const
{
    return m_portWarning;
}

bool SmtpSettings::saveToImap() const
{
    return m_saveToImap;
}

void SmtpSettings::setSaveToImap(const bool shouldSave)
{
    if (shouldSave == m_saveToImap) {
        return;
    }
    m_saveToImap = shouldSave;
    emit saveToImapChanged();
}

QString SmtpSettings::sentMailboxName() const
{
    return m_sentMboxName;
}

void SmtpSettings::setSentMailboxName(const QString &mboxName)
{
    if (mboxName == m_sentMboxName) {
        return;
    }
    m_sentMboxName = mboxName;
    emit sentMailboxNameChanged();
}

bool SmtpSettings::useBurl() const
{
    return m_useBurl;
}

void SmtpSettings::setUseBurl(const bool shouldUse)
{
    if (shouldUse == m_useBurl) {
        return;
    }
    m_useBurl = shouldUse;
    emit useBurlChanged();
}

bool SmtpSettings::migratedUserPrefs() const
{
    m_settings->beginGroup(m_accountId);
    bool migrated = m_settings->value(QString("migrated-mboxConfig"), false).toBool();
    m_settings->endGroup();
    return migrated;
}

void SmtpSettings::setMigrateUserPrefs(const bool migrated)
{
    m_settings->beginGroup(m_accountId);
    m_settings->setValue(QString("migrated-mboxConfig"), migrated);
    m_settings->sync();
    m_settings->endGroup();
}

bool SmtpSettings::save()
{
    if (m_accountId.isEmpty()) {
        emit error(QString("No account id has been set. Cannot save without one"));
        return false;
    }
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    // FIXME: do some logic or find a quick way to drop to root. i.e ""
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_settings->setValue(Dekko::SettingsNames::smtpHostKey, m_server);
    switch (m_authType) {
    case AuthType::PLAIN:
        m_settings->setValue(Dekko::SettingsNames::smtpAuthType, Dekko::SettingsNames::plainAuth);
        break;
    case AuthType::LOGIN:
        m_settings->setValue(Dekko::SettingsNames::smtpAuthType, Dekko::SettingsNames::loginAuth);
        break;
    case AuthType::OAUTH:
        m_settings->setValue(Dekko::SettingsNames::smtpAuthType, Dekko::SettingsNames::oAuth);
        break;
    }
    switch (m_submissionMethod) {
    case SubmissionMethod::SMTP:
    case SubmissionMethod::SMTP_STARTTLS:
        m_settings->setValue(Dekko::SettingsNames::msaMethodKey, Dekko::SettingsNames::methodSMTP);
        m_settings->setValue(Dekko::SettingsNames::smtpStartTlsKey,
                             m_submissionMethod == SubmissionMethod::SMTP_STARTTLS); // unconditionally
        m_settings->setValue(Dekko::SettingsNames::smtpUserKey, m_username);
        m_settings->setValue(Dekko::SettingsNames::smtpHostKey, m_server);
        m_settings->setValue(Dekko::SettingsNames::smtpPortKey, m_port);
        break;
    case SubmissionMethod::SSMTP:
        m_settings->setValue(Dekko::SettingsNames::msaMethodKey, Dekko::SettingsNames::methodSSMTP);
        m_settings->setValue(Dekko::SettingsNames::smtpUserKey, m_username);
        m_settings->setValue(Dekko::SettingsNames::smtpPortKey, m_port);
        break;
    case SubmissionMethod::SENDMAIL:
    case SubmissionMethod::IMAP_SENDMAIL:
        // NOT SUPPORTED YET
        break;
    }

    m_settings->setValue(Dekko::SettingsNames::composerSaveToImapKey, m_saveToImap);
    if (m_saveToImap) {
        m_settings->setValue(Dekko::SettingsNames::composerImapSentKey, m_sentMboxName);
        m_settings->setValue(Dekko::SettingsNames::smtpUseBurlKey, m_useBurl);
    } else {
        m_settings->setValue(Dekko::SettingsNames::smtpUseBurlKey, false);
    }

    // Try to make sure we always drop back to root group when done
    m_settings->endGroup();
    m_settings->sync();
    return true;
}

bool SmtpSettings::reload()
{
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    // FIXME: do some logic or find a quick way to drop to root. i.e ""
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    setServer(m_settings->value(Dekko::SettingsNames::smtpHostKey).toString());
    setUsername(m_settings->value(Dekko::SettingsNames::smtpUserKey).toString());
    QString connMethod = m_settings->value(Dekko::SettingsNames::msaMethodKey,
                                           Dekko::SettingsNames::methodSMTP).toString();
    if (connMethod == Dekko::SettingsNames::methodSMTP) {
        setSubmissionMethod(m_settings->value(Dekko::SettingsNames::smtpStartTlsKey).toBool() ?
                            SubmissionMethod::SMTP_STARTTLS : SubmissionMethod::SMTP);
    } else if (connMethod == Dekko::SettingsNames::methodSSMTP) {
        setSubmissionMethod(SubmissionMethod::SSMTP);
    } else if (connMethod == Dekko::SettingsNames::methodSSMTP) {
        setSubmissionMethod(SubmissionMethod::SSMTP);
    } else if (connMethod == Dekko::SettingsNames::methodSENDMAIL) {
        setSubmissionMethod(SubmissionMethod::SENDMAIL);
    } else if (connMethod == Dekko::SettingsNames::methodImapSendmail) {
        setSubmissionMethod(SubmissionMethod::IMAP_SENDMAIL);
    } else {
        // unknown submission type, lets default to SMTP
        setSubmissionMethod(SubmissionMethod::SMTP);
    }
    m_port = m_settings->value(Dekko::SettingsNames::smtpPortKey, QVariant(0)).toInt();
    if (!m_port) {
        switch (m_submissionMethod) {
        case SubmissionMethod::SMTP:
        case SubmissionMethod::SMTP_STARTTLS:
            setPort(PORT_SMTP_SUBMISSION);
            break;
        case SubmissionMethod::SSMTP:
            setPort(PORT_SMTP_SSL);
            break;
        case SubmissionMethod::SENDMAIL:
        case SubmissionMethod::IMAP_SENDMAIL:
            //do nothing
            break;
        }
    } else {
        // Let's just check that we should be showing a port warning
        maybeShowPortWarning();
    }

    if (m_settings->value(Dekko::SettingsNames::smtpAuthType).toString() == Dekko::SettingsNames::plainAuth) {
        setAuthenticationMethod(AuthType::PLAIN);
    } else if (m_settings->value(Dekko::SettingsNames::smtpAuthType).toString() == Dekko::SettingsNames::loginAuth) {
        setAuthenticationMethod(AuthType::LOGIN);
    } else if (m_settings->value(Dekko::SettingsNames::smtpAuthType).toString() == Dekko::SettingsNames::oAuth) {
        setAuthenticationMethod(AuthType::OAUTH);
    } else {
        setAuthenticationMethod(AuthType::PLAIN);
    };
    m_saveToImap = m_settings->value(Dekko::SettingsNames::composerSaveToImapKey, false).toBool();
    if (m_saveToImap) {
        m_sentMboxName = m_settings->value(Dekko::SettingsNames::composerImapSentKey).toString();
//        m_settings->setValue(Dekko::SettingsNames::smtpUseBurlKey, m_useBurl);
    } /*else {
//        m_settings->setValue(Dekko::SettingsNames::smtpUseBurlKey, false);
    }*/
    // Try to make sure we always drop back to root group when done

    m_settings->endGroup();
    return true;
}

void SmtpSettings::maybeShowPortWarning()
{
    QString errorMsg;
    switch (m_submissionMethod) {
    case SubmissionMethod::SMTP:
    case SubmissionMethod::SMTP_STARTTLS:
        if (m_port != PORT_SMTP_SUBMISSION) {
            errorMsg = QString("This port is nonstandard. The default port is %1").arg(QVariant(PORT_SMTP_SUBMISSION).toString());
        }
        break;
    case SubmissionMethod::SSMTP:
        if (m_port != PORT_SMTP_SSL) {
            errorMsg = QString("This port is nonstandard. The default port is %1").arg(QVariant(PORT_SMTP_SSL).toString());
        }
        break;
    case SubmissionMethod::SENDMAIL:
    case SubmissionMethod::IMAP_SENDMAIL:
        // do nothing
        break;
    }
    m_portWarning = errorMsg;
    emit portWarningChanged();
}

Preferences::Preferences(QObject *parent) :
    AbstractSettings(parent), m_autoLoadImages(false), m_preferPlainText(false),
    m_autoExpunge(false), m_hideDeleted(false), m_enableId(false), m_markAsRead(NEVER), m_markAsReadAfter(0),
    m_gravatarAllowed(false), m_previewLines(0)
{
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reload()));
    connect(this, SIGNAL(preferencesChanged()), this, SLOT(save()));
    // Preferences comes under the global settings so we use
    // our globalId here.
    setAccountId(Id::globalId);
}

bool Preferences::autoLoadImages() const
{
    return m_autoLoadImages;
}

void Preferences::setAutoLoadImages(const bool autoLoad)
{
    if (autoLoad == m_autoLoadImages) {
        return;
    }
    m_autoLoadImages = autoLoad;
    emit preferencesChanged();
}

bool Preferences::preferPlainText() const
{
    return m_preferPlainText;
}

void Preferences::setPreferPlainText(const bool prefer)
{
    if (prefer == m_preferPlainText) {
        return;
    }
    m_preferPlainText = prefer;
    emit preferencesChanged();
}

bool Preferences::autoExpungeOnDelete() const
{
    return m_autoExpunge;
}

void Preferences::setAutoExpungeOnDelete(const bool shouldExpunge)
{
    if (shouldExpunge == m_autoExpunge) {
        return;
    }
    m_autoExpunge = shouldExpunge;
    emit preferencesChanged();
}

QString Preferences::theme() const
{
    return m_theme;
}

void Preferences::setTheme(const QString &theme)
{
    if (theme == m_theme) {
        return;
    }
    m_theme = theme;
    emit preferencesChanged();
}

bool Preferences::imapEnableId() const
{
    return m_enableId;
}

void Preferences::setImapEnableId(const bool enable)
{
    if (enable == m_enableId) {
        return;
    }
    m_enableId = enable;
    emit preferencesChanged();
}

Preferences::MarkAsRead Preferences::markAsRead() const
{
    return m_markAsRead;
}

void Preferences::setMarkAsRead(Preferences::MarkAsRead value)
{
    if (value == m_markAsRead) {
        return;
    }
    m_markAsRead = value;
    emit preferencesChanged();
}

int Preferences::markAsReadAfter() const
{
    return m_markAsReadAfter;
}

void Preferences::setMarkAsReadAfter(const int &secs)
{
//    if (secs == m_markAsReadAfter) {
//        return;
//    }
    m_markAsReadAfter = secs;
    emit preferencesChanged();
}

bool Preferences::hideMarkedDeleted() const
{
    return m_hideDeleted;
}

void Preferences::setHideMarkedDeleted(const bool hide)
{
    if (hide == m_hideDeleted) {
        return;
    }
    m_hideDeleted = hide;
    emit preferencesChanged();
}

bool Preferences::gravatar() const
{
    return m_gravatarAllowed;
}

void Preferences::setGravatar(const bool allow)
{
    if (allow == m_gravatarAllowed) {
        return;
    }
    m_gravatarAllowed = allow;
    emit preferencesChanged();
}

int Preferences::previewLines() const
{
    return m_previewLines;
}

void Preferences::setPreviewLines(const int &lines)
{
    m_previewLines = lines;
    emit preferencesChanged();
}

bool Preferences::save()
{
    if (m_accountId.isEmpty()) {
        emit error(QString("No account id has been set. Cannot save without one"));
        return false;
    }
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    // FIXME: do some logic or find a quick way to drop to root. i.e ""
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_settings->setValue(Dekko::SettingsNames::imapAutoLoadImages, m_autoLoadImages);
    m_settings->setValue(Dekko::SettingsNames::imapAutoExpunge, m_autoExpunge);
    m_settings->setValue(Dekko::SettingsNames::hideMarkedDeleted, m_hideDeleted);
    m_settings->setValue(Dekko::SettingsNames::preferPlainText, m_preferPlainText);
    m_settings->setValue(Dekko::SettingsNames::theme, m_theme);
    m_settings->setValue(Dekko::SettingsNames::imapEnableId, m_enableId);
    m_settings->setValue(Dekko::SettingsNames::allowGravatar, m_gravatarAllowed);
    switch (m_markAsRead) {
    case NEVER:
        m_settings->setValue(SettingsNames::markAsRead, SettingsNames::neverMarkRead);
        break;
    case AFTER_X_SECS:
        m_settings->setValue(SettingsNames::markAsRead, SettingsNames::secondsBeforeMarkRead);
        m_settings->setValue(SettingsNames::markAsReadAfter, m_markAsReadAfter);
        break;
    case IMMEDIATELY:
        m_settings->setValue(SettingsNames::markAsRead, SettingsNames::immediatelyMarkRead);
        break;
    }
    m_settings->setValue(Dekko::SettingsNames::previewLines, m_previewLines);
    // Try to make sure we always drop back to root group when done
    m_settings->endGroup();
    m_settings->sync();
    return true;
}

bool Preferences::reload()
{
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    // We only do single level grouping in the settings file
    // If the QSettings::endGroup() didn't kick us back to root
    // then were screwed, so fail!!
    // FIXME: do some logic or find a quick way to drop to root. i.e ""
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_autoLoadImages = m_settings->value(Dekko::SettingsNames::imapAutoLoadImages, true).toBool();
    m_autoExpunge = m_settings->value(Dekko::SettingsNames::imapAutoExpunge, false).toBool();
    m_hideDeleted = m_settings->value(Dekko::SettingsNames::hideMarkedDeleted, true).toBool();
    m_preferPlainText = m_settings->value(Dekko::SettingsNames::preferPlainText, false).toBool();
    m_theme = m_settings->value(SettingsNames::theme, QLatin1String("Ubuntu.Components.Themes.Ambiance")).toString();
    m_enableId = m_settings->value(Dekko::SettingsNames::imapEnableId, true).toBool();
    m_gravatarAllowed = m_settings->value(Dekko::SettingsNames::allowGravatar, true).toBool();
    m_markAsReadAfter = m_settings->value(Dekko::SettingsNames::markAsReadAfter, 3).toInt();
    if (m_settings->value(SettingsNames::markAsRead).toString() == SettingsNames::neverMarkRead) {
        m_markAsRead = NEVER;
    } else if (m_settings->value(SettingsNames::markAsRead).toString() == SettingsNames::secondsBeforeMarkRead) {
        m_markAsRead = AFTER_X_SECS;
    } else if (m_settings->value(Dekko::SettingsNames::markAsRead).toString() == SettingsNames::immediatelyMarkRead) {
        m_markAsRead = IMMEDIATELY;
    } else {
        m_markAsRead = IMMEDIATELY;
    }
    m_previewLines = m_settings->value(Dekko::SettingsNames::previewLines, 2).toInt();
    // Try to make sure we always drop back to root group when done
    m_settings->endGroup();
    save();
    return true;
}

OfflineSettings::OfflineSettings(QObject *parent) :
    AbstractSettings(parent), m_startOffline(false), m_cacheNumDays(0)
{
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reload()));
    connect(this, SIGNAL(offlineSettingsChanged()), this, SLOT(save()));
    setAccountId(Id::globalId);
}

bool OfflineSettings::startOffline() const
{
    return m_startOffline;
}

void OfflineSettings::setStartOffline(const bool startOffline)
{
    if (startOffline == m_startOffline) {
        return;
    }
    m_startOffline = startOffline;
    emit offlineSettingsChanged();
}

OfflineSettings::CacheOffline OfflineSettings::cacheOffline() const
{
    return m_cacheOffline;
}

void OfflineSettings::setCacheOffline(const CacheOffline &val)
{
    if (val == m_cacheOffline) {
        return;
    }
    m_cacheOffline = val;

    emit offlineSettingsChanged();
}

int OfflineSettings::cacheOfflineNumberOfDays() const
{
    return m_cacheNumDays;
}

void OfflineSettings::setCacheOfflineNumberOfDays(const int &numDays)
{
    if (numDays == m_cacheNumDays) {
        return;
    }
    m_cacheNumDays = numDays;
    emit offlineSettingsChanged();
}

bool OfflineSettings::save()
{
    if (m_accountId.isEmpty()) {
        emit error(QString("No account id has been set. Cannot save without one"));
        return false;
    }
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_settings->setValue(Dekko::SettingsNames::imapStartOffline, m_startOffline);
    m_settings->setValue(Dekko::SettingsNames::cacheOfflineNumberDaysKey, m_cacheNumDays);
    switch (m_cacheOffline) {
    case CacheOffline::NONE:
        m_settings->setValue(Dekko::SettingsNames::cacheOfflineKey, Dekko::SettingsNames::cacheOfflineNone);
        break;
    case CacheOffline::NUM_DAYS:
        m_settings->setValue(Dekko::SettingsNames::cacheOfflineKey, Dekko::SettingsNames::cacheOfflineXDays);
        break;
    case CacheOffline::ALL:
        m_settings->setValue(Dekko::SettingsNames::cacheOfflineKey, Dekko::SettingsNames::cacheOfflineAll);
        break;
    }
    m_settings->endGroup();
    m_settings->sync();
    return true;
}

bool OfflineSettings::reload()
{
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_startOffline = m_settings->value(Dekko::SettingsNames::imapStartOffline, false).toBool();
    m_cacheNumDays = m_settings->value(Dekko::SettingsNames::cacheOfflineNumberDaysKey, 30).toInt();
    if (m_settings->value(Dekko::SettingsNames::cacheOfflineKey) == Dekko::SettingsNames::cacheOfflineNone) {
        m_cacheOffline = CacheOffline::NONE;
    } else if (m_settings->value(Dekko::SettingsNames::cacheOfflineKey) == Dekko::SettingsNames::cacheOfflineXDays) {
        m_cacheOffline = CacheOffline::NUM_DAYS;
    } else if (m_settings->value(Dekko::SettingsNames::cacheOfflineKey) == Dekko::SettingsNames::cacheOfflineAll) {
       m_cacheOffline = CacheOffline::ALL;
    } else {
        m_cacheOffline = CacheOffline::NUM_DAYS;
    }
    m_settings->endGroup();
    return true;
}

UOASettings::UOASettings(QObject *parent) :
    AbstractSettings(parent), m_uoaAccountId(0), m_provider(DEKKO)
{
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reload()));
}

uint UOASettings::uoaAccountId() const
{
    return m_uoaAccountId;
}

void UOASettings::setUoaAccountId(const uint &id)
{
    if (m_uoaAccountId == id) {
        return;
    }
    m_uoaAccountId = id;
    emit uoaAccountIdChanged();
}

UOASettings::Provider UOASettings::provider() const
{
    return m_provider;
}

void UOASettings::setProvider(const UOASettings::Provider provider)
{
    if (provider == m_provider) {
        return;
    }
    m_provider = provider;
    emit providerChanged();
}

bool UOASettings::save()
{
    if (m_accountId.isEmpty()) {
        emit error(QString("No account id has been set. Cannot save without one"));
        return false;
    }
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_settings->setValue(Dekko::SettingsNames::uoaAccountId, m_uoaAccountId);
    switch (m_provider) {
    case Provider::DEKKO:
        m_settings->setValue(Dekko::SettingsNames::uoaProvider, Dekko::SettingsNames::uoaDekkoProvider);
        break;
    case Provider::GMAIL:
        m_settings->setValue(Dekko::SettingsNames::uoaProvider, Dekko::SettingsNames::uoaGmailProvider);
        break;
    }
    m_settings->endGroup();
    m_settings->sync();
    return true;
}

bool UOASettings::reload()
{
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_uoaAccountId = m_settings->value(Dekko::SettingsNames::uoaAccountId).toUInt();
    if (Dekko::SettingsNames::uoaProvider == Dekko::SettingsNames::uoaDekkoProvider) {
        m_provider = Provider::DEKKO;
    } else if (Dekko::SettingsNames::uoaProvider == Dekko::SettingsNames::uoaGmailProvider) {
        m_provider = Provider::GMAIL;
    } else {
        // Default to our own dekko provider
        m_provider = Provider::DEKKO;
    }
    m_settings->endGroup();
    return true;
}

DeveloperSettings::DeveloperSettings(QObject *parent) :
    AbstractSettings(parent), m_enableNotifications(false), m_devModeEnabled(false), m_imapLogEnabled(false),
    m_enableThreading(false), m_enableContacts(false)
{
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reload()));
    connect(this, &DeveloperSettings::enableNotificationsChanged, [this]() {this->save();});
    // Preferences comes under the global settings so we use
    // our globalId here.
    setAccountId(Id::globalId);
}

bool DeveloperSettings::developerModeEnabled() const
{
    return m_devModeEnabled;
}

void DeveloperSettings::setDeveloperModeEnabled(const bool enabled)
{
    m_devModeEnabled = enabled;
    emit developerModeEnabledChanged();
    save();
}

bool DeveloperSettings::enableImapModelLogging() const
{
    return m_imapLogEnabled;
}

void DeveloperSettings::setEnableImapModelLogging(const bool enable)
{
    m_imapLogEnabled = enable;
    emit enableImapModelLoggingChanged();
}

bool DeveloperSettings::enableThreading() const
{
    return m_enableThreading;
}

void DeveloperSettings::setEnableThreading(const bool enable)
{
    m_enableThreading = enable;
    emit enableThreadingChanged();
}

bool DeveloperSettings::enableContacts() const
{
    return m_enableContacts;
}

void DeveloperSettings::setEnableContacts(const bool enable)
{
    m_enableContacts = enable;
    emit enableContactsChanged();
}

bool DeveloperSettings::save()
{
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_settings->setValue("enableNotifications", m_enableNotifications);
    m_settings->setValue("developerModeEnabled", m_devModeEnabled);
    m_settings->endGroup();
    return true;

}

bool DeveloperSettings::reload()
{
    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup(m_accountId);
    m_enableNotifications = m_settings->value("enableNotifications", false).toBool();
    m_devModeEnabled = m_settings->value("developerModeEnabled", false).toBool();
    m_settings->endGroup();
    return true;
}

}
}
