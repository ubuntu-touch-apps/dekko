/* Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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
import Ubuntu.Components 1.1

Row {
    id: toolbarItems
    spacing: units.gu(2)
    layoutDirection: Qt.RightToLeft
    anchors.rightMargin: units.gu(3)
    anchors.topMargin: units.gu(0.5)
    Repeater {
        model: messagesListView.editActions
        delegate: Item {
            id: toolbarItem
            width: Math.max (icon.width, actionNameLabel.width)
            height: icon.height + actionNameLabel.height + units.gu(0.5)
            anchors {
                top: parent.top
            }

            Rectangle {
                color: UbuntuColors.coolGrey
                opacity: 0.1
                anchors.fill: parent
                visible: mouseArea.pressed
                anchors {
                    topMargin: -units.gu(2)
                    leftMargin: -units.gu(1)
                    rightMargin: -units.gu(1)
                    bottomMargin: -units.gu(0.3)
                }
            }
            Icon {
                id: icon
                width: units.gu(3)
                height: width
                anchors.horizontalCenter: parent.horizontalCenter
                name: modelData.iconName
                source: modelData.iconSource
            }
            Label {
                id: actionNameLabel
                anchors {
                    top: icon.bottom
                    topMargin: units.gu(0.5)
                    horizontalCenter: parent.horizontalCenter
                }
                text: modelData.text
                fontSize: "x-small"
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    modelData.trigger();
                }
            }
        }
    }
}
