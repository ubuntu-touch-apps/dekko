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
    id: root
    width: parent.width
    height: checkbox.height
    property alias checked: checkbox.checked
    property alias text: label.text
    Label {
        id: label
        anchors {
            right: parent.right
            left: checkbox.right
            leftMargin: units.gu(1)
            verticalCenter: parent.verticalCenter
        }
        color: Style.common.text
    }
    ListItemWithActionsCheckBox {
        id: checkbox
        anchors.left: parent.left
        isMultiSelect: false
    }
}
