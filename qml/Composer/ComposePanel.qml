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

import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItems
import Ubuntu.Components.Popups 1.0
import TrojitaCore 0.1
import "../Components"
import "../Dialogs"

Panel {
    id: bottomPanel
    animate: true
    z: 1
    __closeOnContentsClicks: true

    readonly property int tipHeight: units.gu(3)
    property alias tipText: tipLabel.text
    triggerSize: tipHeight
    signal submissionRequested
    locked: !dekko.networkAvailable || opened

    Connections {
        target: Qt.inputMethod
        onAnimatingChanged: Qt.inputMethod.animating ? bottomPanel.locked = true : bottomPanel.locked = false
        onVisibleChanged: Qt.inputMethod.visible ? bottomPanel.locked = true : bottomPanel.locked = false
    }

    Timer {
        id: collapseIndicator
        interval: 3000
        running: true
        repeat: false
        onTriggered: tipContainer.state = "collapsed"
    }

    onOpenedChanged: {
        if (!opened) {
            tipContainer.visible = true
            collapseIndicator.start()
        } else {
            tipContainer.visible = false
        }
    }


    Binding {
        target: tipContainer
        property: "state"
        value: "expanded"
        when: bottomPanel.animating
    }
    Binding {
        target: tipContainer
        property: "state"
        value: "expanded"
        when: !opened && bottomPanel.pressed
    }
    Binding {
        target: tipContainer
        property: "visible"
        value: false
        when: !dekko.networkAvailable
    }
    Binding {
        target: tipContainer
        property: "visible"
        value: true
        when: dekko.networkAvailable
    }

    Rectangle {
        id: shadow
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.top
        }
        visible: !opened
        height: units.gu(1)
        opacity: bottomPanel.pressed || bottomPanel.animating ? 1.0 : 0.0
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, 0.2) }
        }

    }

    //TODO: Fix the tip container animation. It's slightly out of sync with the panel trigger animation
    Item {
        id: tipContainer
        objectName: "bottomPanelTip"
        visible: !bottomPanel.opened
        width: childrenRect.width
        height: bottomPanel.tipHeight
        clip: true
        y: -bottomPanel.tipHeight
        anchors.horizontalCenter: parent.horizontalCenter
        Behavior on y {
            UbuntuNumberAnimation {
            }
        }

        states: [
            State {
                name: "expanded"
                PropertyChanges {
                    target: tipContainer
                    y: -tipHeight

                }
            },
            State {
                name: "collapsed"
                PropertyChanges {
                    target: tipContainer
                    y: -units.gu(1)
                }
            }
        ]

        UbuntuShape {
            id: tip
            width: tipLabel.paintedWidth + units.gu(6)
            height: bottomPanel.tipHeight + units.gu(1)
            color: Theme.palette.normal.overlay
            Label {
                id: tipLabel

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                height: bottomPanel.tipHeight
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
    Rectangle {
        anchors.fill: parent
        color: Theme.palette.normal.background
        property bool opened: bottomPanel.opened
        property bool animating: bottomPanel.animating
    }
    ListItems.ThinDivider {
        width: parent.width
        anchors.top: parent.top
    }
}
