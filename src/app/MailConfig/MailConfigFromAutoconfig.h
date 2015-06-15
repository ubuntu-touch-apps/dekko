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
#ifndef MAILCONFIGDB_H
#define MAILCONFIGDB_H
#include <QNetworkAccessManager>
#include "MailConfig.h"
namespace Dekko
{

class MailConfigFromAutoconfig : public QObject{
    Q_OBJECT
public:
    MailConfigFromAutoconfig();
    ~MailConfigFromAutoconfig();
    void setUrl(QString url);
signals:
    void complete(MailConfig * config);
    void failed();

public slots:
    void config();
    void requestFinished(QNetworkReply * reply);

private:
    QString m_url;
    QNetworkAccessManager * m_nam;
    MailConfig * m_config;
};

}
#endif
