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

DialogBase {
    id: passwordDialog

    property string type
    property string accountId
    property alias authErrorMessage: authErrorMessage.text

    readonly property string username: type === "imap" ? imapSettings.username : smtpSettings.username

    title: qsTr("Password required")
    text: qsTr("Please provide password for user <b>%1</b>").arg(username)

    Action {
        id: setPwdAction
        onTriggered: {
            if (passwordInput.text) {
                passwordManager.setPassword(passwordInput.text)
                if (type === "imap")
                    dekko.accountsManager.setPassword(accountId, passwordInput.text)
                else
                    dekko.submissionManager.setPassword(passwordInput.text);
                //Reset our internal auth has failed property.
                // Todo: make this more elegant, i don't like this!!
                dekko.setAuthHasFailed(false)
                PopupUtils.close(passwordDialog)
            }
        }
    }

    Action {
        id: closeConnection
        onTriggered: {
            if (type === "imap")
                dekko.accountsManager.closeImapConnection(accountId)
            else
                dekko.submissionManager.cancelPassword()
            PopupUtils.close(passwordDialog)
        }
    }

    contents: [
        TextField {
            id: passwordInput
            inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            echoMode: showPassword.checked ? TextInput.Normal : TextInput.Password
            onAccepted: setPwdAction.trigger()
        },
        CheckboxWithLabel {
            id: showPassword
            text: qsTr("Show password")
        },

        Label {
            id: authErrorMessage
            wrapMode: TextEdit.Wrap
            visible: text
            anchors {
                left: parent.left
                right: parent.right
            }
        },
        Row {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: units.gu(1)
            Button {
                text: qsTr("Cancel")
                color: UbuntuColors.red
                width: parent.width / 2 - units.gu(0.5)
                onClicked: closeConnection.trigger()
            }
            Button {
                text: qsTr("Confirm")
                color: UbuntuColors.green
                width: parent.width / 2 - units.gu(0.5)
                onClicked: setPwdAction.trigger()
            }
        }
    ]

    ImapSettings {
        id: imapSettings
        accountId: type === "imap" ? passwordDialog.accountId : undefined
    }
    SmtpSettings {
        id: smtpSettings
        accountId: type === "smtp" ? passwordDialog.accountId : undefined
    }

    PasswordManager {
        id: passwordManager
        accountId: passwordDialog.accountId
        type: passwordDialog.type === "imap" ? PasswordManager.IMAP : PasswordManager.SMTP
    }
}
