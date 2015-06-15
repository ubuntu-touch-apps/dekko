/* Shamelessly copied from ubuntu address-book-app
 * Copyright (C) 2012-2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.3
import Ubuntu.Components 1.1

ListView {
    id: listView
    objectName: "valuesListView"

    signal recipientTypeClicked
    clip: true
    orientation: ListView.Horizontal
    snapMode: ListView.SnapToItem

    delegate: Item {
        objectName: "item_" + index

        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        width: arrow.width + listLabel.paintedWidth + units.gu(1)
        opacity: currentIndex == index ? 1.0 : 0.4

        Label {
            id: listLabel

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
            }
            width: paintedWidth
            text: modelData

            // style
            fontSize: "medium"

            MouseArea {
                width: parent.width + units.gu(0.5)
                height: parent.height + units.gu(0.5)
                anchors.centerIn: parent
                onClicked: {
                    currentIndex = index
                    listView.recipientTypeClicked();
                }
            }
        }

        Icon {
            id: arrow
            name: "chevron"
            height: visible ? units.gu(1) : 0
            width: height
            visible: index < (listView.count - 1)

            anchors {
                verticalCenter: listLabel.verticalCenter
                verticalCenterOffset: units.dp(2)
                left: listLabel.right
                leftMargin: units.gu(0.5)
            }
        }
    }
}
