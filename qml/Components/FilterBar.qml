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
import Ubuntu.Components 1.1
import DekkoCore 0.2

Item {
    id: filterBar

    property var filterSections: []
    property alias selectedIndex: row.currentIndex
    height: units.gu(0)
    width: parent.width

    Behavior on height {
        UbuntuNumberAnimation {duration: 200}
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Style.header.background
    }

    Connections {
        target: dekko.accountsManager
        onCurrentAccountChanged: {
            row.currentIndex = 0
            row.positionViewAtIndex(0, ListView.Visible )
        }
    }

    ListView {
        id: row
        anchors {
            fill: parent
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        highlightFollowsCurrentItem: true
        clip: true
        snapMode: ListView.SnapToItem
        orientation: ListView.Horizontal
        layoutDirection: ListView.LeftToRight
        currentIndex: 0
        model: filterSections
        delegate: AbstractButton {
            width: row.width / 3
            height: parent.height
            Label {
                text: modelData
                color: row.currentIndex === model.index ? Style.header.textSelected : Style.header.text
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
            }
            Rectangle {
                color: row.currentIndex === model.index ? Style.header.textSelected : Style.header.text
                height: units.gu(0.3)
                width: parent.width
                anchors.bottom: parent.bottom
            }
            onClicked: row.currentIndex = model.index
        }

    }
}
