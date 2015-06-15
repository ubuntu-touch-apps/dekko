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
    id: messageField
    height: input.height + units.gu(1)
    property alias placeholderText: input.placeholderText
    property bool hasActiveFocus
    property alias hasFocus: input.focus
    property alias message: input.displayText
    property alias text: input.text
    property alias _input: input
    signal rectanglePosition()

    signal keyPressed()

    showDivider: false
    __mouseArea.visible: false

    function getRectPos() {
        return input.positionToRectangle(input.cursorPosition)
    }

    Column {
        id: column
        anchors.fill: parent
        anchors.topMargin: units.gu(2)
        spacing: units.gu(1)

        TextArea {
            id: input
            autoSize: true
            maximumLineCount: 0
            onActiveFocusChanged: hasActiveFocus = activeFocus
            anchors.bottomMargin: units.gu(5)

            width: parent.width
            style:TextAreaStyle {
                overlaySpacing: 0
                frameSpacing: 0
                background: Item {}
            }
            onCursorPositionChanged: rectanglePosition()
            Keys.onReleased: keyPressed()
        }
    }
}
