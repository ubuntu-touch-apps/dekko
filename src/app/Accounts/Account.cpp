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

#include "Account.h"
#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QThread>
#include <QFileInfo>
#include <QSslKey>
#include <QUuid>
#include "app/Settings/SettingsNames.h"
#include "app/Settings/MailboxSettings.h"
#include "Common/SettingsNames.h"
#include "Common/ConnectionMethod.h"
#include "Common/MetaTypes.h"
#include "Common/Paths.h"
#include "Common/PortNumbers.h"
#include "Plugins/PluginManager.h"
#include "3rdParty/trojita/Streams/SocketFactory.h"
#include "3rdParty/trojita/Imap/Model/CombinedCache.h"
#include "3rdParty/trojita/Imap/Model/MemoryCache.h"
#include "3rdParty/trojita/UiUtils/Formatting.h"
#include "Imap/Model/SystemNetworkWatcher.h"
#include "Imap/Model/UnityNetworkWatcher.h"
#include <ubuntu/connectivity/networking-status.h>
#include "AccountUtils.h"

namespace Dekko
{
namespace Accounts
{

Account::Account(QObject *parent) :
    QObject(parent), m_imapModel(0), m_mboxModel(0), m_mboxTreeModel(0), m_threadModel (0), m_msgListModel(0), m_netWatcher(0),
    m_visibleTasksModel(0), m_settings(new QSettings()), m_profile(0), m_imapSettings(0),
    m_smtpSettings(0), m_uoaSettings(0), m_newAccount(false), m_pwdMigrated(false),
    m_sslInfoIcon(UiUtils::Formatting::IconType::NoIcon)
{
}

bool Account::isOnline() const
{
    return m_imapModel->isNetworkOnline();
}

QObject *Account::imapModel() const
{
    return m_imapModel;
}

QObject *Account::mailboxModel() const
{
    return m_mboxTreeModel;
}

QObject *Account::msgListModel() const
{
    return m_msgListModel;
}

QObject *Account::threadingModel() const
{
    return m_threadModel;
}

QSettings *Account::settings()
{
    return m_settings;
}

void Account::setSettings(QSettings *settings)
{
    m_settings = settings;
    emit settingsChanged();
}

QObject *Account::profile() const
{
    return m_profile;
}

void Account::setProfile(QObject *profile)
{
    Dekko::Settings::AccountProfile *s = qobject_cast<Dekko::Settings::AccountProfile *>(profile);
    if (s->accountId() != m_accountId) {
        emit error(QLatin1String("The accountId does not match"));
        Q_ASSERT(false);
        return;
    }
    delete m_profile;
    m_profile = 0;
    m_profile = s;
    m_profile->save();
    m_profile->setParent(this);
    emit profileChanged();
}

QObject *Account::imapSettings() const
{
    return m_imapSettings;
}

void Account::setImapSettings(QObject *imapSettings)
{
    Dekko::Settings::ImapSettings *s = qobject_cast<Dekko::Settings::ImapSettings *>(imapSettings);
    if (s->accountId() != m_accountId) {
        emit error(QLatin1String("The accountId does not match"));
        Q_ASSERT(false);
        return;
    }
    delete m_imapSettings;
    m_imapSettings = 0;
    m_imapSettings = s;
    m_imapSettings->setParent(this);
    emit imapSettingsChanged();
}

QObject *Account::smtpSettings() const
{
    return m_smtpSettings;
}

void Account::setSmtpSettings(QObject *smtpSettings)
{
    Dekko::Settings::SmtpSettings *s = qobject_cast<Dekko::Settings::SmtpSettings *>(smtpSettings);
    if (s->accountId() != m_accountId) {
        emit error(QLatin1String("The accountId does not match"));
        Q_ASSERT(false);
        return;
    }
    delete m_smtpSettings;
    m_smtpSettings = 0;
    m_smtpSettings = s;
    m_smtpSettings->setParent(this);
    emit smtpSettingsChanged();
}

QObject *Account::uoaSettings() const
{
    return m_uoaSettings;
}

void Account::setUoaSettings(QObject *uoaSettings)
{
    Dekko::Settings::UOASettings *s = qobject_cast<Dekko::Settings::UOASettings *>(uoaSettings);
    if (s->accountId() != m_accountId) {
        emit error(QLatin1String("The accountId does not match"));
        Q_ASSERT(false);
        return;
    }
    delete m_uoaSettings;
    m_uoaSettings = 0;
    m_uoaSettings = s;
    m_uoaSettings->setParent(this);
    emit uoaSettingsChanged();
}

QObject *Account::network() const
{
    return m_netWatcher;
}

QString Account::accountId() const
{
    return m_accountId;
}

void Account::setAccountId(const QString &id)
{
    if (id == m_accountId) {
        return;
    }
    if (m_newAccount) {

    }
    m_accountId = id;
    // force a reload for new accountId
    invalidateSettings();
    m_cacheDir = Common::writablePath(Common::LOCATION_CACHE) + m_accountId + QLatin1Char('/');
}

bool Account::newAccount() const
{
    return m_newAccount;
}

void Account::setNewAccount(const bool isNew)
{
    if (isNew) {
        m_newAccount = true;
        setAccountId(generateNewAccountId());
        // just to be sure that we aren't being set on an active account
        // We invalidate it before reloading.
        invalidateSettings();
        // we should be done now and no longer new.
        m_newAccount = false;
    }
}

void Account::forgetSslCertificate() const
{
    // FIXME
    //    m_settings->remove(Common::SettingsNames::imapSslPemPubKey);

}

bool Account::passwordMigrated() const
{
    return m_pwdMigrated;
}

void Account::setPasswordMigrated(const bool migrated)
{
    if (migrated == m_pwdMigrated) {
        return;
    }
    m_pwdMigrated = migrated;
    emit passwordMigrationChanged();
}

QString Account::generateNewAccountId() const
{
    QSettings settings;
    settings.beginGroup("Accounts");
    // SO the idea here is we run a loop to get a unique id
    for (int i = 0; i < 10; i++) {
        // get a unique ID
        QUuid uid = QUuid::createUuid();
        // turn it to a string and strip {}
        QString uidString = uid.toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
        if (!settings.childGroups().contains(uidString)) {
            return uidString;
        }
    }
    return QString();
}

bool Account::hasVisibleTasks() const
{
    return m_visibleTasksModel ? m_visibleTasksModel->hasVisibleTasks() : false;
}

QVariant Account::sslInfo() const
{
    QMap<QString, QVariant> sslData;
    sslData.insert(QString("accountId"), m_accountId);
    sslData.insert(QString("title"), m_sslInfoTitle);
    sslData.insert(QString("message"), m_sslInfoMessage);
    return sslData;
}

bool Account::isGmail() const
{
    return m_imapSettings->server() == QString("imap.gmail.com") || m_imapSettings->server() == QString("imap.googlemail.com");
}

void Account::setCacheDir(const QString &cacheDir)
{
    m_cacheDir = cacheDir;
}

QStringList Account::unfilteredCapabilities() const
{
    QStringList capabilities;
    capabilities << m_imapModel->capabilities() << m_imapSettings->imapExtensionBlackList();
    return capabilities;
}

void Account::appendExtensionToImapBlacklist(const QString &extension)
{
    if (m_imapSettings->imapExtensionBlackList().contains(extension)) {
        return;
    }
    qDebug() << "APPENDING: " << extension;
    QStringList l = m_imapSettings->imapExtensionBlackList();
    qDebug() << "CURRENT LIST: " << l;
    l.append(extension);
    l.removeDuplicates();
    m_imapSettings->setImapExtensionBlackList(l);
    qDebug() << "BLACKLISTED NOW: " << m_imapSettings->imapExtensionBlackList();
    m_imapSettings->save();
}

void Account::removeExtensionFromImapBlackList(const QString &extension)
{
    if (!m_imapSettings->imapExtensionBlackList().contains(extension)) {
        return;
    }
    qDebug() << "REMOVING: " << extension;
    QStringList l = m_imapSettings->imapExtensionBlackList();
    qDebug() << "CURRENT LIST: " << l;
    l.removeAll(extension);
    l.removeDuplicates();
    l.removeAll("");
    m_imapSettings->setImapExtensionBlackList(l);
    qDebug() << "BLACKLISTED NOW: " << m_imapSettings->imapExtensionBlackList();
    m_imapSettings->save();
}

bool Account::isExtensionBlackListed(const QString &extension)
{
    return m_imapSettings->imapExtensionBlackList().contains(extension);
}

void Account::openImapConnection()
{
    if (m_accountId.isEmpty()) {
        emit error(QStringLiteral("Cannot open an imap connection without an accountId"));
        qDebug() << "[Account::openImapConn] account id empty";
        return;
    }
    if (m_imapModel) {
        qDebug() << "[Account::openImapConn] killing imap connection";
        killImapConnection();
    }
    Q_ASSERT(!m_imapModel);
    using ubuntu::connectivity::NetworkingStatus;

    NetworkingStatus *networkStatus = new NetworkingStatus(this);

    // We follow pretty much the same process as ImapAccess does here.
    Imap::Mailbox::SocketFactoryPtr sockFactoryPtr;
    Imap::Mailbox::TaskFactoryPtr taskFactory(new Imap::Mailbox::TaskFactory());
    Streams::ProxySettings proxySettings = m_imapSettings->useSystemProxy() ?
                Streams::ProxySettings::RespectSystemProxy : Streams::ProxySettings::DirectConnect;
    using Dekko::Settings::ImapSettings;
    switch (m_imapSettings->connectionMethod()) {
    case Dekko::Settings::ImapSettings::ConnectionMethod::NONE:
    case Dekko::Settings::ImapSettings::ConnectionMethod::STARTTLS:
        sockFactoryPtr.reset(new Streams::TlsAbleSocketFactory(m_imapSettings->server(), m_imapSettings->port()));
        sockFactoryPtr->setStartTlsRequired(m_imapSettings->connectionMethod() ==
                                            Dekko::Settings::ImapSettings::ConnectionMethod::STARTTLS);
        sockFactoryPtr->setProxySettings(proxySettings, QLatin1String("imap"));
        qDebug() << "[Account::openImapConn] settings TlsAbleFactory";
        break;
    case Dekko::Settings::ImapSettings::ConnectionMethod::SSL:
        sockFactoryPtr.reset(new Streams::SslSocketFactory(m_imapSettings->server(), m_imapSettings->port()));
        sockFactoryPtr->setProxySettings(proxySettings, QLatin1String("imap"));
        qDebug() << "[Account::openImapConn] settings sslsocket factory";
        break;
    case Dekko::Settings::ImapSettings::ConnectionMethod::PROCESS:
    case Dekko::Settings::ImapSettings::ConnectionMethod::INVALID:
        sockFactoryPtr.reset(new Streams::FakeSocketFactory(Imap::CONN_STATE_LOGOUT));
        qDebug() << "[Account::openImapConn] hmmm we should reach process/invalid";
        break;
    }
    Settings::OfflineSettings *m_offlineSettings = new Settings::OfflineSettings(this);
    m_offlineSettings->setAccountId(Settings::Id::globalId);
    bool shouldUsePersistentCache = m_offlineSettings->cacheOffline() != Dekko::Settings::OfflineSettings::CacheOffline::NONE;
    if (shouldUsePersistentCache && !QDir().mkpath(m_cacheDir)) {
        onCacheError(QString("Failed to create directory %1").arg(m_cacheDir));
        shouldUsePersistentCache = false;
        qDebug() << "FAILED TO CREATE DIRECTORY: " << m_cacheDir;
    }

    if (shouldUsePersistentCache) {
        QFile::Permissions expectedPerms = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner;
        if (QFileInfo(m_cacheDir).permissions() != expectedPerms) {
            if (!QFile::setPermissions(m_cacheDir, expectedPerms)) {
                onCacheError(QString("Failed to set safe permissions on cache directory %1. Using in memory cache for this session").arg(m_cacheDir));
                shouldUsePersistentCache = false;
            }
        }
        if (QFileInfo(Common::writablePath(Common::LOCATION_CACHE)).permissions() != expectedPerms) {
            if (!QFile::setPermissions(Common::writablePath(Common::LOCATION_CACHE), expectedPerms)) {
                onCacheError(QString("Failed to set safe permissions on cache directory %1. Using in memory cache for this session").arg(m_cacheDir));
                shouldUsePersistentCache = false;
            }
        }
    }

    Imap::Mailbox::AbstractCache *cache = 0;

    if (!shouldUsePersistentCache) {
        cache = new Imap::Mailbox::MemoryCache(this);
        qDebug() << "[Account::openImapConn] using memory cache";
    } else {
        cache = new Imap::Mailbox::CombinedCache(this, QLatin1String(m_accountId.toLatin1()), m_cacheDir);
        connect(cache, SIGNAL(error(QString)), this, SLOT(onCacheError(QString)));
        if (! static_cast<Imap::Mailbox::CombinedCache *>(cache)->open()) {
            // Error message was already shown by the cacheError() slot
            cache->deleteLater();
            cache = new Imap::Mailbox::MemoryCache(this);
            qDebug() << "[Account::openImapConn] memory cche";
        } else {
            if (m_offlineSettings->cacheOffline() == Dekko::Settings::OfflineSettings::CacheOffline::ALL) {
                cache->setRenewalThreshold(0);
            } else {
                cache->setRenewalThreshold(m_offlineSettings->cacheOfflineNumberOfDays());
            }
        }
    }

    m_imapModel = new Imap::Mailbox::Model(this, cache, std::move(sockFactoryPtr), std::move(taskFactory));
    m_imapModel->setObjectName(QString::fromUtf8("imapModel-%1").arg(m_accountId));
    qDebug() << "[Account::openImapConn] created imap model";
    Settings::Preferences *m_preferences = new Settings::Preferences(this);
    m_preferences->setAccountId(Settings::Id::globalId);
    m_imapModel->setCapabilitiesBlacklist(m_imapSettings->imapExtensionBlackList());
    m_imapModel->setProperty("trojita-imap-enable-id", m_preferences->imapEnableId());
    // If this is outlook.com(TM) then be agressive on the NOOPing to avoid outlooks
    // aggressive server timeouts
    // TODO: make this configurable in settings
    if (m_imapSettings->server() == QLatin1String("imap-mail.outlook.com")) {
        m_imapModel->setProperty("trojita-imap-noop-period", int(40000));
    }

    connect(m_imapModel, SIGNAL(alertReceived(QString)), this, SLOT(imapAlertReceived(QString)));
    connect(m_imapModel, SIGNAL(imapError(QString)), this, SLOT(imapErrorReceived(QString)));
    connect(m_imapModel, SIGNAL(networkError(QString)), this, SLOT(imapNetworkError(QString)));
    connect(m_imapModel, SIGNAL(logged(uint,Common::LogMessage)), this, SLOT(imapSomethingGotLogged(uint,Common::LogMessage)));
    connect(m_imapModel, SIGNAL(needsSslDecision(QList<QSslCertificate>,QList<QSslError>)),
            this, SLOT(imapSslError(QList<QSslCertificate>,QList<QSslError>)));
    connect(m_imapModel, SIGNAL(requireStartTlsInFuture()), this, SLOT(onRequireStartTlsInFuture()));

    if (qgetenv("QT_QPA_PLATFORM") != "ubuntumirclient") {
        qDebug() << "[Account::openImapConn] created SystemNetworkWatcher";
        m_netWatcher = new Imap::Mailbox::SystemNetworkWatcher(this, m_imapModel);
    } else {
        qDebug() << "[Account::openImapConn] created Unity8 Network watcher";
        m_netWatcher = new Imap::Mailbox::UnityNetworkWatcher(this, m_imapModel);
    }
    bool netStatusOffline = networkStatus->status() == NetworkingStatus::Offline;
    if (netStatusOffline) {
        qDebug() << "Starting offline as no network is available";
        QMetaObject::invokeMethod(m_netWatcher, "setNetworkOffline", Qt::QueuedConnection);
    } else {
        qDebug() << "[Account::openImapConn] changed network status";
        QMetaObject::invokeMethod(m_netWatcher,
                                  m_offlineSettings->startOffline() ?
                                      "setNetworkOffline" : "setNetworkOnline",
                                  Qt::QueuedConnection);
    }
    if (m_imapSettings->authenticationMethod() != Dekko::Settings::ImapSettings::AuthType::OAUTH) {
        m_imapModel->setImapUser(m_imapSettings->username());
        qDebug() << "[Account::openImapConn] setting username";
    }
    qDebug() << "[Account::openImapConn] Connection Ready.";

    m_mboxModel = new Imap::Mailbox::MailboxModel(this, m_imapModel);
    m_mboxModel->setObjectName(QString::fromUtf8("mailboxModel-%1").arg(m_accountId));
    m_mboxTreeModel = new Imap::Mailbox::SubtreeModelOfMailboxModel(this);
    m_mboxTreeModel->setObjectName(QString::fromUtf8("mailboxSubtreeModel-%1").arg(m_accountId));
    m_mboxTreeModel->setSourceModel(m_mboxModel);
    m_mboxTreeModel->setOriginalRoot();
    m_msgListModel = new Imap::Mailbox::MsgListModel(this, m_imapModel);
    m_msgListModel->setObjectName(QString::fromUtf8("msgListmodel-%1").arg(m_accountId));

    m_threadModel = new Imap::Mailbox::ThreadingMsgListModel(this);
    m_threadModel->setObjectName(QString::fromUtf8("threadModel-%1").arg(m_accountId));
    m_threadModel->setSourceModel(m_msgListModel);
    m_threadModel->setUserWantsThreading(false);
    m_threadModel->setUserSearchingSortingPreference(QStringList(), Imap::Mailbox::ThreadingMsgListModel::SORT_NONE, Qt::DescendingOrder);
    m_visibleTasksModel = new Imap::Mailbox::VisibleTasksModel(this, m_imapModel->taskModel());
    m_visibleTasksModel->setObjectName(QString::fromUtf8("visibleTasksModel-%1").arg(m_accountId));
    connect(m_visibleTasksModel, SIGNAL(hasVisibleTasksChanged()), this, SIGNAL(visibleTasksChanged()));
    //    m_watcher = new Imap::Mailbox::ModelWatcher(this);
    //    m_watcher->setModel(m_imapModel);
    if (netStatusOffline) {
        emit modelsChanged();
        return;
    }
    if (!m_offlineSettings->startOffline())
        m_imapModel->reloadMailboxList();
    emit modelsChanged();
}

void Account::killImapConnection()
{
    if (m_imapModel) {
        m_netWatcher->setNetworkOffline();
        delete m_imapModel;
        m_imapModel = 0;
        delete m_mboxModel;
        m_mboxModel = 0;
        delete m_mboxTreeModel;
        m_mboxTreeModel = 0;
        delete m_netWatcher;
        m_netWatcher = 0;
        delete m_msgListModel;
        m_msgListModel = 0;
        delete m_threadModel;
        m_threadModel = 0;
        delete m_visibleTasksModel;
        m_visibleTasksModel = 0;
    } else {
        emit error(QString("Trying to kill a non existant imap connection.... ignoring!! But really you should fix this.."));
    }
}

void Account::resetAll()
{
    killImapConnection();
    invalidateSettings();
}

void Account::invalidateSettings()
{
    if (m_profile) {
        delete m_profile;
        m_profile = 0;
        delete m_imapSettings;
        m_imapSettings = 0;
        delete m_smtpSettings;
        m_smtpSettings = 0;
        delete m_uoaSettings;
        m_uoaSettings = 0;
    }
    reloadSettings();
}

QString Account::getDefaultSignature() const
{
    return QString(tr("Sent using Dekko from my Ubuntu device"));
}

void Account::deleteAccountSettings()
{
    if (m_accountId.isEmpty())
        return;
    AccountUtils::nukeCache(m_accountId);
    Settings::MailboxSettings *mboxSettings = new Settings::MailboxSettings(this);
    mboxSettings->setAccount(this);
    m_settings->beginGroup("Accounts");
    m_settings->remove(m_accountId);
    m_settings->endGroup();
    m_settings->remove(m_accountId);
    mboxSettings->removeAccount();
    this->deleteLater();
    emit accountSettingsDeleted();
}

void Account::nukeCurrentCache()
{
    AccountUtils::nukeCache(m_accountId);
}

void Account::setSslPolicy(bool value)
{
    if (value && !m_sslChain.isEmpty()) {
        m_imapSettings->setPublicKey(m_sslChain[0].publicKey().toPem());
        m_imapSettings->save();
    }
    m_imapModel->setSslPolicy(m_sslChain, m_sslErrors, value);
}

void Account::setNetworkOnline(const bool online)
{
    if (online) {
        m_netWatcher->setNetworkOnline();
    } else {
        m_netWatcher->setNetworkOffline();
    }
}

void Account::save()
{
    m_imapSettings->save();
    m_profile->save();
    m_smtpSettings->save();

    if (!m_settings->group().isEmpty()) {
        m_settings->endGroup();
    }
    Q_ASSERT(m_settings->group().isEmpty());
    m_settings->beginGroup("Accounts");
    if (!m_settings->childGroups().contains(m_accountId)) {
        m_settings->beginGroup(m_accountId);
        m_settings->setValue("AccountId", m_accountId);
        m_settings->endGroup();
    }
    m_settings->endGroup();
    if (!QFile::exists(m_settings->fileName())) {
        m_settings->sync();
    }
    qDebug() << "Saving all settings";
    emit saveAllSettings();
}

void Account::imapAlertReceived(const QString &message)
{
    qDebug() << "alertReceived" << message;
}

void Account::imapErrorReceived(const QString &message)
{
    qDebug() << "imapErrorReceived" << message;
}

void Account::imapNetworkError(const QString &message)
{
    qDebug() << "imapNetworkError" << message;
}

void Account::imapSslError(const QList<QSslCertificate> &sslCertificateChain, const QList<QSslError> &sslErrors)
{
    m_sslChain = sslCertificateChain;
    m_sslErrors = sslErrors;
    qDebug() << "[Account::openImapConn] sslerror";
    QByteArray lastKnownPubKey = m_imapSettings->publicKey();
    if (!m_sslChain.isEmpty() && !lastKnownPubKey.isEmpty() && lastKnownPubKey == m_sslChain[0].publicKey().toPem()) {
        // This certificate chain contains the same public keys as the last time; we should accept that
        m_imapModel->setSslPolicy(m_sslChain, m_sslErrors, true);
    } else if (m_sslErrors.isEmpty()) {
        qDebug() << "NO SSL ERRORS";
        // We havn't seen this certificate chain before but the system has no error with accepting it so let's let it through
        // and trust the system store on this one
        setSslPolicy(true);
    } else {
        UiUtils::Formatting::formatSslState(
                    m_sslChain, lastKnownPubKey, m_sslErrors, &m_sslInfoTitle, &m_sslInfoMessage, &m_sslInfoIcon);
        emit checkSslPolicy();
    }
}

void Account::onRequireStartTlsInFuture()
{
    // It's possible that we're called after the user has already changed their preferences.
    // In order to not change stuff which was not supposed to be changed, let's make sure that we won't undo their changes.
    if (m_imapSettings->connectionMethod() == Settings::ImapSettings::ConnectionMethod::NONE) {
        m_imapSettings->setConnectionMethod(Settings::ImapSettings::ConnectionMethod::STARTTLS);
    }
}

void Account::reloadSettings()
{
    m_profile = new Dekko::Settings::AccountProfile(this);
    m_profile->setAccountId(m_accountId);
    m_profile->reload();
    m_imapSettings = new Dekko::Settings::ImapSettings(this);
    m_imapSettings->setAccountId(m_accountId);
    m_imapSettings->reload();
    m_smtpSettings = new Dekko::Settings::SmtpSettings(this);
    m_smtpSettings->setAccountId(m_accountId);
    m_smtpSettings->reload();
    m_uoaSettings = new Dekko::Settings::UOASettings(this);
    m_uoaSettings->setAccountId(m_accountId);
    m_uoaSettings->reload();
    qDebug() << "[Account::reloadSettings] Complete";
    emit accountIdChanged();
}

void Account::onCacheError(QString msg)
{
    emit error(msg);
    if (m_imapModel) {
        m_imapModel->setCache(new Imap::Mailbox::MemoryCache(m_imapModel));
    }
}

void Account::imapSomethingGotLogged(uint parserId, const Common::LogMessage &message)
{
    if (message.kind != Common::LOG_IO_READ) {
        qDebug() << "LOG" << parserId << message.timestamp << message.kind << message.source << message.message;
    }
}

}
}
