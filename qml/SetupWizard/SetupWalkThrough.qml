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
import DekkoCore 0.2
import "../Components"

Flickable {
    id: setupWalkThrough
    contentHeight: __isBasic ? basicSetup.height + units.gu(5) : manualSetup.height + units.gu(5)

    property string name: __isBasic ? basicSetup.name : manualSetup.name
    property string email: __isBasic ? basicSetup.email : manualSetup.email
    property string description: __isBasic ? basicSetup.description : manualSetup.description
    property string company: __isBasic ? basicSetup.company : manualSetup.company
    property string imapPassword: __isBasic ? basicSetup.password : manualSetup.imapPassword
    property string smtpPassword: __isBasic ? basicSetup.password : manualSetup.smtpPassword
    property var imapEncryptionMethod: manualSetup.imapEncryptionMethod
    property string imapServer: manualSetup.imapServer
    property string imapPort: parseInt(manualSetup.imapPort)
    property string imapUsername: __isBasic ? basicSetup.email : manualSetup.imapUsername
    property var smtpEncryptionMethod: manualSetup.smtpEncryptionMethod
    property string smtpServer: manualSetup.smtpServer
    property int smtpPort: parseInt(manualSetup.smtpPort)
    property string smtpUsername: __isBasic ? basicSetup.email : manualSetup.smtpUsername

    readonly property bool __isBasic: state === "basic"

    signal startAutoConfig()
    // This should be emitted when AutoConfig on the basic setup has failed
    // This will move all info we currently have to the manualSetup view
    signal basicSetupFailed()
    onBasicSetupFailed: state = "extended"

    signal showInvalidFields()
    // Only show the invalid fields after a failed "Save" event
    // The *requiredField* property will pick this up and display red text to highlight
    // the required text until something has been entered.
    onShowInvalidFields: {
        internal.invalidFields = true
    }

    state: "basic"

    states: [
        State {
            name: "basic"
        },
        State {
            name: "extended"
            PropertyChanges {
                target: manualSetup
                email: basicSetup.email
                name: basicSetup.name
                company: basicSetup.company
                description: basicSetup.description
                imapPassword: basicSetup.password
                smtpPassword: basicSetup.password
            }
        }
    ]

    QtObject {
        id: internal
        property bool invalidFields: false
    }

    BasicSetup {
        id: basicSetup
        spacing: units.gu(1)
        anchors {
            left: parent.left
            top: parent.top
            topMargin: units.gu(2)
            right: parent.right
        }
        visible: setupWalkThrough.state === "basic"
    }

    ManualSetup {
        id: manualSetup
        spacing: units.gu(1)
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            topMargin: units.gu(2)
        }
        visible: setupWalkThrough.state === "extended"
    }
}
