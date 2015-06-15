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
#ifndef POSTALSERVICEWORKER_H
#define POSTALSERVICEWORKER_H

#include <QObject>
#include <QJsonObject>
#include <QDBusPendingCallWatcher>
#include "app/Settings/NotificationSettings.h"

namespace Dekko
{
namespace Notifications
{

class PostalMessage
{

public:
    QString from;
    QString subject;
    QString accountId;
    QString mailbox;
    QString gravatar;
    int uid;
    int count;
    QJsonDocument toPushMessage(Settings::NotificationSettings *settings, const QString &appId);
    QJsonObject toJsonObject(Settings::NotificationSettings *settings);
    QString tag();
};

class PostalServiceWorker : public QObject
{
    Q_OBJECT
public:
    explicit PostalServiceWorker(QObject *parent = 0);

    void registerApp();
signals:
    void tokenChanged(QString token);
public slots:
    void sendPost(PostalMessage *post);
    void clearPersistentMessage(PostalMessage *message);
    void setCount(int count);

private slots:
    void wasPostDeliveredToPostOffice(QDBusPendingCallWatcher *watcher);
    void clearPersistentTags(QStringList &tags);

private:
    Settings::NotificationSettings *m_settings;
    QString appId;
    QString pkgname;
    QString token;

};
}
}

#endif // POSTALSERVICEWORKER_H
