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
import QtQuick 2.3
import GSettings 1.0
import DekkoCore 0.2

Item {
    id: service
    GSettings {
        id: gsettings
        schema.id: "com.canonical.qtmir"
        schema.path: "/com/canonical/qtmir/"
    }

    Connections {
        target: GlobalSettings.developer
        onEnableNotificationsChanged: {
            if (GlobalSettings.developer.enableNotifications) {
                if (notificationService.isRunning()) {
                    dekko.startService()
                } else {
                    start()
                }
            } else {
                // just close the network Connections
                dekko.stopService()
            }
        }
    }

    function start() {
        dekko.startService.connect(notificationService.openConnections)
        dekko.stopService.connect(notificationService.closeConnections)
        accountsManager.unreadCountChanged.connect(notificationService.updateUnreadCount)
        notificationService.start()
    }

    function stop() {
        dekko.startService.disconnect(notificationService.openConnections)
        dekko.stopService.disconnect(notificationService.closeConnections)
        accountsManager.unreadCountChanged.disconnect(notificationService.updateUnreadCount)
        notificationService.stop()
    }

    NotificationService {
        id: notificationService
        useBackgroundThread: true
        Component.onCompleted: {
            if (GlobalSettings.developer.enableNotifications) {
                service.start()
            }
        }
    }

    property bool active: Qt.application.active
    onActiveChanged: {
        if (!GlobalSettings.developer.enableNotifications) {
            return;
        }

        if (!notificationService.isRunning()) {
            return;
        }
        console.log("WE ARE GOING TO START A TIMER")
        if (Qt.application.state === Qt.ApplicationActive) {
            console.log("Active changed: app is active ")
            if (timer.running) {
                timer.stop()
            }
            stopService()
            if (!dekko.accountsManager.networkStatus) {
                dekko.accountsManager.networkStatus = true
            }
        } else {
            console.log("Active changed: app no longer active")
            timer.start()
        }
    }

    Timer {
        id: timer
        interval: 10000
        onTriggered: {
            // Ok so after 10 seconds let's assume the user doesn't
            // seem to be coming back, let's switch over
            // to the notification service now.
            dekko.accountsManager.networkStatus = false
            startService()
        }
    }
}
