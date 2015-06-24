/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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
import DekkoCore 0.2
import "../Composer"
import "../Components"
import "imports/Contacts.js" as Contacts
import "../Dialogs"

Page {
    id: root

    property var contact

    signal editContact(var contact);

    flickable: null
    title: ""

    Component.onCompleted: {
        for (var i in contact.emails) {
            emailList.append({email: contact.emails[i]});
        }
    }
    ListModel {
        id: emailList
    }

    DekkoHeader {
        id: header
        width: parent.width
        title: contact.firstName
        primaryAction: Action {
            iconName: "delete"
            onTriggered: {
                PopupUtils.open(confirmDeleteDialogComponent, root, {contact: root.contact})
            }
        }
        secondaryActions: Action {
            iconName: "edit"
            onTriggered: {
                pageStack.push(Qt.resolvedUrl("./ContactEditPage.qml"), {
                                contact: root.contact
                               }).contactUpdated.connect(function(c) {
                                    root.contact = c;
                                    emailList.clear();
                                    for (var i in root.contact.emails) {
                                        emailList.append({email: root.contact.emails[i]});
                                    }
                                    addressbookModelFiltered.sortByTag();
                               })
            }
        }

        backAction: Action {
            iconName: "back"
            onTriggered: pageStack.pop();
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
                leftMargin: units.gu(2)
                rightMargin: units.gu(2)
            }
            Item {
                width: parent.width
                height: units.gu(10)
                Item {
                    //TODO avatar
                    id: avatar
                    height: units.gu(8)
                    width: 1
                    anchors {
                        left: parent.left
                        top: parent.top
                        margins: units.gu(1)
                    }
                }

                Label {
                    id: nameLabel
                    anchors {
                        left: avatar.right
                        top: avatar.top
                        margins: units.gu(1)
                        right: parent.right
                    }
                    text: contact.firstName + " " + contact.lastName
                    fontSize: "large"
                    font.bold: true
                    elide: Text.ElideRight
                }
                Label {
                    anchors {
                        top: nameLabel.bottom
                        left: nameLabel.left
                        topMargin: units.gu(1)
                    }

                    text: contact.organization
                }
                //TODO add favourite
            }
            Column {
                width: parent.width

                ListItem.Header {
                    text: qsTr("Emails")
                }
                Repeater {
                    model: emailList
                    delegate:
                        ListItem.Standard {
                            text: email
                            Icon {
                                name: "compose"
                                height: units.gu(3)
                                width: height
                                anchors {
                                    right: parent.right
                                    rightMargin: units.gu(2)
                                    verticalCenter: parent.verticalCenter
                                }
                            }
                            onClicked: {
                                pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                                   {
                                       replyMode: ReplyMode.REPLY_NONE,
                                       pageTitle:qsTr("New Message"),
                                       mailtoUrl: internal.formatMailto(email),
                                       isBottomEdgeMode: false
                                   });
                            }
                        }
                }
            }
        }
    }
    Component {
        id: confirmDeleteDialogComponent
        ConfirmationDialog {
            id: confirmDeleteDialog
            property var contact
            text: qsTr("Remove contact %1?").arg(Contacts.formatToDisplay(contact))
            confirmAction: Action {
                onTriggered: {
                    addressbookModel.removeContact(contact.contactId)
                    pageStack.pop();
                }
            }
        }
    }
    QtObject {
        id: internal
        function formatMailto(email) {
            if (contact.firstName) {
                return "mailto:" + contact.firstName + " " + contact.lastName + " <" + email + ">"
            } else
                return "mailto:" + email
        }
    }
}
