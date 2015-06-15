/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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

#include "ImapSettingVerifier.h"

#include "3rdParty/trojita/Imap/Model/Model.h"
#include "3rdParty/trojita/Imap/Model/TaskFactory.h"
#include "3rdParty/trojita/Imap/Model/MemoryCache.h"
#include "3rdParty/trojita/Imap/Model/DummyNetworkWatcher.h"
#include "3rdParty/trojita/Streams/SocketFactory.h"
#include "3rdParty/trojita/Common/Logging.h"
#include "app/Settings/Settings.h"

namespace Dekko {

ImapSettingVerifier::ImapSettingVerifier(QObject *parent) :
    QObject(parent), m_imapSettings(0), m_imapModel(0), m_authRequested(false), m_result(IMAP_SETTINGS_CONNECT_UNKNOWN)
{
}

void ImapSettingVerifier::start()
{
    m_errorMessage.clear();
    if (m_imapModel) {
        deleteImapModel();
        m_authRequested = false;
    }

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
        break;
    case Dekko::Settings::ImapSettings::ConnectionMethod::SSL:
        sockFactoryPtr.reset(new Streams::SslSocketFactory(m_imapSettings->server(), m_imapSettings->port()));
        sockFactoryPtr->setProxySettings(proxySettings, QLatin1String("imap"));
        break;
    case Dekko::Settings::ImapSettings::ConnectionMethod::PROCESS:
    case Dekko::Settings::ImapSettings::ConnectionMethod::INVALID:
        sockFactoryPtr.reset(new Streams::FakeSocketFactory(Imap::CONN_STATE_LOGOUT));
        break;
    }

    m_cache = new Imap::Mailbox::MemoryCache(this);
    m_imapModel = new Imap::Mailbox::Model(this, m_cache, std::move(sockFactoryPtr), taskFactory);
    connect(m_imapModel, SIGNAL(logged(uint,Common::LogMessage)), this, SLOT(onLogged(uint, Common::LogMessage)));
    connect(m_imapModel, SIGNAL(needsSslDecision(QList<QSslCertificate>,QList<QSslError>)), this, SLOT(imapSslError(QList<QSslCertificate>,QList<QSslError>)));
    connect(m_imapModel, SIGNAL(authRequested()), this, SLOT(onAuthRequested()));
    connect(m_imapModel, SIGNAL(authAttemptFailed(QString)), this, SLOT(onAuthFailed(QString)));
    connect(m_imapModel, SIGNAL(authAttemptSucceeded()), this, SLOT(onAuthSucceeded()));
    connect(m_imapModel, SIGNAL(networkError(QString)), this, SLOT(onNetworkError(QString)));
    m_netWatcher = new Imap::Mailbox::DummyNetworkWatcher(this, m_imapModel);
    m_netWatcher->setNetworkOnline();
    taskFactory->createOpenConnectionTask(m_imapModel);
    setResult(IMAP_SETTINGS_CONNECT_RUNNING);
    emit imapModelChanged();
}

int ImapSettingVerifier::result()
{
    return m_result;
}

void ImapSettingVerifier::setResult(int r)
{
    m_result = (ResultStatus) r;
    emit resultChanged();
}

QAbstractItemModel *ImapSettingVerifier::imapModel()
{
    return m_imapModel;
}

void ImapSettingVerifier::onLogged(uint parserId, Common::LogMessage message)
{
    qDebug() << "LOG" << parserId << message.timestamp << message.kind << message.source << message.message;
}

void ImapSettingVerifier::imapSslError(const QList<QSslCertificate> &sslCertificateChain, const QList<QSslError> &sslErrors)
{
    m_imapModel->setSslPolicy(sslCertificateChain, sslErrors, true);
}

void ImapSettingVerifier::onAuthRequested()
{
    if (!m_authRequested) {
        m_imapModel->setImapUser(m_username);
        m_imapModel->setImapPassword(m_password);
        m_authRequested = true;
    }
}

void ImapSettingVerifier::onAuthFailed(QString error)
{
    m_errorMessage = error;
    setResult(IMAP_SETTINGS_CONNECT_FAILED);
}

void ImapSettingVerifier::onAuthSucceeded()
{
    setResult(IMAP_SETTINGS_CONNECT_SUCCEED);
}

void ImapSettingVerifier::deleteImapModel()
{
    m_netWatcher->deleteLater();
    m_netWatcher = 0;
    m_imapModel->deleteLater();
    m_imapModel = 0;
    m_cache->deleteLater();
    m_cache = 0;
    emit imapModelChanged();
}

void ImapSettingVerifier::onNetworkError(QString error)
{
    m_errorMessage = error;
    setResult(IMAP_SETTINGS_CONNECT_FAILED);
    deleteImapModel();
    m_authRequested = false;
}

}
