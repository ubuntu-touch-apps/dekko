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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItems
import Ubuntu.Components.Popups 1.0
import TrojitaCore 0.1
import "../Dialogs"

ToolbarItems {
    id: messageListToolbar
    property var actions: [markMailReadAction, expungeAction]

    // Actions
    Action {
        id: optionButtonAction
        iconName: "user-switch"
        text: qsTr("Sort Options")
        onTriggered: PopupUtils.open(optionComponent)
    }

    Action {
        id: expungeAction
        visible: !imapAccess.autoExpungeOnDelete && dekko.networkAvailable
        iconName: "delete"
        text: qsTr("Expunge mailbox")
        onTriggered: imapAccess.imapModel.expungeMailbox(imapAccess.msgListModel.currentMailbox())
    }

    Action {
        id: markMailReadAction
        iconName: "select"
        visible: dekko.networkAvailable
        text: qsTr("Mark mailbox read")
        onTriggered: PopupUtils.open(
                         Qt.resolvedUrl("../Dialogs/ConfirmationDialog.qml"),
                         dekko,
                         {
                             title: qsTr("Are you sure?"),
                             text: qsTr("Please confirm you wish to mark all messages in %1 as read").arg(messageListPage.title),
                             confirmAction: markMailReadConfirmAction
                         }
                         )
    }

    Action {
        id: markMailReadConfirmAction
        iconName: "select"
        text: qsTr("Mark mailbox read")
        onTriggered: imapAccess.imapModel.markMailboxAsRead(imapAccess.msgListModel.currentMailbox())
    }


    ToolbarButton {
        id: markMailboxReadButton
        action: markMailReadAction
    }
    ToolbarButton {
        id: expungeMailboxButton
        action: expungeAction
    }
    // FIXME: disabling again until we have search capabilities with
    // the threadeing model
//    ToolbarButton {
//        id: optionButton
//        action: optionButtonAction
//    }

    // sort button popover
    Component {
        id: optionComponent
        Dialog {
            id: popover
            title: qsTr("Sort Options")
//            text: qsTr("")
            contents: OptionSelector {
                id: sortOrderSelector
                text: qsTr("Sort Order")
                model: sortOrderModel
                // It would be great to get a sort icon to go next to each sort order
                delegate: OptionSelectorDelegate { text: description }
                onSelectedIndexChanged: sortAction.trigger()
                property var sortOrder: {'ASCENDING': 0, 'DESCENDING': 1 }
                Component.onCompleted: {
                    switch (imapAccess.threadingMsgListModel.currentSortOrder()) {
                    case Qt.AscendingOrder:
                        sortOrderSelector.selectedIndex = sortOrder.ASCENDING
                        break
                    case Qt.DescendingOrder:
                        sortOrderSelector.selectedIndex = sortOrder.DESCENDING
                        break
                    }
                }
            }

            Action {
                id: sortAction
                onTriggered: {
                    if (sortOrderSelector.model.get(sortOrderSelector.selectedIndex).prop !==
                            imapAccess.threadingMsgListModel.currentSortOrder()) {
                        imapAccess.threadingMsgListModel.setUserSearchingSortingPreference(
                                    [],
                                    ThreadingMsgListModel.SORT_NONE,
                                    sortOrderSelector.model.get(sortOrderSelector.selectedIndex).prop
                                    )
                        PopupUtils.close(popover)
                    }
                }
            }
        }
    }

    ListModel {
        id: sortOrderModel
        ListElement { description: QT_TR_NOOP("Ascending"); prop: Qt.AscendingOrder }
        ListElement { description: QT_TR_NOOP("Descending"); prop: Qt.DescendingOrder }
    }
}
