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
import "../Components"

Page {
    id: newAccountPage
    flickable: null

    DekkoHeader {
        id: header
        title: qsTr("New Account")
        backAction: Action {
            iconName: "back"
            onTriggered: pageStack.pop()
        }
    }

    NewAccountTypeListView {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onConfigureForProvider: pageStack.push(Qt.resolvedUrl("AccountSetup.qml"),
                                               {
                                                   state: "pre-set-account",
                                                   accountPreset: serviceInfo
                                               })
        onConfigureGenericAccount: pageStack.push(Qt.resolvedUrl("AccountSetup.qml"))
        onConfigureOnlineAccount: console.log("Configure an account to use online accounts")
    }
}
