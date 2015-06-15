/* Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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
import QtContacts 5.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import "../Composer"
import "../Components"

Page {
    id: root

    property var contact;
    property string prettyEmailAddress

    signal contactUpdated(var contact)

    flickable: null
    title: ""

    Component.onCompleted: {
        if (contact) {
            firstNameField.text = contact.firstName
            surnameField.text = contact.lastName
            organizationField.text = contact.organization
            var emails = contact.emails;
            emailModel.clear();
            for (var i in emails) {
                emailModel.append({email: emails[i]})
            }
        } else {
            firstNameField.textFieldFocusHandle.forceActiveFocus();
        }
    }

    ListModel {
        id: emailModel
        ListElement {
            email: ""
        }
    }

    DekkoHeader {
        id: header
        width: parent.width
        title: contact ? qsTr("Edit contact") : qsTr("New contact")
        primaryAction: Action {
            iconName: "ok"
            onTriggered: {
                if (firstNameField.text == "") {
                    PopupUtils.open(Qt.resolvedoldContactUrl("../Dialogs/InfoDialog.qml"), root, {
                                        title: qsTr("Error creating contact"),
                                        text: qsTr("Please fill in the name of the contact.")
                                    })
                    return;
                }

                var emails = [];
                for (var i = 0; i < emailModel.count; i++) {
                    if (emailModel.get(i).email.length > 0)
                        emails.push(emailModel.get(i).email);
                }
                if (contact) {
                    contact = {
                        id: contact.contactId,
                        firstName: firstNameField.text,
                        lastName: surnameField.text,
                        organization: organizationField.text,
                        emails: emails
                    };
                    addressbookModel.updateContact(contact);
                    root.contactUpdated(contact);
                } else {
                    addressbookModel.newContact(firstNameField.text, surnameField.text, organizationField.text, emails);
                    addressbookModelFiltered.sortByTag();
                }
                pageStack.pop();
            }
        }
        backAction: Action {
            iconName: "back"
            onTriggered: {
                pageStack.pop();
            }
        }
    }

    Flickable {
        id: scrollArea
        clip: true
        anchors {
            top: header.bottom
            bottom: parent.bottom
        }
        width: parent.width
        flickableDirection: Flickable.VerticalFlick
        contentHeight: contents.height + units.gu(2)
        contentWidth: parent.width
        Column {
            id: contents
            spacing: units.gu(1)
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: units.gu(1)
            }

            TitledTextField {
                id: firstNameField
                title: qsTr("First name")
            }

            TitledTextField {
                id: surnameField
                title: qsTr("Surname")
            }

            TitledTextField {
                id: organizationField
                title: qsTr("Organization")
            }

            Repeater {
                model: emailModel
                delegate: Item {
                    height: emailField.height
                    width: parent.width
                    TitledTextField { //TODO make visible when keyboard shows up
                        id: emailField
                        title: qsTr("Email") + " " + (index + 1)
                        text: emailModel.get(index).email
                        anchors {
                            left: parent.left
                            right: deleteEmailIcon.left
                        }

                        onTextChanged: emailModel.set(index, { email: text });

                    }
                    Icon {
                        id: deleteEmailIcon
                        width: units.gu(3)
                        height: width
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                            rightMargin: units.gu(2)
                            bottomMargin: units.gu(1)
                        }
                        name: "delete"
                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -units.gu(1)
                            onClicked: emailModel.remove(index, 1);
                            Rectangle {
                                anchors.fill: parent
                                color: parent.pressed ? "black" : "transparent"
                                opacity: 0.1
                            }
                        }
                    }
                }
            }

            ListItem.Standard {
                onClicked: emailModel.append({email: ""})
                showDivider: false
                Row {
                    height: parent.height
                    anchors {
                        left: parent.left
                        leftMargin: units.gu(2)
                    }
                    spacing: units.gu(2)
                    Icon {
                        width: units.gu(3)
                        height: width
                        anchors.verticalCenter: parent.verticalCenter
                        name: "add"
                    }
                    Label {
                        text: qsTr("Add email")
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }
}
