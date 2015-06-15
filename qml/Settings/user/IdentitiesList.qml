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
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../Components"
import "../../UCSComponents"
import "../common"

SettingsGroupBase {

    property var newIdentityPage

    function openNewIdentityPage() {
        var component = Qt.createComponent(Qt.resolvedUrl("NewIdentityPage.qml"))
        newIdentityPage = component.createObject(root)
        newIdentityPage.closing.connect(listView.forceLayout)
        pageStack.push(newIdentityPage, {model: identityModel})
    }

    SenderIdentityModel {
        id: identityModel
        accountId: root.account.accountId
        onCountChanged: saveIdentites()
    }

    ListView {
        id: listView
        // prevent binding loop for contentheight
        property int conHeight: contentHeight
        interactive: false
        height: conHeight
        anchors {
            left: parent.left
            right: parent.right
        }

        model: identityModel
        delegate: ListItem.Standard {
            removable: true
            confirmRemoval: true
            onItemRemoved: identityModel.removeRow(index)
            text: model.formattedString
        }

        footer: Item {
            visible: identityModel.count
            width: parent.width
            height: units.gu(7)
            Button {
                anchors {
                    centerIn: parent
                }
                visible: identityModel.count
                text: qsTr("Add identity")
                color: "green"
                width: units.gu(15)
                onClicked: openNewIdentityPage()
            }
        }
    }

    EmptyState {
        id: emptyState
        visible: !identityModel.count
        iconName: "contact"
        title: qsTr("No identities configured")
        anchors {
            left: parent.left
            top: parent.top
            topMargin: units.gu(15)
            right: parent.right
        }
    }

    Button {
        anchors {
            top: emptyState.bottom
            horizontalCenter: parent.horizontalCenter
        }
        visible: emptyState.visible
        text: qsTr("Add identity")
        color: "green"
        width: units.gu(15)
        onClicked: openNewIdentityPage()
    }
}
