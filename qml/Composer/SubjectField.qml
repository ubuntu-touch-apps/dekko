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
import Ubuntu.Components.Themes.Ambiance 0.1

ListItem.Base {
    id: subjectField
    __mouseArea.visible: false
    property alias placeholderText: input.placeholderText
    property alias hasFocus: input.focus
    property alias text: input.text
    property alias cursorPosition: input.cursorPosition
    property string label
    Label {
        id: label
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        text: subjectField.label + " : "
        fontSize: "small"
    }

    TextField {
        id: input
        anchors {
            left: label.right
            verticalCenter: parent.verticalCenter
            right: parent.right
        }

        style:TextFieldStyle {
            background: Item {}
        }
    }
}
