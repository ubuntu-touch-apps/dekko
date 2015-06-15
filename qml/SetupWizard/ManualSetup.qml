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
import Ubuntu.Components.ListItems 1.0
import DekkoCore 0.2
import "../Components"

Column {
    id: column

    anchors {
        left: parent.left
        top: parent.top
        right: parent.right
    }

    property alias name: nameField.text
    property alias description: descriptionField.text
    property alias email: emailField.text
    property alias company: companyField.text
    property alias imapEncryptionMethod: imapEncryptionSelector.method
    property alias imapServer: imapServerField.text
    property string imapPort: imapPortField.text
    property alias imapUsername: imapEmailField.text
    property alias imapPassword: imapPasswordField.text
    property alias smtpEncryptionMethod: smtpEncryptionSelector.method
    property alias smtpServer: smtpServerField.text
    property alias smtpPort: smtpPortField.text
    property alias smtpUsername: smtpEmailField.text
    property alias smtpPassword: smtpPasswordField.text

    onVisibleChanged: {
        if (visible) {
            nameField.forceActiveFocus()
            nameField.textFieldFocusHandle.focus = true
        }
    }

    TitledTextField {
        id: nameField
        title: qsTr("Name")
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        placeholderText: qsTr("Eg: \"John Smith\"")
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: emailField.textFieldFocusHandle
        font.capitalization: Font.Capitalize
    }

    TitledTextField {
        id: emailField
        title: qsTr("Email")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        placeholderText: qsTr("Eg: \"johnsmith@example.com\"")
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: descriptionField.textFieldFocusHandle
    }

    TitledTextField {
        id: descriptionField
        title: qsTr("Description")
        sendTabEventOnEnter: true
        placeholderText: qsTr("Eg: \"Personal\" or \"Work\"")
        font.capitalization: Font.Capitalize
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: companyField.textFieldFocusHandle
    }

    TitledTextField {
        id: companyField
        title: qsTr("Company (optional)")
        sendTabEventOnEnter: true
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapServerField.textFieldFocusHandle
    }

    SectionDivider {
        text: qsTr("Incoming Mail Server")
        showDivider: false
    }

    Item {
        width: parent.width
        height: col1.height
        Column {
            id: col1
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            Rectangle {
                width: parent.width
                height: units.dp(2)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.1) }
                    GradientStop { position: 0.49; color: Qt.rgba(0, 0, 0, 0.1) }
                    GradientStop { position: 0.5; color: Qt.rgba(1, 1, 1, 0.4) }
                    GradientStop { position: 1.0; color: Qt.rgba(1, 1, 1, 0.4) }
                }
            }
            EncryptionSelector {
                id: imapEncryptionSelector
                type: "imap"
                anchors {
                    left: parent.left
                    right: parent.right
                }
                onPortChanged: imapPortField.text = port
            }
        }
    }

    TitledTextField {
        id: imapServerField
        title: qsTr("Host name")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        placeholderText: qsTr("Eg: \"imap.example.com\"")
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapPortField.textFieldFocusHandle
    }

    TitledTextField {
        id: imapPortField
        title: qsTr("Port")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapEmailField.textFieldFocusHandle
    }

    TitledTextField {
        id: imapEmailField
        title: qsTr("Username")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapPasswordField.textFieldFocusHandle
    }

    TitledTextField {
        id: imapPasswordField
        title: qsTr("Password")
        sendTabEventOnEnter: true
        inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        echoMode: showImapPassword.checked ? TextInput.Normal : TextInput.Password
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpServerField.textFieldFocusHandle
    }

    CheckboxWithLabel {
        id: showImapPassword
        text: qsTr("Show password")
        anchors {
            left:parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
    }

    SectionDivider {
        text: qsTr("Outgoing Mail Server")
        showDivider: false
        anchors {}
    }

    Item {
        width: parent.width
        height: col.height
        Column {
            id: col
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            Rectangle {
                width: parent.width
                height: units.dp(2)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.1) }
                    GradientStop { position: 0.49; color: Qt.rgba(0, 0, 0, 0.1) }
                    GradientStop { position: 0.5; color: Qt.rgba(1, 1, 1, 0.4) }
                    GradientStop { position: 1.0; color: Qt.rgba(1, 1, 1, 0.4) }
                }
            }
            EncryptionSelector {
                id: smtpEncryptionSelector
                type: "smtp"
                anchors {
                    left: parent.left
                    right: parent.right
                }
                onPortChanged: smtpPortField.text = port
            }
        }
    }

    TitledTextField {
        id: smtpServerField
        title: qsTr("Host name")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        placeholderText: qsTr("Eg \"smtp.example.com\"")
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpPortField.textFieldFocusHandle
    }

    TitledTextField {
        id: smtpPortField
        title: qsTr("Port")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpEmailField.textFieldFocusHandle
    }

    TitledTextField {
        id: smtpEmailField
        title: qsTr("Username")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: internal.invalidFields
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpPasswordField.textFieldFocusHandle
    }

    TitledTextField {
        id: smtpPasswordField
        title: qsTr("Password")
        sendTabEventOnEnter: true
        inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        echoMode: showSmtpPassword.checked ? TextInput.Normal : TextInput.Password
        KeyNavigation.priority: KeyNavigation.BeforeItem
    }

    CheckboxWithLabel {
        id: showSmtpPassword
        text: qsTr("Show password")
        anchors {
            left:parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
    }
}
