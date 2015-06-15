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


ListView {
    id: mailboxListView
    anchors.fill: parent
    clip: true

    property bool lockDelegate: false
    property bool isPickMode: false

    property var _currentSwipedItem: null

    signal openMailbox(string mboxName, string shortMboxName)

    function resetSwipe() {
        if (_currentSwipedItem) {
            _currentSwipedItem.resetSwipe()
            _currentSwipedItem = null
        }
    }

    function _updateSwipeState(item) {
        if (item.swipping) {
            return
        }

        if (item.swipeState !== "Normal") {
            if (_currentSwipedItem !== item) {
                if (_currentSwipedItem) {
                    _currentSwipedItem.resetSwipe()
                }
                _currentSwipedItem = item
            }
        } else if (item.swipeState !== "Normal" && _currentSwipedItem === item) {
            _currentSwipedItem = null
        }
    }

    delegate: MailboxListDelegate {
        id: delegate

        anchors {
            left: parent.left
            right: parent.right
        }

        property Action deleteAction: Action {
            text: qsTr("Delete")
            iconName: "delete"
            onTriggered: dekko.currentAccount.imapModel.deleteMailbox(mailboxName)
        }

        selected: mailboxListView.isSelected(delegate)
        selectionMode: mailboxListView.isInSelectionMode
        isFirst: model.index === 0
        active: ListView.isCurrentItem
        property bool shouldLock:  dekko.currentAccount.imapModel.isNetworkOnline ? !mailboxIsSelectable : true
        locked: lockDelegate ? lockDelegate : shouldLock
        onItemClicked: {
            if (mailboxIsSelectable || mailboxIsINBOX) {
                mailboxListView.openMailbox(mailboxName, shortMailboxName)
            }
        }

        onSwippingChanged: mailboxListView._updateSwipeState(delegate)
        onSwipeStateChanged: mailboxListView._updateSwipeState(delegate)

        // There is no point trying to delete an un-selectable mailbox as it is
        // most probably *virtual* and as far as i'm concerned I do not want to be
        // responsible for somone deleteing or attempting to delete INBOX and something
        // BAD happens.
        leftSideAction: (mailboxIsSelectable && !mailboxIsINBOX) ? deleteAction : null
        // TODO: mailbox actions
        rightSideActions: [
            Action {
                id: extras
                iconName: "add"
                onTriggered: PopupUtils.open(Qt.resolvedUrl("./MailboxActionPopover.qml"), delegate, {mboxName: mailboxName})

            }
        ]

    }
}
