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
#include "PostalServiceWorker.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingCall>
#include <QtDBus/QDBusPendingReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDateTime>
#include <QDebug>
#include "app/Utils/Path.h"
#define PUSH_SERVICE "com.ubuntu.PushNotifications"
#define POSTAL_SERVICE "com.ubuntu.Postal"
#define PUSH_PATH "/com/ubuntu/PushNotifications"
#define POSTAL_PATH "/com/ubuntu/Postal"
#define PUSH_IFACE "com.ubuntu.PushNotifications"
#define POSTAL_IFACE "com.ubuntu.Postal"
#define APP_ID "dekko.dekkoproject_dekko"

namespace Dekko
{
namespace Notifications
{
QJsonDocument PostalMessage::toPushMessage(Settings::NotificationSettings *settings, const QString &appId)
{
    QJsonObject pushMsg;
    QDateTime date = QDateTime::currentDateTime().addDays(1);
    date.setTimeSpec(Qt::UTC);
    pushMsg[QStringLiteral("appid")] = appId;
    pushMsg[QStringLiteral("token")] = settings->value("push_token").toString();
    pushMsg[QStringLiteral("expire_on")] = date.toString(Qt::ISODate);
    pushMsg[QStringLiteral("clear_pending")] = true;
    pushMsg[QStringLiteral("replace_tag")] = tag();
    pushMsg[QStringLiteral("data")] = this->toJsonObject(settings);
    return QJsonDocument(pushMsg);
}

QJsonObject PostalMessage::toJsonObject(Settings::NotificationSettings *settings) {
    QJsonObject obj;
    QJsonObject notification;
    QJsonObject card;
    card[QStringLiteral("summary")] = this->from;
    card[QStringLiteral("body")] = this->subject;
    card[QStringLiteral("popup")] = settings->value("show_popup").toBool();
    card[QStringLiteral("persist")] = settings->value("persist").toBool();
    card[QStringLiteral("icon")] = this->gravatar;
    QJsonArray actions;
    QString uri = QString("dekko://notify/%1/INBOX").arg(this->accountId);
    actions.append(uri);
    card[QStringLiteral("actions")] = actions;
    notification[QStringLiteral("card")] = card;
    QJsonObject emblem;
    emblem[QStringLiteral("count")] = this->count;
    emblem[QStringLiteral("visible")] = this->count > 0;
    notification[QStringLiteral("emblem")] = emblem;
    if (settings->value("play_sound").toBool()) {
        notification[QStringLiteral("sound")] = Dekko::soundFilePath(QStringLiteral("blop.mp3"));
    }
    QJsonObject vibrate;
    // FIXME: work out why this returns an empty array from settings
    vibrate[QStringLiteral("pattern")] = QJsonArray() << 200 << 100;
    vibrate[QStringLiteral("duration")] = settings->value("vibrate_duration").toInt();
    vibrate[QStringLiteral("repeat")] = settings->value("repeat_vibration").toInt();
    notification[QStringLiteral("vibrate")] = vibrate;
    notification[QStringLiteral("tag")] = tag();
    obj[QStringLiteral("notification")] = notification;
    obj[QStringLiteral("message")] = this->from+QString::number(this->uid);
    return obj;
}

QString PostalMessage::tag()
{
    QString f = this->from;
    return QString("%1%2").arg(f.replace(" ", "")).arg(QString::number(this->uid));
}
PostalServiceWorker::PostalServiceWorker(QObject *parent) :
    QObject(parent), m_settings(0)
{
    appId = QString("dekko.dekkoproject_dekko");
    pkgname = QString("dekko.dekkoproject");
    pkgname = pkgname.replace(".", "_2e");
    m_settings = new Settings::NotificationSettings(this);
}

void PostalServiceWorker::sendPost(PostalMessage *post)
{
    // It would appear sending post directly to the post office doesn't create
    // notifications. It is currently under talks to be unified so that a "Post" will prompt
    // for a notification. So how does polld do it then? All I can see is they do exactly the same as this :-/
#if 0
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    qDebug() << path;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "Post");
    QString notification = QString(QJsonDocument(post->toJsonObject()).toJson(QJsonDocument::Compact));
    qDebug() << notification;
    message << appId << notification;
    QDBusPendingCall pending = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pending, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(wasPostDeliveredToPostOffice(QDBusPendingCallWatcher*)));
    delete post;
#endif
    QNetworkRequest request(QUrl("https://push.ubuntu.com/notify"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    QJsonDocument doc = post->toPushMessage(m_settings, appId);
    QByteArray data = doc.toJson();
    qDebug() << data;
    nam->post(request, data);
    connect(nam, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        if (reply->error()) {
            qDebug() << "ERROR: " << reply->errorString();
        }
        qDebug() << "FINISHED";
    });
}

void PostalServiceWorker::clearPersistentMessage(PostalMessage *message)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage msg = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "ListPersistent");
    QString tag = message->tag();
    msg << this->appId;
    QDBusPendingCall pcall = bus.asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [=](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QStringList> reply = *watcher;
        if (reply.isError()) {
            qDebug() << "Failed to list persistent tags:" << reply.error().message();
        } else {
            QStringList tags = reply.value();
            if (tags.contains(tag)) {
                clearPersistentTags(QStringList() << tag);
            }
        }
        watcher->deleteLater();
    });
}

void PostalServiceWorker::setCount(int count)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    bool visible = count != 0;
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "SetCounter");
    message << this->appId << count << visible;
    QDBusPendingCall pcall = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [=](QDBusPendingCallWatcher *watcher){
        QDBusPendingReply<void> reply = *watcher;
        if (reply.isError()) {
            qDebug() << "Failed to set unread count:" << reply.error().message();
        }
        watcher->deleteLater();
    });
    // Because our mailbox watcher is timestamp based we may still
    // have notifications persisted from previous connections that
    // got marked read at some point but are not in MailboxWatcher::m_uids or MailboxWatcher::m_uidsWaiting
    // Since there is no unread messages there shouldn't be any persisted so
    // just remove them all.
    if (count == 0) {
        QStringList fakeList;
        clearPersistentTags(fakeList);
    }
}

void PostalServiceWorker::registerApp()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(PUSH_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(
                PUSH_SERVICE, path, PUSH_IFACE, "Register");
    message << appId;
    QDBusPendingCall pending = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pending, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [=](QDBusPendingCallWatcher *watcher){
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isError()) {
            qDebug() << "Failed to register with push service:" << reply.error().message();
        } else {
            qDebug() << "SUCCESS REGISTERING WITH PUSH SERVICE";
            token = reply.value();
            m_settings->write(QLatin1String("push_token"), token);
            emit tokenChanged(token);
        }
        watcher->deleteLater();
    });
}

void PostalServiceWorker::wasPostDeliveredToPostOffice(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<void> reply = *watcher;
    if (reply.isError()) {
        qDebug() << "Delivery failed: " << reply.error().message();
    }
    watcher->deleteLater();
}

void PostalServiceWorker::clearPersistentTags(QStringList &tags)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(
                POSTAL_SERVICE, path, POSTAL_IFACE, "ClearPersistent");
    message << this->appId;
    for (int i = 0; i < tags.size(); ++i) {
        message << tags.at(i);
    }
    QDBusPendingCall pcall = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [=](QDBusPendingCallWatcher *watcher){
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isError()) {
            qDebug() << "Failed to remove persistent tag" << reply.error().message();
        } else {
            qDebug() << "Persistent tag removed";
        }
        watcher->deleteLater();
    });
}

}
}
