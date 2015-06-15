/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Touch/

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
import "../Components"


ListItemWithActions {
    id: accountsDelegate

    //-----------------------------------
    // PROPERTIES
    //-----------------------------------
    property alias accountImagePath: accountsImage.source
    property string description
    property string organization
    property bool isFirst: false
    property bool active: false

    //----------------------------------
    // OBJECT PROPERTIES
    //----------------------------------
    height: units.gu(8)
    showDivider: false
    color: Theme.palette.normal.background
    triggerActionOnMouseRelease: true

    //--------------------------------
    // COMPONENTS / CHILD OBJECTS
    //-------------------------------
    Rectangle {
        anchors {
            fill: parent
            topMargin: units.gu(-1)
            bottomMargin: units.gu(-1)
            leftMargin: units.gu(-2)
            rightMargin: units.gu(-2)
        }
        opacity: accountsDelegate.active ? 0.2 : 0.0
        color: "black"
        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: units.gu(0.2)
        Icon {
            id: accountsImage
            height: units.gu(4); width: height
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            color: Style.common.text
        }

        Label {
            id: nameLabel
            anchors {
                left: accountsImage.right
                leftMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
                right: progression.left
            }
            text: description
            fontSize: "medium"
            font.weight: Font.DemiBold
            clip: true
            color: Style.common.text
        }
        // TODO: replace with INBOX count once we have the Inbox model on the Account object
//        Icon {
//            id: progression
//            anchors {
//                right: parent.right
//                top: parent.top
//                topMargin: units.gu(1.5)
//                bottom: parent.bottom
//                bottomMargin: units.gu(1.5)
//            }
//            width: units.gu(2)
//            name: "go-next"
//        }
    }
    //----------------------------------
    // TRANSITIONS
    //----------------------------------
    transitions: [
        Transition {
            UbuntuNumberAnimation {
                properties: "opacity"
            }
        }
    ]
}
