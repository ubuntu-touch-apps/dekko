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
import Ubuntu.Components.Popups 1.0
import DekkoCore 0.2
import "../Components"

Dialog {
    id: root

    property int uid

    function moveMessage() {
        if (mailboxUtils.moveMessageToMailbox(dekko.currentAccount.msgListModel.mailboxName, uid, mboxSelector.selectedMailbox)) {
            PopupUtils.close(root)
        } else {
            root.title = qsTr("Failed")
            root.text = qsTr("Unable to move message")
            mboxSelector.visible = false
            mvBtn.visible = false
            cancelBtn.width = parent.width
            cancelBtn.text = qsTr("Close")
        }
    }

    MailboxUtils {
        id: mailboxUtils
        account: dekko.currentAccount
    }

    title: qsTr("Move message.")
    text: qsTr("Select the mailbox to move message to")
    contents: [
        MailboxSelector {
            id: mboxSelector
        },
        Row {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: units.gu(1)
            Button {
                id: cancelBtn
                text: qsTr("Cancel")
                color: UbuntuColors.red
                width: parent.width / 2 - units.gu(0.5)
                onClicked: PopupUtils.close(root)
            }
            Button {
                id: mvBtn
                text: qsTr("Move")
                color: UbuntuColors.green
                width: parent.width / 2 - units.gu(0.5)
                onClicked: moveMessage()
            }
        }
    ]
}
