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

Selector {
    id: encryptionSelector

    property string type
    property int port
    property var method
    // TODO: make this *safer*, what about an ENUM for 'type'??
    onTypeChanged: type === "imap" ? encryptionMethodModel.loadImapModel() : encryptionMethodModel.loadSmtpModel()



    description: qsTr("Secure Connection")

    contents: Item {
        id: radioSelector
        anchors {left: parent.left; right: parent.right}
        height: radioColumn.height
        property int selectedIndex: 0
        property alias model: repeater.model
        onSelectedIndexChanged: updateValues()
        function updateValues() {
            method = model.get(selectedIndex).method
            port = model.get(selectedIndex).port
            selectedText = model.get(selectedIndex).description
        }
        Column {
            id: radioColumn
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }

            spacing: units.gu(1)
            Repeater {
                id: repeater
                model: encryptionMethodModel
                delegate: CheckboxWithLabel {
                    id: delegate
                    text: description
                    property bool isSelected: selectedIndex === model.index
                    checked: selectedIndex === model.index
                    onCheckedChanged: if (checked) {selectedIndex = model.index}
                                      else if (selectedIndex === model.index) {checked = true;}
                    Connections {
                        target: radioSelector
                        onSelectedIndexChanged: {
                            delegate.checked = delegate.isSelected
                        }
                    }
                }
            }
        }
        Component.onCompleted: {
            if (type === "imap") {
                switch (imapSettings.connectionMethod) {
                case ImapSettings.NONE:
                    selectedIndex = 0
                    break
                case ImapSettings.STARTTLS:
                    selectedIndex = 1
                    break
                case ImapSettings.SSL:
                    selectedIndex = 2
                    break
                }
            } else {
                switch (smtpSettings.submissionMethod) {
                case SmtpSettings.SMTP:
                    selectedIndex = 0
                    break
                case SmtpSettings.SMTP_STARTTLS:
                    selectedIndex = 1
                    break
                case SmtpSettings.SSMTP:
                    selectedIndex = 2
                    break
                }
            }
            updateValues()
        }
    }


    ListModel {
        id: encryptionMethodModel

        function loadImapModel() {
            encryptionMethodModel.append({ "description": qsTr("No encryption"), "method": ImapSettings.NONE, "port": ImapSettings.PORT_IMAP })
            encryptionMethodModel.append({ "description": qsTr("Use encryption (STARTTLS)"), "method": ImapSettings.STARTTLS, "port": ImapSettings.PORT_IMAP })
            encryptionMethodModel.append({ "description": qsTr("Force encryption (SSL/TLS)"), "method": ImapSettings.SSL, "port": ImapSettings.PORT_IMAPS })
        }

        function loadSmtpModel() {
            encryptionMethodModel.append({ "description": qsTr("No encryption"), "method": SmtpSettings.SMTP, "port": SmtpSettings.PORT_SMTP_SUBMISSION })
            encryptionMethodModel.append({ "description": qsTr("Use encryption (STARTTLS)"), "method": SmtpSettings.SMTP_STARTTLS, "port": SmtpSettings.PORT_SMTP_SUBMISSION })
            encryptionMethodModel.append({ "description": qsTr("Force encryption (SSL/TLS)"), "method": SmtpSettings.SSMTP, "port": SmtpSettings.PORT_SMTP_SSL })
        }
    }
}

