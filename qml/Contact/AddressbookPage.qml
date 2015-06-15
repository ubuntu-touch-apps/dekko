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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import QtContacts 5.0
import "../Composer"
import "../Components"
import "imports"
Page {
    id: root

    Component.onCompleted: addressbookModelFiltered.sortByTag();
    Action {
        id: closeAction
        iconName: "back"
        onTriggered: pageStack.pop();
    }

    Action {
        id: searchAction
        iconName: "search"
        onTriggered: {
            header.state = "custom"
        }
    }
    Action {
        id: newContactAction
        iconName: "contact-new"
        onTriggered: {
            pageStack.push(Qt.resolvedUrl("./ContactEditPage.qml"));
        }
    }
    Action {
        id: leaveSearchAction
        text: "cancel"
        iconName: "cancel"
        onTriggered: {
            addressbookModelFiltered.setFilterFixedString("");
            header.state = "default"
        }
    }
    DekkoHeader {
        id: header
        title: qsTr("Addressbook")
        backAction: closeAction
        primaryAction: newContactAction
        secondaryActions: searchAction
        customComponent:
            Component {
                Item {
                    anchors.fill: parent
                    HeaderButton {
                        id: cancelSearchButton
                        anchors {
                            top: parent.top
                            left: parent.left
                            leftMargin: units.gu(1)
                            bottom: parent.bottom
                        }
                        action: leaveSearchAction
                    }

                    TextField {
                         id: searchBox

                         anchors {
                             left: cancelSearchButton.right
                             margins: units.gu(1)
                             right: parent.right
                             verticalCenter: parent.verticalCenter
                         }
                         inputMethodHints: Qt.ImhNoPredictiveText
                         placeholderText: qsTr("Search contact")
                         primaryItem: Icon {
                              height: parent.height * 0.5
                              width: parent.height * 0.5
                              anchors.verticalCenter: parent.verticalCenter
                              name: "find"
                          }
                         onTextChanged: addressbookModelFiltered.setFilterFixedString(text.trim().toLowerCase())
                         Component.onCompleted: forceActiveFocus()
                     }
                }
            }
    }

    ListView {
        id: contactList
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        model: addressbookModelFiltered
        delegate: ListItemWithActions {
            width: parent.width
            leftSideAction: Action {
                iconName: "delete"
                onTriggered: addressbookModel.removeContact(contactId);
            }

            ContactAvatar {
                id: avatar
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: height
            }
            Column {
                anchors {
                    left: avatar.right
                    right: parent.right
                    margins: units.gu(1)
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    id: name
                    objectName: "nameLabel"
                    color: UbuntuColors.lightAubergine
                    width: parent.width
                    text: firstName + " " + lastName
                    elide: Text.ElideRight
                    fontSize: "medium"
                    font.bold: true
                }

                Label {
                    width: parent.width
                    text: emails.length > 1 ? (emails.length + " " + qsTr("emails")): emails[0]
                    elide: Text.ElideRight
                    fontSize: "small"
                }
            }
            onItemClicked: {
                pageStack.push(Qt.resolvedUrl("./ContactDetailPage.qml"), {
                                contact: {
                                       contactId: contactId,
                                       firstName: firstName,
                                       lastName: lastName,
                                       emails: emails,
                                       organization: organization
                                   }
                               }
                                   );
            }
        }
        section {
            property: "tag"
            criteria: ViewSection.FirstCharacter
            delegate: SectionDelegate {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: section != "" ? section : "#"
            }
        }
    }
}
