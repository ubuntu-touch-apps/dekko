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
#include "NotificationService.h"
#include <QDebug>

namespace Dekko
{
namespace Notifications
{
NotificationService::NotificationService(QObject *parent) :
    QObject(parent), m_useBackgroundThread(false), m_postalService(0)
{
    m_postalService = new PostalServiceWorker(this);
    m_postalService->registerApp();
    connect(m_postalService, &PostalServiceWorker::tokenChanged, this, &NotificationService::tokenChanged);
    connect(this, SIGNAL(updateUnreadCount(int)), m_postalService, SLOT(setCount(int)));
}

NotificationService::~NotificationService()
{
    if (m_workerThread.isRunning() && m_useBackgroundThread) {
        m_workerThread.quit();
        m_workerThread.wait();
    }
}

void NotificationService::start()
{
    qDebug() << "STARTING SERVICE";
    connect(this, &NotificationService::openConnections, &m_worker, &NotificationWorker::openConnections);
    connect(this, &NotificationService::closeConnections, &m_worker, &NotificationWorker::closeConnections);
    connect(this, &NotificationService::setNetworkPolicy, &m_worker, &NotificationWorker::setNetworkPolicy);
    connect(&m_worker, SIGNAL(logMessage(QString)), this, SLOT(logMessage(QString)));
    connect(&m_worker, SIGNAL(newMessage(PostalMessage*)), m_postalService, SLOT(sendPost(PostalMessage*)));
    connect(&m_worker, SIGNAL(messageNowMarkedRead(PostalMessage*)), m_postalService, SLOT(clearPersistentMessage(PostalMessage*)));
    connect(&m_worker, SIGNAL(unreadCountChanged(int)), m_postalService, SLOT(setCount(int)));
    if (m_useBackgroundThread) {
        qDebug() << "STARTING BACKGROUND THREAD";
        m_worker.moveToThread(&m_workerThread);
        connect(&m_workerThread, SIGNAL(finished()), &m_worker, SLOT(deleteLater()));
        m_workerThread.start();
        qDebug() << "SERVICE RUNNING: " << m_workerThread.isRunning();
    }
}

void NotificationService::stop()
{
    if (m_workerThread.isRunning() && m_useBackgroundThread) {
        m_workerThread.quit();
        m_workerThread.wait();
    }
    disconnect(this, &NotificationService::openConnections, &m_worker, &NotificationWorker::openConnections);
    disconnect(this, &NotificationService::closeConnections, &m_worker, &NotificationWorker::closeConnections);
    disconnect(this, &NotificationService::setNetworkPolicy, &m_worker, &NotificationWorker::setNetworkPolicy);
}

bool NotificationService::isRunning() const
{
    return m_workerThread.isRunning();
}

void NotificationService::sendPostalMessage(PostalMessage *message)
{
    PostalMessage *msg = message;
    qDebug() << ">>>> NotificationsService >>> MESSAGE RECEIVED: FROM: " << msg->from << "SUBJECT:" << msg->subject;
    m_postalService->sendPost(msg);
}

void NotificationService::logMessage(QString &message)
{
    qDebug() << message;
}
}
}
