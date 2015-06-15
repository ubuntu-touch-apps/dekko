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
#ifndef AUTOCONFIG_H
#define AUTOCONFIG_H

#include <QObject>
#include "MailConfigFromAutoconfig.h"
#include "MailConfig.h"

namespace Dekko
{
class AutoConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool inProgress MEMBER m_inProgress NOTIFY progressChanged)
    Q_PROPERTY(ConfigStatus status MEMBER m_status NOTIFY statusChanged)
    Q_PROPERTY(QString emailAddress READ emailAddress WRITE setEmailAddress NOTIFY emailAddressChanged)
    Q_PROPERTY(QString domain MEMBER m_domain NOTIFY domainChanged)
    Q_ENUMS(ConfigStatus)

public:
    explicit AutoConfig(QObject *parent = 0);
    ~AutoConfig();

    enum ConfigStatus {
        INVALID,
        NEW_REQUEST,
        REQUEST_AUTOCONFIG,
        REQUEST_AUTOCONFIG_WELLKNOWN,
        REQUEST_AUTOCONFIG_ISPDB,
        REQUEST_FAILED,
        REQUEST_SUCCEEDED,
        BUILDING_RESULT
    };

    QString emailAddress() const;
    void setEmailAddress(const QString &email);

signals:
    void success(MailConfig *config);
    void failed();
    void progressChanged();
    void statusChanged();
    void domainChanged();
    void emailAddressChanged();

public slots:
    void start();

private slots:
    void buildNewRequest();
    void requestFailed();
    void requestSuceeded(MailConfig *mailConfig);

private:
    MailConfigFromAutoconfig *m_MCFAC;
    bool m_inProgress;
    ConfigStatus m_status;
    QString m_email;
    QString m_domain;


    void setStatus(const ConfigStatus &status);
};
}

#endif // AUTOCONFIG_H
