/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
import Ubuntu.Components.ListItems 1.0 as ListItem
import "../Utils/Utils.js" as Utils

// TODO: this need massively reworking as nothing is lining up correctly
// What was i thinking???
ListItem.Base {
    id: headerExpandable

    // Properties
    height: infoColumn.height
    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
//        leftMargin: units.gu(1)
        rightMargin: units.gu(1)
        topMargin: units.gu(1.5)
    }
    clip: true
//            showDivider: false
    property var model
    property bool expanded: false
    Column {
        id: infoColumn
        width: parent.width

        spacing: units.gu(1)

        Item {
            id: fromItem
            height: fromRow.height
            width: parent.width
            clip: true

            HeaderRecipientRow {
                id: fromRow
                label: qsTr("From:")
                address: model.from
                maxWidth: parent.width - dateLabel.width - dateLabel.anchors.rightMargin - units.gu(1)
            }

            Label {
                id: dateLabel
                fontSize: "small"
                text: Utils.formatDateDetailed(model.date)
                anchors {
                    right: parent.right
                    rightMargin: units.gu(1)
                    baseline: fromLabel.baseline
                }
            }
        }
        HeaderRecipientRow {
            id: toRow
            label: qsTr("To:")
            address: model.to
        }
        HeaderRecipientRow {
            id: ccRow
            label: qsTr("Cc:")
            address: model.cc
            height: (headerExpandable.expanded ? address.length > 0 : false) ? contentHeight : 0
            Behavior on height { UbuntuNumberAnimation { } }
        }
        HeaderRecipientRow {
            id: bccRow
            label: qsTr("Bcc:")
            address: model.bcc
            height: (headerExpandable.expanded ? address.length > 0 : false) ? contentHeight : 0
            Behavior on height { UbuntuNumberAnimation { } }
        }
        Item {
            id: subjectLabelItem
            height: subjectLabel.height + units.gu(2)
            width: parent.width
            ListItem.ThinDivider {}

            Label {
                id: subjectLabel
                text: model.subject
                width: parent.width
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                anchors {
                    verticalCenter: parent.verticalCenter
                }

            }
        }
    }

    Image {
        id: dropDownIcon
        anchors {
            right: parent.right
            top: parent.top
            topMargin: units.gu(3)
            rightMargin: units.gu(1)
        }
        height: units.gu(3)
        width: height
        source: "qrc:///actions/dropdown-menu.svg"
        rotation: headerExpandable.expanded ? 180 : 0
        visible: model.cc.length + model.bcc.length > 0
        MouseArea {
            anchors.fill: parent
            onClicked: {
                headerExpandable.expanded = !headerExpandable.expanded
            }
        }
    }
}

