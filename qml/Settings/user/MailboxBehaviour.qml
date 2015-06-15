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
import DekkoCore 0.2
import "../common"
import "../../Components"

SettingsGroupBase {

    Component.onCompleted: account = dekko.accountsManager.getFromId(root.account.accountId)
    spacing: 0
    anchors.topMargin: units.gu(0)
    property Account account
    property alias mboxSettings: mailboxSettings

    MailboxSettings {
        id: mailboxSettings
        account: root.account
        // The settings configured from this view are
        // only saved on the global object.
        mailboxName: "global"
    }

    Behavior on height {
        UbuntuNumberAnimation{}
    }
    SectionDivider {
        // So we can at least get the strings translated
        // let's hide the ones we do not have implementations for yet
        visible: false
        text: qsTr("Archiving")
    }
    ListItem.Standard {
        visible: false
        text: qsTr("Enable archiving")
        control: DekkoSwitch {
            onClicked: mboxSettings.write(mboxSettings.keys.archiveEnabled, checked)
            Component.onCompleted: checked = mboxSettings.data.archive.enabled
        }
    }
    Selector {
        visible: false
        description: qsTr("Archive messages")
        selectedText: qsTr("In Archive folder")
    }

    Selector {
        visible: false
        description: qsTr("Archive format")
        selectedText: qsTr("Monthly")
    }

    SectionDivider {
        text: qsTr("Composing messages")
    }

    Selector {
        id: sentSelector
        description: qsTr("Save sent messages")
        contents: MboxSelector {
            id: sentMboxSelector
            disableString: qsTr("Never")
            selectedString: mboxSettings.data.sentMailbox ? qsTr("In %1 folder").arg(mboxSettings.data.sentMailbox) : qsTr("Select folder...")
            onSelectedStringChanged: sentSelector.selectedText = selectedString
            selectAction: Action {
                onTriggered: mboxPicker.pick()
            }
            onSelectedIndexChanged: {
                updateSelectedString()
                if (selectedIndex === 1 && mboxSettings.data.sentMailbox) {
                    mboxSettings.write(mboxSettings.keys.saveToImap, true)
                } else {
                    mboxSettings.write(mboxSettings.keys.saveToImap, false)
                }
            }
            function updateSelectedString() {
                switch(selectedIndex) {
                case 0:
                    sentSelector.selectedText = selectedText
                    break;
                case 1:
                    sentSelector.selectedText = selectedString
                    break;
                }
            }

            Component.onCompleted: {
                if (mboxSettings.submission.saveToImap && mboxSettings.data.sentMailbox) {
                    selectedIndex = 1
                } else {
                    selectedIndex = 0
                }
                updateSelectedTextString()
                updateSelectedString()
            }
        }

        MailboxPicker {
            id: mboxPicker
            account: root.account
            onMailboxSelected: {
                mboxSettings.write(mboxSettings.keys.sentMailbox, mboxName)
                sentSelector.expanded = false
                mboxSettings.write(mboxSettings.keys.saveToImap, true)
            }
        }
    }

    ListItem.Standard {
        text: qsTr("Attempt re-submit on fail")
        visible: false
        control: DekkoSwitch {
            onClicked: mboxSettings.write(mboxSettings.keys.allowRetries, checked)
            Component.onCompleted: checked = mboxSettings.submission.allowRetries
        }
    }
    ListItem.Standard {
        text: qsTr("Quote original message")
        control: DekkoSwitch {
            onClicked: mboxSettings.write(mboxSettings.keys.quoteOriginalMessage, checked)
            Component.onCompleted: checked = mboxSettings.submission.quoteOriginalMessage
        }
    }
    Selector {
        id: quoteSelector
        visible: mboxSettings.submission.quoteOriginalMessage
        description: qsTr("Start reply")
        selectedText: qsTr("Above quoted message")
        contents: QuoteSelector {
            onSelectedTextChanged: {
                quoteSelector.selectedText = selectedText
            }
        }
    }
    Selector {
        id: sigSelector
        description: qsTr("Include signature")
        contents: SignatureSelector {
            onSelectedTextChanged: {
                sigSelector.selectedText = selectedText
            }
        }
    }
    Selector {
        id: includeAddressesSelector
        description: qsTr("Include addresses")
        selectedText: qsTr("added to every message")
        contents: Item {
            anchors {left: parent.left; right: parent.right}
            height: c.height
            Column {
                id: c
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                spacing: units.gu(1)

                TitledTextField {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    title: qsTr("Cc these addresses")
                    onTextChanged: {
                        if (text) {
                            mboxSettings.write(mboxSettings.keys.includeCC, true)
                        } else {
                            mboxSettings.write(mboxSettings.keys.includeCC, false)
                        }
                        mboxSettings.write(mboxSettings.keys.cc, text)
                    }

                    Component.onCompleted: {
                        if (mboxSettings.submission.includeCC) {
                            text = mboxSettings.submission.cc
                        }
                    }
                }
                TitledTextField {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    title: qsTr("Bcc these addresses")
                    onTextChanged: {
                        if (text) {
                            mboxSettings.write(mboxSettings.keys.includeBCC, true)
                        } else {
                            mboxSettings.write(mboxSettings.keys.includeBCC, false)
                        }
                        mboxSettings.write(mboxSettings.keys.bcc, text)
                    }
                    Component.onCompleted: {
                        if (mboxSettings.submission.includeBCC) {
                            text = mboxSettings.submission.bcc
                        }
                    }
                }

                Label {
                    text: qsTr("Use comma separated values")
                    fontSize: "small"
                }
            }
        }
    }

    SectionDivider {
        text: qsTr("Drafts")
        visible: false
    }

    Selector {
        id: draftsSelector
        visible: false
        description: qsTr("Save drafts")
        selectedText: qsTr("In %1").arg(mboxSettings.data.draftsMailbox)
    }

    SectionDivider {
        text: qsTr("Deleting messages")
    }

    Selector {
        id: deletedSelector
        description: qsTr("Move deleted messages")
        contents: MboxSelector {
            id: delSentMboxSelector
            disableString: qsTr("Never")
            selectedString: mboxSettings.data.trashMailbox ? qsTr("To %1 folder").arg(mboxSettings.data.trashMailbox) : qsTr("Select folder...")
            onSelectedStringChanged: deletedSelector.selectedText = selectedString
            selectAction: Action {
                onTriggered: mboxPicker2.pick()
            }
            onSelectedIndexChanged: {
                updateSelectedString()
                if (selectedIndex === 1 && mboxSettings.data.trashMailbox) {
                    mboxSettings.write(mboxSettings.keys.moveToTrash, true)
                } else {
                    mboxSettings.write(mboxSettings.keys.moveToTrash, false)
                }
            }
            function updateSelectedString() {
                switch(selectedIndex) {
                case 0:
                    deletedSelector.selectedText = selectedText
                    break;
                case 1:
                    deletedSelector.selectedText = selectedString
                    break;
                }
            }

            Component.onCompleted: {
                if (mboxSettings.deleted.moveToTrash && mboxSettings.data.trashMailbox) {
                    selectedIndex = 1
                } else {
                    selectedIndex = 0
                }
                updateSelectedTextString()
                updateSelectedString()
            }
        }
        MailboxPicker {
            id: mboxPicker2
            account: root.account
            onMailboxSelected: {
                mboxSettings.write(mboxSettings.keys.trashMailbox, mboxName)
                mboxSettings.write(mboxSettings.keys.moveToTrash, true)
                deletedSelector.expanded = false
            }
        }
    }

    Selector {
        id: permDeleteSelector
        visible: false
        description: qsTr("Permanently delete messages")
        selectedText: qsTr("After %1 days")
    }
    ListItem.Standard {
        text: qsTr("Mark as read when deleted")
        control: DekkoSwitch {
            onClicked: mboxSettings.write(mboxSettings.keys.markRead, checked)
            Component.onCompleted: checked = mboxSettings.deleted.markRead
        }
    }
    ListItem.Standard {
        text: qsTr("Never delete starred messages")
        visible: false
        control: DekkoSwitch {
            onClicked: mboxSettings.write(mboxSettings.keys.preserveFlagged, checked)
            Component.onCompleted: checked = mboxSettings.deleted.preserveFlagged
        }
    }
}
