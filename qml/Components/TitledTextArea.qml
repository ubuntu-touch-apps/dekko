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

FocusScope {
    id: titledTextField
    width: parent.width
    height: column.height

    property alias title : titleLabel.text
    property alias text: textfield.text
//    property alias inputMethodHints: textfield.inputMethodHints
    // TODO: There must be a more elegant way than this
//    property alias font: textfield.font
    property alias textFieldFocusHandle: textfield
//    property bool sendTabEventOnEnter
//    property bool requiredField: false
    readonly property bool _inputValid: requiredField ? textfield.text : true

    Column {
        id: column
        spacing: units.gu(1)
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }

        Label {
            id: titleLabel
            anchors {
                left: parent.left
                leftMargin: units.gu(0.3)
                right: parent.right
            }
        }

        TextArea {
            id: textfield
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            autoSize: true
            maximumLineCount: 10
            anchors {
                left: column.left
                right: column.right
            }
        }
    }
}
