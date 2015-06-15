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
import "../Components"
import DekkoCore 0.2


ListItemWithActions {
    id: mailboxDelegate

    property bool isFirst: false
    property bool active: false
    showDivider: true
    height: units.gu(7)
    color: Theme.palette.normal.background
    triggerActionOnMouseRelease: true
    property int defaultIndentationMargin: model.nestingDepth < 4 ? units.gu((model.nestingDepth - 1) * 5) : units.gu(3 * 3)

    function getIconSourceForMailbox() {
        if (mailboxIsINBOX) {
            return "qrc:///actions/inbox.svg"
        } else if (isGmailImportantSpecialUse) {
            return "qrc:///actions/mail-mark-important03.svg"
        } else if (mailboxIsSpecialUseALL) {
            return "qrc:///actions/mail-unread.svg"
        } else if (mailboxIsSpecialUseFLAGGED) {
            return "qrc:///actions/starred.svg"
        } else if (mailboxIsSpecialUseSENT || mailboxIsSENT) {
            return "qrc:///actions/send.svg"
        } else if (mailboxIsSpecialUseTRASH || mailboxIsTRASH) {
            return "qrc:///actions/edit-delete.svg"
        } else if (mailboxIsSpecialUseJUNK || mailboxIsJUNK) {
            return "qrc:///actions/junk.svg"
        } else if (mailboxIsSpecialUseDRAFTS || mailboxIsDRAFTS) {
            return "qrc:///actions/draft.svg"
        } else {
            return "qrc:///actions/inbox.svg"
        }
    }

    function getIconColorForMailbox() {
        if (isGmailImportantSpecialUse) {
            return Style.actions.important
        } else if (mailboxIsSpecialUseFLAGGED) {
            return Style.actions.starred
        } else {
            // Match the text color
            return Style.common.text
        }
    }

    transitions: [
        Transition {
            UbuntuNumberAnimation {
                properties: "opacity"
            }
        }
    ]

//    Rectangle {
//        anchors {
//            fill: parent
//            topMargin: units.gu(-1)
//            bottomMargin: units.gu(-1)
//            leftMargin: units.gu(-2)
//            rightMargin: units.gu(-2)
//        }
//        opacity: mailboxDelegate.active ? 0.2 : 0.0
//        color: "black"
//        Behavior on opacity {
//            UbuntuNumberAnimation {}
//        }
//    }

    Item {
        anchors.fill: parent
        anchors.margins: units.gu(0.2)
        Icon {
            id: accountsImage
            height: units.gu(3); width: height
            anchors {
                left: parent.left
                // This is where we determine the nesting depth
                // Any nesting up to level 3 will get indented and anything greater than 3 will be flattened to level 3
                leftMargin: mboxListView.isSearchMode ? 0 : defaultIndentationMargin
//                top: parent.top
                verticalCenter: parent.verticalCenter
            }
            color: getIconColorForMailbox()
            source: getIconSourceForMailbox()
        }

        Label {
            id: nameLabel
            anchors {
                left: accountsImage.right
                leftMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
                right: infoText.left
            }
            property string mboxName: mailboxIsINBOX ? qsTr("Inbox") : shortMailboxName
            property string searchMboxName : mailboxIsINBOX ? qsTr("Inbox") : mailboxName
            // Firstly if this is INBOX let's make it a more normalized name rather than all uppercase i.e "Inbox"
            // TODO: talk with design about what the mailboxname should look like for nesting deeper than level 3??
            text: mboxListView.isSearchMode ? searchMboxName : mboxName
            color: Style.common.text
            fontSize: "large"
            clip: true
        }

        Label {
            id: infoText
            text: unreadMessageCount && !isPickMode ? unreadMessageCount : ""
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            fontSize: "medium"
            font.weight: Font.DemiBold
            color: Style.common.text
        }
    }
}
