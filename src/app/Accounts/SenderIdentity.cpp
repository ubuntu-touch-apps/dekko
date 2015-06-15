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
#include "SenderIdentity.h"

namespace Dekko
{
namespace Accounts
{
SenderIdentity::SenderIdentity(QObject *parent) :
    QObject(parent), m_useDefaultSignature(false)
{
}

QString SenderIdentity::name() const
{
    return m_name;
}

void SenderIdentity::setName(const QString &name)
{
    if (name == m_name) {
        return;
    }
    m_name = name;
    emit dataChanged();
}

QString SenderIdentity::email() const
{
    return m_email;
}

void SenderIdentity::setEmail(const QString &email)
{
    if (email == m_email) {
        return;
    }
    m_email = email;
    emit dataChanged();
}

QString SenderIdentity::organization() const
{
    return m_organization;
}

void SenderIdentity::setOrganization(const QString &org)
{
    if (org == m_organization) {
        return;
    }
    m_organization = org;
    emit dataChanged();
}

QString SenderIdentity::signature() const
{
    return m_signature;
}

void SenderIdentity::setSignature(const QString &signature)
{
    if (signature == m_signature) {
        return;
    }
    if (signature != getDefaultSignature()) {
        setUseDefaultSignature(false);
    }
    m_signature = signature;
    emit dataChanged();
}

QString SenderIdentity::getDefaultSignature() const
{
    return QString(tr("Sent using Dekko from my Ubuntu device"));
}

bool SenderIdentity::useDefaultSignature() const
{
    return m_useDefaultSignature;
}

void SenderIdentity::setUseDefaultSignature(const bool shouldUse)
{
    if (shouldUse == m_useDefaultSignature) {
        return;
    }
    if (shouldUse) {
        setSignature(getDefaultSignature());
    }
    m_useDefaultSignature = shouldUse;
    emit dataChanged();
}

QString SenderIdentity::toFormattedString()
{
    return QString("%1 <%2>").arg(m_name, m_email);
}

}
}
