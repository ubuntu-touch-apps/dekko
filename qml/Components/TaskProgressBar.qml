/* Modified copy from reminders-app, Dan Chapman <dpniel@ubuntu.com> 2015
 * This file is part of Dekko email client for Ubuntu Devices/

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
 *-------------------------------------------------------------------------
 * Copyright: 2014 Canonical, Ltd
 *
 * This file is part of reminders-app
 *
 * reminders-app is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * reminders-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.3
import Ubuntu.Components 1.1

Item {
    height: visible ? units.dp(3) : 0
    width: parent.width
    visible: dekko.currentAccount.hasVisibleTasks || dekko.accountsManager.openingFirstAccount
    onVisibleChanged: visible ? animation.start() : animation.stop()

    Rectangle {
        id: rectangle
        anchors.fill: parent
        color: UbuntuColors.orange
        visible: animation.running // Avoid to show the orange bar before animation starts
    }

    SequentialAnimation {
        id: animation
        loops: Animation.Infinite

        ParallelAnimation {
            PropertyAnimation { target: rectangle; property: "anchors.leftMargin"; from: 0; to: width * 7/8; duration: 1000; easing.type: Easing.InOutQuad }
            PropertyAnimation { target: rectangle; property: "anchors.rightMargin"; from: width * 7/8; to: 0; duration: 1000; easing.type: Easing.InOutQuad }
        }
        ParallelAnimation {
            PropertyAnimation { target: rectangle; property: "anchors.leftMargin"; from: width * 7/8; to: 0; duration: 1000; easing.type: Easing.InOutQuad }
            PropertyAnimation { target: rectangle; property: "anchors.rightMargin"; from: 0; to: width * 7/8; duration: 1000; easing.type: Easing.InOutQuad }
        }
    }
}
