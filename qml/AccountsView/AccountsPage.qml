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
import TrojitaCore 0.1
import DekkoCore 0.2
import "../UCSComponents"
import "../SetupWizard"
import "../Components"

Page {
    id: accountsPage
    flickable: null

    DekkoHeader {
        id: header
        width: parent.width
        height: units.gu(7)
        title: qsTr("Accounts")
    }

    signal openAccount(int index)

    property alias model: accountsListView.listModel

    Action {
        id: addAccountAction
        onTriggered: {
            pageStack.push(Qt.resolvedUrl("../SetupWizard/NewAccountPage.qml"))
        }
    }

    EmptyState {
        id: emptyState
        visible: !model.count
        iconName: "email"
        title: qsTr("No email account is setup yet. Add one?")
        anchors {
            left: parent.left
            top: header.bottom
            topMargin: units.gu(15)
            right: parent.right
        }
    }

    AccountsListView {
        id: accountsListView
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: model.count
    }

    Button {
        id: btn
        anchors {
            top: emptyState.bottom
            topMargin: units.gu(7)
            horizontalCenter: parent.horizontalCenter
        }
        visible: emptyState.visible
        strokeColor: Style.buttons.stroke
        width: units.gu(20)
        height: units.gu(5)
        onClicked: addAccountAction.trigger()
        Text {
            anchors {
                centerIn: parent
            }
            color: btn.pressed ? "#FFFFFF" : Style.buttons.stroke
            elide: Text.ElideRight
            height: parent.height
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Add account")
            verticalAlignment: Text.AlignVCenter
            width: parent.width - units.gu(2)
        }
    }

}
