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
import Ubuntu.Components.Themes.Ambiance 1.0
import DekkoCore 0.2
import "../Components"

Column {
    id: column

    property alias name: nameField.text
    property alias email: emailField.text
    property alias password: passwordField.text
    property alias description: descriptionField.text
    property alias company: companyField.text

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
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: emailField.textFieldFocusHandle
        font.capitalization: Font.Capitalize
        placeholderText: qsTr("Eg: \"John Smith\"")
        Component.onCompleted: textFieldFocusHandle.focus = true
    }

    TitledTextField {
        id: emailField
        title: qsTr("Email address")
        requiredField: internal.invalidFields
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        placeholderText: qsTr("Eg: \"johnsmith@example.com\"")
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: passwordField.textFieldFocusHandle
        onActiveFocusChanged: {
            if (!activeFocus) {
                setupWalkThrough.startAutoConfig();
                accountSetup.verifyImapSettingsIfComplete();
            }
        }
        secondaryItem: Loader {
            width: sourceComponent == null ? 0 : units.gu(2)
            height: width
            sourceComponent: (imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_SUCCEED ? authenticationSucceedIcon :
                                 (imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_FAILED ? authenticationFailedIcon : null))
        }
    }

    TitledTextField {
        id: passwordField
        title: qsTr("Password")
        sendTabEventOnEnter: true
        inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        echoMode: showPassword.checked ? TextInput.Normal : TextInput.Password
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: descriptionField.textFieldFocusHandle
        onActiveFocusChanged: {
            if (!activeFocus) {
                accountSetup.verifyImapSettingsIfComplete();
            }
        }
        secondaryItem: Loader {
            width: sourceComponent == null ? 0 : units.gu(2)
            height: width
            sourceComponent: (imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_SUCCEED ? authenticationSucceedIcon :
                                    (imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_FAILED ? authenticationFailedIcon : null))
        }
    }

    CheckboxWithLabel {
        id: showPassword
        anchors {
            left:parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        text: qsTr("Show password")
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
    }

    Component {
        id: authenticationSucceedIcon
        Icon {
            name: "ok"
            color: UbuntuColors.green
        }
    }
    Component {
        id: authenticationFailedIcon
        Icon {
            name: "close"
            color: UbuntuColors.red
        }
    }
}
