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
import Ubuntu.Components.Popups 1.0
import "../Dialogs"
import "../Components"
import DekkoCore 0.2

Page {
    id: imapServerInfo
    flickable: null

    DekkoHeader {
        id: header
        title: qsTr("IMAP Capabilities")
        backAction: bkAction
    }

    Action {
        id: bkAction
        iconName: "back"
        onTriggered: {
            account.imapSettings.save()
            pageStack.pop()
        }
    }

    // This is just a really simple list view that displays the capabilities of the user's IMAP server
    // This will come in handy when looking at bug reports related to certain server's not
    // supporting specific extensions or is a braindead server. Wink Wink gmail :-P
    // We can ask the user to look at the supported capabilities during bug analysis.

    property Account account

    UbuntuListView {
        id: serverList
        anchors {
            left: parent.left
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
        }

        clip: true
        header: ListItem.Header { text: qsTr("Click on a capability to disable it") }
        visible: account.imapModel
        model: account.unfilteredCapabilities()
        delegate: ListItem.Standard {
            Component.onCompleted: if (!modelData) { height = 0 }
            text: modelData
            control: Switch {
                checked: !account.isExtensionBlackListed(modelData)
                onCheckedChanged: {
                    if (checked) {
                        account.removeExtensionFromImapBlackList(modelData)
                    } else {
                        account.appendExtensionToImapBlacklist(modelData)
                    }
                }
            }
        }
    }
}
