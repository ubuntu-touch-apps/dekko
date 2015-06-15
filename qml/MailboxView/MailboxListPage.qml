/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Touch/

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
import Ubuntu.Components.Popups 1.0
import TrojitaCore 0.1
import DekkoCore 0.2
import "../Components"
import "../Composer"
import "../UCSComponents"


DekkoPage {
    id: mailboxListPage
    pageTitle: dekko.currentAccount.profile.displayName
    flickable: null

    // This sets the DekkoPage's internal state to set the correct actions
    // from this context.
    pageType: "mbox"
    property QtObject model: dekko.currentAccount.mailboxModel
    property string currentMailbox
    property string currentMailboxLong

    signal mailboxSelected(string mailbox)
    onMailboxSelected: {
        pageStack.push(Qt.resolvedUrl("../MessageListView/MessageListPage.qml"), {accountsDrawerVisible: false})
        dekko.currentAccount.msgListModel.setMailbox(mailbox)
        // This ensures the Inbox is always visible in the accounts drawer. As any nestingDepth causes
        // It to be hidden.
        dekko.currentAccount.mailboxModel.setOriginalRoot()
    }
    enableSearching: true
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
    onSearchActivated: {
        searchFilterModel.queryString = searchString
        searchFilterModel.invalidate()
    }

    onSearchCanceled: {
        searchFilterModel.queryString = ""
        // Let's ensure it invalidated incase we search again and
        // the previous filter is still applied. Probably unlikely
        // but let's guard it.
        searchFilterModel.invalidate()
    }


    MailboxFilterModel {
        id: filterModel
        // Eventually we are most likely going to have
        // to filter subscribed here, so we just set FILTER_NONE for the time being
        filterRole: MailboxFilterModel.FILTER_NONE
        sourceModel: dekko.currentAccount.mailboxModel
    }

    FlatteningProxyModel {
        id: flatModel
        sourceModel: filterModel
    }

    // Filter model for search queries
    MailboxFilterModel {
        id: searchFilterModel
        sourceModel: isSearchMode ? flatModel : null
        filterRole: MailboxFilterModel.FILTER_QUERY
    }

    MailboxListView {
        id: mboxListView
        property bool isSearchMode: mailboxListPage.isSearchMode
        anchors.fill: parent
        anchors.topMargin: units.gu(7)
        model: isSearchMode ? searchFilterModel : flatModel
        onOpenMailbox: {
            currentMailbox = shortMboxName
            currentMailboxLong = mboxName
            mailboxSelected(mboxName)
        }
    }
//    RadialBottomEdge {
//        visible: dekko.currentAccount.imapModel.isNetworkOnline && !isSearchMode
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
    hintIconColor: Style.common.text
}
