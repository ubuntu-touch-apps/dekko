/* Copyright (C) 2006 - 2014 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import "../Utils/Utils.js" as Utils
import DekkoCore 0.2

Item {
    id: container

    property variant address
    property bool nameOnly: addressOnly ? false : true
    property bool addressOnly: false
    property alias color: addressWidget.color
    property bool appendComma: false
    property alias text: addressWidget.text

    width: addressWidget.width + units.gu(1)
    height: addressWidget.height //+ units.gu(1)
    Label {
        id: addressWidget
        text: Utils.formatMailAddress(address, nameOnly, addressOnly) + (appendComma? "," : "")
        linkColor: UbuntuColors.orange
        wrapMode: Text.WordWrap
        font.weight: Font.DemiBold
    }
    MouseArea {
        anchors.fill: parent
        onClicked:{
            PopupUtils.open(contextMenu, addressWidget)
        }
    }
    Component {
        id: contextMenu
        Popover {
            id: popover
            Column {
                width: parent.width

                ListItem.Header {
                    text: Utils.formatPlainArrayMailAddress(address, false, true)
                }

                ListItem.Standard {
                    text: qsTr("Write to")
                    onClicked: {
                        oneMessagePage.writeTo(Utils.prettyAddress(address));
                        PopupUtils.close(popover);
                    }
                }

                ListItem.Standard {
                    text: qsTr("Add to Addressbook")
                    visible: currentAccount.devModeEnableContacts
                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("../Contact/ContactChoicePage.qml"),
                                        {   mode: "add",
                                            prettyEmailAddress: Utils.prettyAddress(address)
                                        });
                        PopupUtils.close(popover);
                    }
                }
            }
        }
    }
}
