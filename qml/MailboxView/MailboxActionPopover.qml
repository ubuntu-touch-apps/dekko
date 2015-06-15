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
import Ubuntu.Components.ListItems 1.0 as ListItems
import DekkoCore 0.2

ActionSelectionPopover {
    id: actionPopover
    property string mboxName

    MailboxUtils {
        id: mboxUtils
        account: dekko.currentAccount
    }

    delegate: ListItems.Standard {
        text: action.text
        onClicked: PopupUtils.close(actionPopover)
    }
    actions: ActionList {
        Action {
            id: expungeMbox
            text: qsTr("Expunge mailbox")
            onTriggered: {
                mboxUtils.expungeMailboxByName(mboxName)
            }
        }
        Action {
            id: markMboxRead
            text: qsTr("Mark mailbox read")
            onTriggered: {
                mboxUtils.markMailboxAsRead(mboxName)
            }
        }
    }

    InverseMouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: PopupUtils.close(actionPopover)
    }
}
