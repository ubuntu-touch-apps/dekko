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
import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import "../Components"
import DekkoCore 0.2

Page {
    id: devSettings
    flickable: null

    DekkoHeader {
        id: header
        title: "Developer settings"
        backAction: bkAction
    }

    Action {
        id: bkAction
        iconName: "back"
        onTriggered: {
            pageStack.pop()
        }
    }

    Column {
        anchors {
            left: parent.left
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
        }

        anchors.margins: units.gu(1)

        ListItem.Standard {
            text: "Enable notifications (TweakGeek required)"
            control: Switch {
                checked: GlobalSettings.developer.enableNotifications
                onCheckedChanged: GlobalSettings.developer.enableNotifications = checked
            }
        }

//        ListItem.Standard {
//            text: "Enable Imap logging"
//            control: Switch {
//                checked: currentAccount.devModeImapModelLogging
//                onCheckedChanged: currentAccount.devModeImapModelLogging = checked
//            }
//        }

        ListItem.Standard {
            text: "Nuke current accounts cache"
            control: Button {
                text: "Do It!!"
                color: UbuntuColors.green
                onClicked: dekko.currentAccount.nukeCurrentCache()
            }
        }

//        ListItem.Standard {
//            text: "Enable Threading"
//            control: Switch {
//                checked: currentAccount.devModeEnableThreading
//                onCheckedChanged: currentAccount.devModeEnableThreading = checked
//            }
//        }

//        ListItem.Standard {
//            text: "Enable Contacts"
//            control: Switch {
//                checked: currentAccount.devModeEnableContacts
//                onCheckedChanged: currentAccount.devModeEnableContacts = checked
//            }
//        }
    }
}
