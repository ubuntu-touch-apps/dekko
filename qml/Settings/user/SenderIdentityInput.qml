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
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../Composer"
import "../../Components"

Column {
    id: inputListView

    property alias nameField: nameField.text
    property alias emailField: emailField.text
    property alias organizationField: organizationField.text
    property alias signatureField: signatureField.text

    anchors {
        left: parent.left
        right: parent.right
        top: parent.top
        topMargin: units.gu(2)
    }

    spacing: units.gu(1)

    SenderIdentity {
        id: newIdentity
        useDefaultSignature: true
    }

    TitledTextField {
        id: nameField
        title: qsTr("Name")
        text: imapSettings.server
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        KeyNavigation.tab: emailField.textFieldFocusHandle
    }
    TitledTextField {
        id: emailField
        title: qsTr("Email Address")
        text: imapSettings.server
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: organizationField.textFieldFocusHandle
    }
    TitledTextField {
        id: organizationField
        title: qsTr("Organization")
        text: imapSettings.server
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: signatureField.textFieldFocusHandle
    }

    TitledTextArea {
        id: signatureField
        title: qsTr("Signature")
        text: newIdentity.signature
    }
}
