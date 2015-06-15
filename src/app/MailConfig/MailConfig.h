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
#ifndef MAILCONFIG_H
#define MAILCONFIG_H

#include "app/Settings/Settings.h"

namespace Dekko
{

class MailConfig : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString imapHost READ imapHost)
    Q_PROPERTY(int imapPort READ imapPort)
    Q_PROPERTY(int imapMethod READ imapMethod)
    Q_PROPERTY(QString smtpHost READ smtpHost)
    Q_PROPERTY(int smtpPort READ smtpPort)
    Q_PROPERTY(int smtpMethod READ smtpMethod)
public:
    QString imapHost() const;
    void setImapHost(const QString &imapHost);

    int imapPort() const;
    void setImapPort(int imapPort);

    int imapMethod() const;
    void setImapMethod(const int imapMethod);

    QString smtpHost() const;
    void setSmtpHost(const QString &smtpHost);

    int smtpPort() const;
    void setSmtpPort(int smtpPort);

    int smtpMethod() const;
    void setSmtpMethod(const int smtpMethod);

    void clear();
    void print();
private:
    QString m_imapHost;
    int m_imapPort;
    int m_imapMethod;

    QString m_smtpHost;
    int m_smtpPort;
    int m_smtpMethod;
};

}
#endif
