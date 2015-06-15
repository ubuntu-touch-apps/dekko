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
import "../AccountsView"
import "../Composer"
import "../UCSComponents"

PageWithBottomEdge {
    id: dekkoPage

    property alias mailboxModel: mboxProxy
    property alias drawerOpen: accountsDrawer.opened
    property alias pageType: dekkoPageState.state
    // We should show the back action when this is false
    // THis would be used in the case that we have opened a mailbox from
    // the mailbox list and the user should be able to just go back rather than open the drawer and then reopen
    // the mailbox list. i.e one tap less
    property bool accountsDrawerVisible: true

    flickable: null
    property alias pageTitle: header.title
    property alias primaryAction: header.primaryAction
    property alias secondaryActions: header.secondaryActions
    property alias multiSelectActions: header.multiSelectActionList
    property alias enableSearching: header.enableSearching
    property alias showDivider: header.showDivider
    property alias filters: header.filterSections
    property alias selectedFilterIndex: header.selectedIndex
    readonly property bool isSearchMode: header.isSearchMode
    readonly property bool isMultiSelect: header.isMultiSelect
    property alias pageFlickable: header.pageFlickable

    signal searchActivated(string searchString)
    signal searchCanceled()
    signal multiSelectCanceled()

    function activateMultiSelectToolbar() {
        header.startMultiSelect()
    }

    DekkoHeader {
        id: header
        width: parent.width
        backAction: accountsDrawerVisible ? drawerAction : bkAction
        onSearchActivated: dekkoPage.searchActivated(searchString)
        onSearchCanceled: dekkoPage.searchCanceled()
        onMultiSelectCanceled: dekkoPage.multiSelectCanceled()
    }

    Action {
        id: drawerAction
        iconName: "navigation-menu"
        onTriggered: accountsDrawer.opened ? accountsDrawer.close() : accountsDrawer.open()
    }

    Action {
        id: bkAction
        iconName: "back"
        onTriggered: pageStack.pop()
    }

    TaskProgressBar {
        id: taskProgress
        z: 1
        anchors {
            top: header.bottom
        }
    }

    // This item tracks the pageState in terms of whether we are
    // looking at the msglist view or mboxview
    // This way we can determine whether we need to pop & push
    // or just simpley switch currentAccount and set page title.
    // It's been placed in this Item as I don't want it to
    // interfere with the page states of either of the views
    Item {
        id: dekkoPageState
        state: "msgList"
        states: [
            State {
                name: "mbox"
            },
            State {
                name: "msgList"
            }
        ]
    }

    // For the inbox model we use this proxy model which merges
    // the mailbox models for each account into a single one
    // We then pass it through the MailboxFilterModel on the FILTER_INBOX filter role
    // to just display the inbox for each account. The rolenames are exactly the same as that of
    // of the mailboxmodel.
    MailboxProxyModel {
        id: mboxProxy
        mailboxModels: dekko.accountsManager.mailboxModels
    }
    MailboxFilterModel {
        id: filterModel
        filterRole: MailboxFilterModel.FILTER_INBOX
        sourceModel: mboxProxy
    }
    // TODO: ask design if we should use this panel background or not. I personally prefer it
    // as it helps to bring focus onto the drawer.
    Rectangle {
        id: panelOpenBackground
        visible: accountsDrawer.opened || accountsDrawer.animating
        color: "white"
        z: 1
        anchors.fill: dekkoPage
        opacity: 0.5 * ((dekkoPage.width - accountsDrawer.position) / dekkoPage.width)
    }

    // FIXME: At the moment it's not easy to select the inbox on app start
    // Let's just open the panel and let the user pick the mailbox they want
    // Not ideal but at least we have affline mode working again
    Connections {
        target: dekko.accountsManager
        onStartedOffline: accountsDrawer.open()
    }

    AccountsDrawer {
        id: accountsDrawer
        animate: true
        // Take up 90% height and 80% width
        maxHeight: (parent.height - header.height) / 10 * 9
        width: parent.width / 10 * 8
        // make sure this overlays the page contents
        z: 60
        anchors {
            left: parent.left
            top: taskProgress.bottom
        }
        openSettingsAction: Action {
            onTriggered: dekko.openSettings(dekkoPageState.state)
        }
        openAddressbookAction: Action {
            onTriggered: pageStack.push(Qt.resolvedUrl("../Contact/AddressbookPage.qml"));
        }

        // The accountsManager holds all the info we need for this model
        accountsModel: dekko.accountsManager
        accountsDelegate: AccountsDrawerDelegate {
            text: model.displayName
            iconSource: Qt.resolvedUrl(model.domainIcon)
            isSelected: dekko.currentAccount.accountId === dekko.accountsManager.get(index).accountId && pageType === "mbox"
            function trigger() {
                // TODO: decide on whether we clear the stack or push on top of
                // the current view. If we push on top we would have to pop()
                // the page after eventually selecting a mailbox.
                // It probably makes more sense to pop the messagelist before pushing this
                // NOTE: this seems to work *ok* at the moment.
                dekko.currentAccount = dekko.accountsManager.get(index)
                accountsDrawer.close()
                if (dekkoPageState.state === "msgList") {
                    pageStack.pop()
                    pageStack.push(Qt.resolvedUrl("../MailboxView/MailboxListPage.qml"))
                }
            }
        }
        inboxModel: filterModel
        inboxDelegate: AccountsDrawerDelegate {
            property bool isCurrentAccount: profile.accountId === dekko.currentAccount.accountId
            property bool isInbox: dekko.currentAccount.msgListModel.mailboxName === "INBOX"
            property bool isMessageListView: pageType === "msgList"
            text: profile.accountId ? profile.displayName : ""
            iconSource: "qrc:///actions/inbox.svg"
            infoText: unreadMessageCount ? unreadMessageCount : ""
            isSelected: isCurrentAccount && isInbox && isMessageListView
            property string mboxName: mailboxIsINBOX ? qsTr("Inbox") : mailboxName
            function trigger() {
                dekko.currentAccount = mboxProxy.getAccountAndSetMailbox(index, mailboxName)
                accountsDrawer.close()
                if (dekkoPageState.state === "msgList") {
                    dekkoPage.updateTitle()
                } else if (dekkoPageState.state === "mbox") {
                    pageStack.clear()
                    pageStack.push(Qt.resolvedUrl("../MessageListView/MessageListPage.qml"))
                }
            }
            // The designs require we show the description
            // and organization here instead of mailboxName. Now the mailboxmodel
            // doesn't hold this type of info so we use the new accountId role
            // to load an AccountProfile instance which will give us what we need.
            AccountProfile {
                id: profile
                accountId: model.accountId
            }
        }
    }
}
