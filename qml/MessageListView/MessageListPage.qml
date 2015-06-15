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
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItems
import Ubuntu.Components.Popups 1.0
import "../Components"
import "../Utils/Utils.js" as Utils
import "../UCSComponents"
import "../Composer"
import TrojitaCore 0.1
import DekkoCore 0.2

DekkoPage {
    id: messageListPage
    flickable: null
    pageTitle: dekko.currentAccount.msgListModel.unreadCount ? qsTr("Inbox %1").arg(dekko.currentAccount.msgListModel.unreadCount ) : qsTr("Inbox")


    function updateTitle() {
        // The title field accepts rich text so that we can shrink the
        // unread count using the <small> markup
        if (mailBox === "INBOX") {
            if (unreadCount) {
                pageTitle = qsTr("Inbox <small>(%1)</small>").arg(unreadCount)
            } else {
                pageTitle = qsTr("Inbox")
            }
        } else {
            if (unreadCount) {
                pageTitle = "%1 <small>(%2)</small>".arg(mailBox).arg(unreadCount)
            } else {
                pageTitle = "%1".arg(mailBox)
            }
        }
    }

    //-----------------------------------
    // PROPERTIES
    //-----------------------------------

    property string mailBox: dekko.currentAccount.msgListModel.mailboxName
    property int unreadCount: dekko.currentAccount.msgListModel.unreadCount
    readonly property var defaultFilters: [qsTr("All"), qsTr("Unread"), qsTr("Starred"), qsTr("Tagged"), qsTr("Deleted")]

    pageType: "msgList"
    showDivider: true
    enableSearching: true
    filters: defaultFilters
    onSelectedFilterIndexChanged: {
        messagesListView.setFilter(selectedFilterIndex)
    }


    // place these in secondary actions as searchEnabled takes primaryAction
    secondaryActions: [
        Action {
            id: connectionDialogAction
            iconName: "nm-no-connection"
            visible: !dekko.currentAccount.imapModel.isNetworkOnline
            text: qsTr("Network")
            onTriggered: {
                dekko.currentAccount.network.setNetworkOnline()
            }
        }
    ]
    multiSelectActions: messagesListView.editActions
    onUnreadCountChanged: updateTitle()
    onMailBoxChanged: updateTitle()
    onSearchActivated: mboxSearch.searchString = searchString
    onSearchCanceled: mboxSearch.searchString = ""
    onMultiSelectCanceled: {
        messagesListView.cancelSelection()
        state = "default"
    }

    Connections {
        target: dekko.currentAccount.msgListModel
        onMailboxChanged: messagesListView.forceLayout()
    }

    Component {
        id: editComponent
        MultiSelectToolbar {
            onVisibleChanged: {
                //To prevent a bug that display this on OneMessagePage
                if (!messageListPage.active)
                    visible = false;
            }
        }
    }

    Component {
        id: searchComponent
        TextField {
            id: searchField
            inputMethodHints: Qt.ImhNoPredictiveText
            placeholderText: qsTr("Search...")
            onAccepted: {
                focus = false
                mboxSearch.searchString = text
            }
            Component.onCompleted: focus = true
        }
    }
    MailboxSearch {
        id: mboxSearch
        msgModel: dekko.currentAccount.threadingModel ? dekko.currentAccount.threadingModel : null
        // Lets just support sender and subject as default and let the
        // user add body and recipient from the popover
        searchSender: true
        searchSubject: true
        searchRecipients: true
    }

    //----------------------------------
    // COMPONENTS
    //----------------------------------

    // We do not need to set any anchors on the messageListView, the page header
    // will take care of it.
    pageFlickable: messagesListView
    //We just let the header know this is our
    // main content object on the screen.
    MessagesListView {
        id: messagesListView
        interactive: !drawerOpen
//        clip: true
    }

    Connections {
        target: head.sections
        onSelectedIndexChanged: {
            // switch section wont remember selection now, so disable selection when user switch sections in edit mode.
            if (messagesListView.isInSelectionMode)
                messagesListView.cancelSelection();

            messagesListView.setFilter(head.sections.selectedIndex)
        }
    }

//    RadialBottomEdge {
//        visible: messageListPage.state !== "search" && dekko.currentAccount.imapModel.isNetworkOnline
//        actions: [
//            RadialAction {
//                iconSource: "qrc:///actions/draft.svg"
//                top: true
//                onTriggered: {
//                    pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
//                                   {
//                                       replyMode: ReplyMode.REPLY_NONE,
//                                       pageTitle:qsTr("New Message")
//                                   });
//                }
//                text: qsTr("Compose")
//            }
//        ]
//    }

    bottomEdgePageComponent: ComposePage {
        replyMode: ReplyMode.REPLY_NONE
        pageTitle: qsTr("New Message")
        isOpen: bottomEdgeExpanded
    }
    bottomEdgeEnabled: !messageListPage.isSearchMode && dekko.currentAccount.imapModel.isNetworkOnline
    hintIconColor: Style.common.text
//    bottomEdgeTitle: qsTr("Compose")
}
