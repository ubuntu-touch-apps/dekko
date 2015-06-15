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
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2

ListItem.Empty {
    id: drawerDelegate

    property alias iconSource: image.source
    property alias iconName: image.name
    property alias text: textLabel.text
    property alias infoText: infoText.text
    property bool isSelected: false

    Item {
        anchors.fill: parent
        anchors.margins: units.gu(0.2)
        Icon {
            id: image
            height: units.gu(3); width: height
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
            color: isSelected ? Style.common.textSelected: Style.common.text
        }

        Label {
            id: textLabel
            anchors {
                left: image.right
                leftMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
                right: infoText.left
            }
            fontSize: "medium"
            clip: true
            font.bold: isSelected
            color: isSelected ? Style.common.textSelected: Style.common.text
        }
        Label {
            id: infoText
            visible: text
            anchors {
                right: parent.right
                rightMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
            font.bold: true
            color: isSelected ? Style.common.textSelected: Style.common.text
        }
    }
}
