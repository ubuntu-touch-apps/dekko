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
#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include <QObject>
#include <QThread>
#include <QGSettings>
#include "PostalServiceWorker.h"
#include "NotificationWorker.h"
#include "app/Utils/Helpers.h"

namespace Dekko
{
namespace Notifications
{
class PostalMessage;
class NotificationWorker;
class NotificationService : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(bool, useBackgroundThread)
public:
    explicit NotificationService(QObject *parent = 0);
    ~NotificationService();
    // Start the NotificationWorker in a new thread
    // Note: this will not open the connections
    // use NotificationService::openConnections to start the service
    Q_INVOKABLE void start();
    // Stop the NotificationWorker
    // NOTE: this will quit & delete the worker thread.
    Q_INVOKABLE void stop();
    // Determine if NotificationWorker is running
    Q_INVOKABLE bool isRunning() const;

signals:
    // start the service
    void openConnections();
    // sets the service network policy
    // So it would be a good idea to set this offline
    // whe Qt.application.active. If the app is on screen
    // and the phone is awake then we don't need to be watching
    void setNetworkPolicy(const bool online);
    // Disable but don't delete the service
    void closeConnections();
    // This means settings were changed in the IMAP settings
    // from in the app so we need to reload our configuration
    // in the worker.
    void reloadConnections();

    void tokenChanged(QString token);

    void updateUnreadCount(int count);

protected slots:
    void sendPostalMessage(PostalMessage *message);
    void logMessage(QString &message);

private:
    NotificationWorker m_worker;
    QThread m_workerThread;
    PostalServiceWorker *m_postalService;
};
}
}
#endif // NOTIFICATIONSERVICE_H
