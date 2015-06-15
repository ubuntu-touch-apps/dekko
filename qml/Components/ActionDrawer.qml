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
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.1
import DekkoCore 0.2

Item {
    id: actionsDrawer

    anchors {
        top: parent.bottom
        right: parent.right
    }
    width: units.gu(22)
    height: actionsColumn.height
    clip: actionsColumn.y != 0
    visible: false

    actions: header.actions

    function close() {
        opened = false;
    }

    property bool opened: false
    property list<Action> actions

    onOpenedChanged: {
        if (opened)
            visible = true;
    }

    Rectangle {
        id: btmshadow
        anchors {
            left: parent.left
            right: parent.right
            top: parent.bottom
        }
        height: units.gu(0.6)
        gradient: Gradient {
            GradientStop { position: 1.0; color: "transparent" }
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.2) }
        }
    }
    HorizontalGradiant {
        id: sideShadow
        anchors {
            right: parent.left
            bottom: btmshadow.bottom
            bottomMargin: units.gu(0.3)
            top: parent.top
        }
        width: units.gu(0.6)
        rightToLeft: true
        gradient: Gradient {
            GradientStop { position: 1.0; color: "transparent" }
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.2) }
        }
    }

    InverseMouseArea {
        anchors.fill: parent
        onPressed: actionsDrawer.close();
        enabled: actionsDrawer.opened
    }

    Column {
        id: actionsColumn
        anchors {
            left: parent.left
            right: parent.right
        }
        y: actionsDrawer.opened ? 0 : -height
        Behavior on y { UbuntuNumberAnimation {} }

        onYChanged: {
            if (y == -height)
                actionsDrawer.visible = false;
        }

        Repeater {
            model: actionsDrawer.actions.length > 0 ? actionsDrawer.actions : 0
            delegate: AbstractButton {
                id: actionButton
                objectName: "actionButton" + label.text
                anchors {
                    left: actionsColumn.left
                    right: actionsColumn.right
                }
                height: units.gu(6)
                enabled: action.enabled

                action: modelData
                onClicked: actionsDrawer.close()

                Rectangle {
                    anchors.fill: parent
                    color: actionButton.pressed ? Style.header.background : Style.common.background
                }

                Label {
                    id: label
                    anchors {
                        left: icon.right
                        leftMargin: units.gu(2)
                        right: parent.right
                        rightMargin: units.gu(2)
                        verticalCenter: parent.verticalCenter
                    }
                    text: model.text
                    fontSize: "small"
                    elide: Text.ElideRight
                    color: actionButton.pressed ? Style.header.textSelected : UbuntuColors.darkGrey // TODO: Find out what color text should be here
                }

                Icon {
                    id: icon
                    anchors {
                        left: parent.left
                        leftMargin: units.gu(2)
                        verticalCenter: parent.verticalCenter
                    }
                    width: height
                    height: label.paintedHeight
                    color: label.color
                    name: model.iconName
                }
            }
        }
    }
}
