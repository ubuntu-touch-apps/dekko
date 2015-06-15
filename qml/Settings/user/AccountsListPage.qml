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
import Ubuntu.Components.Popups 1.0
import DekkoCore 0.2
import "../../AccountsView"
import "../common"
import "../../Components"


Page {
    id: root

    flickable: null

    DekkoHeader {
        id: header
        title: qsTr("Account Settings")
        backAction: bkAction
//        primaryAction: Action {
//            text: qsTr("Delete")
//            iconName: "delete"
//            visible: root.state === "accounts"
//            onTriggered: dekko.accountsManager.removeRow(dekko.accountsManager.indexFromAccountId(account.accountId))
//        }
    }
    SenderIdentityModel {
        id: identityModel
        accountId: root.account.accountId
        onCountChanged: saveIdentites()
    }
    property Account account
    property alias senderIdentityModel: identityModel
    // accountslistview emits this
    signal openAccount(int index)



    onOpenAccount: {
        account = dekko.accountsManager.get(index)
        state = "groups"
    }

    // On triggered, confirm dialog settings changed
    Action {
        id: bkAction
        iconName: "back"
        onTriggered: {
            if (state === "settings") {
                if (settingsView.accountSettingsChanged) {
                    // We need to confirm settings changed should be saved
                    var confirmDialog = PopupUtils.open(Qt.resolvedUrl("../../Dialogs/ConfirmationDialog.qml"), root, {title: qsTr("Save Changes?")})
                    confirmDialog.confirmClicked.connect(saveConfirmed)
                    confirmDialog.cancelClicked.connect(proceedToPreviousState)
                } else {
                    root.proceedToPreviousState();
                }
            } else {
                root.proceedToPreviousState()
            }
        }
    }

    function saveConfirmed() {
        settingsView.save()
    }

    function proceedToPreviousState() {
        if (state === "settings") {
            state = "accounts"
            settingsView.accountSettingsChanged = false
            settingsView.destroyChildren()
        } else if (state === "groups") {
            state = "accounts"
        } else {
            pageStack.pop()
        }
    }

    state: "accounts"
    states: [
        State {
            name: "accounts"
            PropertyChanges {
                target: header
                title: qsTr("Account Settings")
            }
        },
        State {
            name: "groups"
            PropertyChanges {
                target: header
                title: qsTr("Account Settings")
            }
        },
        State {
            name: "settings"
        }
    ]

    // Step 1 choose account
    ListView {
        id: accountsListView
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: root.state === "accounts"
        model: AccountListModel{
            function openView(description, componentPath) {
                settingsView.groupTitle = description
                settingsView.createSettingsObject(Qt.resolvedUrl(componentPath))
            }
            onOpenImapSettings: openView(qsTr("Incoming server"), "ServerSettings.qml")
            onOpenSmtpSettings: openView(qsTr("Outgoing server"), "OutgoingServerSettings.qml")
            onOpenAccountDescription: openView(qsTr("Account description"), "ProfileSettingsPage.qml")
            onOpenMailboxBehaviour: openView(qsTr("Mailbox behaviour"), "MailboxBehaviour.qml")
        }
    }

    //Step 2 edit settings
    SettingsGroupView {
        id: settingsView
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: root.state === "settings"
        property bool accountSettingsChanged: false
        property string groupTitle: ""
        onSettingsChanged: accountSettingsChanged = changed
        onComponentLoaded: {
            root.state = "settings"
            header.title = groupTitle
        }
        onSaveComplete:root.proceedToPreviousState()
        onError: console.log(error)
    }
}
