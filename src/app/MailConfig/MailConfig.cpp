/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>
 * Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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
#include "MailConfig.h"
#include <QDebug>
namespace Dekko {

QString MailConfig::imapHost() const
{
    return m_imapHost;
}

void MailConfig::setImapHost(const QString &imapHost)
{
    m_imapHost = imapHost;
}
int MailConfig::imapPort() const
{
    return m_imapPort;
}

void MailConfig::setImapPort(int imapPort)
{
    m_imapPort = imapPort;
}
int MailConfig::imapMethod() const
{
    return m_imapMethod;
}

void MailConfig::setImapMethod(const int imapMethod)
{
    m_imapMethod = imapMethod;
}
QString MailConfig::smtpHost() const
{
    return m_smtpHost;
}

void MailConfig::setSmtpHost(const QString &smtpHost)
{
    m_smtpHost = smtpHost;
}
int MailConfig::smtpPort() const
{
    return m_smtpPort;
}

void MailConfig::setSmtpPort(int smtpPort)
{
    m_smtpPort = smtpPort;
}
int MailConfig::smtpMethod() const
{
    return m_smtpMethod;
}

void MailConfig::setSmtpMethod(const int smtpMethod)
{
    m_smtpMethod = smtpMethod;
}

void MailConfig::clear() {
    m_imapHost.clear();
    m_imapPort = 0;
    m_imapMethod = Dekko::Settings::ImapSettings::ConnectionMethod::NONE;
    m_smtpHost.clear();
    m_smtpPort = 0;
    m_smtpMethod = Dekko::Settings::SmtpSettings::SubmissionMethod::SMTP;
}

void MailConfig::print() {
    qDebug() << imapHost() << imapPort() << imapMethod()
             << smtpHost() << smtpPort() << (int)smtpMethod();
}

}
