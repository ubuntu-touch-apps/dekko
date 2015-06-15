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
import U1db 1.0 as U1db
import "../UCSComponents"

Panel {
    id: draftsPanel
    signal openDraft(string docId)
    property alias draftsData: draftsDatabase
    property alias _listView: listView
    align: Qt.AlignLeft
    triggerSize: units.gu(0)

    U1db.Database {
        id: draftsDatabase
        path: dekko.currentAccount.accountId
    }
    U1db.Index {
        database: draftsDatabase
        id: draftsIndex
        expression: ["to", "subject", "date"]
    }

    U1db.Query {
        id: draftsQuery
        index: draftsIndex
        query: ["*", "*"]
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.palette.normal.background
    }

    EmptyState {
        id: emptyState
        visible: !listView.count
        iconName: "edit"
        title: qsTr("You currently have no draft messages saved")
        anchors {
            left: parent.left
//            leftMargin: units.gu()
            right: parent.right
//            rightMargin: units.gu(3)
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: -units.gu(4)
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        clip: true
        model: draftsQuery
        delegate: ListItem.Subtitled {
            text: contents.to[0] ? contents.to[0] : qsTr("<No Recipient>")
            subText: contents.subject
            removable: true
            confirmRemoval: true
            onItemRemoved: {
                draftsDatabase.putDoc("", model.docId)
            }
            onClicked: openDraft(model.docId)
            Label {
                anchors {
                    right: parent.right
                    rightMargin: units.gu(2)
                    verticalCenter: parent.verticalCenter
                }
                text: contents.date
            }
        }
    }
}
