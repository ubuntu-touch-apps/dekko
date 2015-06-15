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
#include "AutoConfig.h"
namespace Dekko
{
AutoConfig::AutoConfig(QObject *parent) :
    QObject(parent), m_MCFAC(0), m_inProgress(false), m_status(ConfigStatus::INVALID)
{
    m_MCFAC = new MailConfigFromAutoconfig();
    connect(m_MCFAC, SIGNAL(failed()), this, SLOT(requestFailed()));
    connect(m_MCFAC, SIGNAL(complete(MailConfig*)), this, SLOT(requestSuceeded(MailConfig*)));
}

AutoConfig::~AutoConfig()
{
    m_MCFAC->deleteLater();
}

QString AutoConfig::emailAddress() const
{
    return m_email;
}

void AutoConfig::setEmailAddress(const QString &email)
{
    if (email == m_email) {
        return;
    }
    m_email = email;
    emit emailAddressChanged();
    if (m_email.contains("@", Qt::CaseInsensitive)) {
        m_domain = m_email.split("@")[1];
        emit domainChanged();
    } else {
        m_domain = QString();
    }
}

void AutoConfig::start()
{
    if (m_domain.isEmpty()) {
        emit failed(); // domain needed for request. Fail so we can load manual setup
        return;
    }
    setStatus(ConfigStatus::NEW_REQUEST);
    buildNewRequest();
}

void AutoConfig::buildNewRequest()
{
    // NEW_REQUEST - builds the first of the config request attempts
    // If this fails failedRequest will set the next REQUEST_* type before calling buildNewRequest() again
    // Once all config requests have been exhausted i.e REQUEST_AUTOCONFIG_ISPDB failed
    // The status is set to REQUEST_FAILED and the failed signal emitted.
    switch (m_status) {
    case INVALID:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    case NEW_REQUEST:
        setStatus(REQUEST_AUTOCONFIG);
        buildNewRequest();
        break;
    case REQUEST_AUTOCONFIG:
        m_MCFAC->setUrl(QString("http://autoconfig.%1/mail/config-v1.1.xml").arg(m_domain));
        m_MCFAC->config();
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN:
        m_MCFAC->setUrl(QString("http://%1/.well-known/autoconfig/mail/config-v1.1.xml").arg(m_domain));
        m_MCFAC->config();
        break;
    case REQUEST_AUTOCONFIG_ISPDB:
        m_MCFAC->setUrl(QString("https://autoconfig.thunderbird.net/v1.1/%1").arg(m_domain));
        m_MCFAC->config();
        break;
    case REQUEST_FAILED:
    case REQUEST_SUCCEEDED:
    case BUILDING_RESULT:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    }
}

void AutoConfig::requestFailed()
{
    switch (m_status) {
    case INVALID:
    case NEW_REQUEST:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    case REQUEST_AUTOCONFIG:
        setStatus(REQUEST_AUTOCONFIG_WELLKNOWN);
        buildNewRequest();
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN:
        setStatus(REQUEST_AUTOCONFIG_ISPDB);
        buildNewRequest();
        break;
    // If the request failed on REQUEST_AUTOCONFIG_ISPDB then we
    // have nothing left we can do so just *fail* this.
    case REQUEST_AUTOCONFIG_ISPDB:
    case REQUEST_FAILED:
    case REQUEST_SUCCEEDED:
    case BUILDING_RESULT:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    }
}

void AutoConfig::requestSuceeded(MailConfig *mailConfig)
{
    if (mailConfig != NULL) {
        setStatus(REQUEST_SUCCEEDED);
    }
    emit success(mailConfig);
}

void AutoConfig::setStatus(const AutoConfig::ConfigStatus &status)
{
    // Anything other than INVALID and REQUEST_FAILED means we are in progress.
    if (status == ConfigStatus::INVALID || status == ConfigStatus::REQUEST_FAILED || status == ConfigStatus::REQUEST_SUCCEEDED) {
        m_inProgress = false;
    } else if (!m_inProgress) {
        m_inProgress = true;
    }
    m_status = status;
    emit progressChanged();
    emit statusChanged();
}
}
