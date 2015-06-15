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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import QtContacts 5.0
import "../Composer"
Page {
    id: root
    signal contactSelected(var emailAddress, var displayLabel, var recipientKindIndex)
    property alias recipientKindIndex: recipientTypeSelectorListView.currentIndex

    property string mode: "read" // "read" for reading email from an existing contact,
                                 // "add" for adding email to a contact
    property string prettyEmailAddress // email to be added in "add" mode

    title: mode == "read" ? qsTr("Add recipients") : qsTr("Add to addressbook")
    state: "default"
    states: [
        PageHeadState {
            name: "default"
            head: root.head
            actions:  [
                Action {
                    iconName: "search"
                    onTriggered: {
                        root.state = "search"
                        searchBox.forceActiveFocus();
                    }
                }, Action {
                    iconName: "contact-new"
                    visible: mode == "add"
                    onTriggered: {
                        pageStack.push(Qt.resolvedUrl("../Contact/ContactEditPage.qml"), {
                                           prettyEmailAddress: root.prettyEmailAddress,
                                           contactModel: contactModel,
                                       });
                    }
                }
            ]
        },
        PageHeadState {
            name: "search"
            head: root.head
            backAction: Action {
                id: leaveSearchAction
                text: "cancel"
                iconName: "cancel"
                onTriggered: {
                    searchBox.text = "";
                    root.state = "default"
                    contactList.expandedIndex = -1;
                }
            }
            contents: searchBox
        }
    ]
    Column {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            id: recipientKindSelector
            visible: mode == "read"
            text: qsTr("Recipient kind")
            showDivider: false
            control:
                RecipientTypeSelectorListView {
                    id: recipientTypeSelectorListView
                    width: units.gu(15)
                    height: units.gu(4)
                    model: ["To", "Cc", "Bcc"]
                }
        }

        UbuntuListView {
            id: contactList
            objectName: "contactPopoverList"
            width: parent.width
            height: root.height - (recipientKindSelector.visible ? recipientKindSelector.height : 0)
            model: contactModel
            clip: true
            delegate: ListItem.Expandable {
                property var item: contactModel.contacts[index]
                id: expandingItem
                expandedHeight: emailForASinglePersonColumn.height
                collapsedHeight: contactNameListItem.height
                collapseOnClick: true

                showDivider: contactModel.contacts.length <= 1 //setting showDivider false will cause click not expanding when there is only one element.

                Column {
                    id: emailForASinglePersonColumn
                    width: parent.width
                    ListItem.Empty {
                        id: contactNameListItem

                        removable: true;
                        confirmRemoval: true;
                        onItemRemoved: {
                            contactModel.removeContact(item.contactId)
                        }

                        Icon {
                            id: avatar
                            anchors {
                                left: parent.left
                                top: parent.top
                                bottom: parent.bottom
                                margins: units.gu(1)
                            }
                            width:height
                            // cannot display avatar because it is stored in ~/.local/share/evolution/...
                            // TODO: We can add read permission to path ~/.local/share/evolution/ to apparmor like addressbook-app does
//                            source: item.avatar.imageUrl == "" ?
//                                            "image://theme/contact" : item.avatar.imageUrl
                            name: "contact"
                        }
                        Label {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                left: avatar.right
                                margins: units.gu(2)
                            }
                            text: item ? item.displayLabel.label : ""
                        }

                        showDivider: false
                        onClicked: {
                            if (root.mode == "read") {
                                contactList.expandedIndex = index
                            } else { //root.mode == "add"
                                pageStack.push(Qt.resolvedUrl("../Contact/ContactEditPage.qml"), {
                                                   contact: item,
                                                   prettyEmailAddress: root.prettyEmailAddress
                                               });
                            }
                        }
                    }
                    Column {
                        width: parent.width
                        anchors {
                            left: parent.left
                            right: parent.right
                            leftMargin: units.gu(4)
                        }

                        Repeater {
                            model: item ? item.details(ContactDetail.Email) : []
                            delegate: ListItem.Subtitled {
                                text: modelData.emailAddress
                                subText: {
                                    if (modelData.contexts.indexOf(ContactDetail.ContextHome) > -1) {
                                        return "Home";
                                    } else if (modelData.contexts.indexOf(ContactDetail.ContextWork) > -1) {
                                        return "Work";
                                    } else if (modelData.contexts.indexOf(ContactDetail.ContextOther) > -1) {
                                        return "Other";
                                    } else {
                                        return "";
                                    }
                                }
                                showDivider: false
                                removable: true;
                                confirmRemoval: true;
                                onClicked: {
                                    root.contactSelected(modelData.emailAddress, item.displayLabel.label, recipientTypeSelectorListView.currentIndex)
                                    pageStack.pop();
                                }
                                onItemRemoved: {
                                    contactList.expandedIndex = -1;
                                    item.removeDetail(item.details(ContactDetail.Email)[index]);
                                    item.save();
                                }
                                Icon {
                                    width: height
                                    anchors {
                                        top: parent.top
                                        bottom: parent.bottom
                                        right: parent.right
                                        margins: units.gu(1.5)
                                    }
                                    name: "email"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    TextField {
        id: searchBox
        width: parent.width
        visible: root.state === "search"
        placeholderText: qsTr("Search contact")
        primaryItem: Icon {
             height: parent.height * 0.5
             width: parent.height * 0.5
             anchors.verticalCenter: parent.verticalCenter
             name:"find"
         }
    }
    DetailFilter {
        id: contactFilter
        detail: ContactDetail.DisplayLabel
        field: DisplayLabel.Label
        matchFlags: Filter.MatchContains
        value: searchBox.text
    }
    ContactModel {
        id: contactModel
        manager: "memory"
        filter: contactFilter
        autoUpdate: true
        Component.onCompleted: {
            importContacts(addressbookStoreLocation);
        }
        onContactsChanged: {
            exportContacts(addressbookStoreLocation);
        }
    }
}
