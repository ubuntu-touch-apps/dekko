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
import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0
import TrojitaCore 0.1
import DekkoCore 0.2
import "../Components"

MultipleSelectionListView {
    id: root

    //-----------------------------------
    // PROPERTIES
    //-----------------------------------
    //    property alias hasScrollbar: listViewScrollBar.visible
    property var _currentSwipedItem: null
    property var editActions: [selectAllAction, multiReadAction, multiFlagAction, multiDeleteAction]

    //-----------------------------------
    // SIGNALS
    //-----------------------------------
    signal messageSelected(int msgUid)

    //-----------------------------------
    // SLOTS
    //-----------------------------------
    onMessageSelected: internal.openMessage(msgUid)
    onSelectionDone: {
        for (var i=0; i < items.count; i++) {
            accountsListModel.removeRow(items.get(i).index)
        }
    }

    function moveMessage(msgUid) {
        mboxPicker.pick()
        mboxPicker.uid = msgUid
    }

    MailboxPicker {
        id: mboxPicker
        property var uid
        account: dekko.currentAccount
        onMailboxSelected: {
            mailboxUtils.moveMessageToMailbox(dekko.currentAccount.msgListModel.mailboxName, uid, mboxName)
        }
    }

    onIsInSelectionModeChanged: {
        if (isInSelectionMode) {
            if (_currentSwipedItem) {
                _currentSwipedItem.resetSwipe()
                _currentSwipedItem = null
            }
            messageListPage.activateMultiSelectToolbar()
        }
    }

    onSelectionCanceled: {
        multiFlagAction.selectedUnFlaggedCount = 0;
        multiReadAction.selectedUnReadCount = 0;
    }

    onMovementStarted: {
        internal.wasAtYBeginning = atYBeginning
        internal.initialContentY = contentY
    }

    onContentYChanged: {
        if (dragging) {
            if (internal.wasAtYBeginning && internal.initialContentY - contentY > units.gu(7)) {
                internal.toBeReloaded = true
            }
            else {
                internal.toBeReloaded = false
            }
        }
    }

    onMovementEnded: {
        if (internal.toBeReloaded) {
            internal.resyncMailbox()
            internal.toBeReloaded = false;
        }
    }

    //----------------------------------
    // FUNCTIONS
    //----------------------------------
    function resetSwipe() {
        if (_currentSwipedItem) {
            _currentSwipedItem.resetSwipe()
            _currentSwipedItem = null
        }
    }

    function _updateSwipeState(item) {
        if (item.swipping) {
            return
        }

        if (item.swipeState !== "Normal") {
            if (_currentSwipedItem !== item) {
                if (_currentSwipedItem) {
                    _currentSwipedItem.resetSwipe()
                }
                _currentSwipedItem = item
            }
        } else if (item.swipeState !== "Normal" && _currentSwipedItem === item) {
            _currentSwipedItem = null
        }
    }

    function setFilter(filterRole) {
        messageListFilterModel.filterRole = filterRole;
        messageListFilterModel.invalidate();
    }

    function updateSelectOperations() {
        var unReadCount = 0;
        var unFlaggedCount = 0;
        for (var i = 0; i < messagesListView.selectedItems.count; i++) {
            if (!messagesListView.selectedItems.get(i).model.isMarkedRead) {
                unReadCount ++;
            }
            if (!messagesListView.selectedItems.get(i).model.isMarkedFlagged) {
                unFlaggedCount ++;
            }
        }
        multiFlagAction.selectedUnFlaggedCount = unFlaggedCount;
        multiReadAction.selectedUnReadCount = unReadCount;
    }

    MessageListFilterModel {
        id: messageListFilterModel
        sourceModel: dekko.currentAccount.threadingModel
        filterMarkedDeleted: GlobalSettings.preferences.hideMarkedDeleted
    }

    LimitProxyModel {
        id: limitedProxyModel
        // TODO: make this configurable
        limit: 50
        model: messageListFilterModel
    }


    //----------------------------------
    // OBJECT PROPERTIES
    //----------------------------------
    clip: false
    maximumFlickVelocity: height * 6
    flickDeceleration: height * 2
    cacheBuffer: 5000
    section.property: dekko.currentAccount.msgListModel.itemsValid ? "fuzzyDate" : ""
    section.delegate: SectionDivider {
        text: section ? section : ""
    }
    footer: Loader {
        width: parent.width
        height: units.gu(10)
        active: messageListFilterModel.count > limitedProxyModel.limit
        sourceComponent: footerComponent
    }

    // TODO: Move position bindings from the component to the Loader.
    //       Check all uses of 'parent' inside the root element of the component.
    Component {
        id: footerComponent
        ListItem {
            anchors.fill: parent
            Label {
                anchors.centerIn: parent
                text: qsTr("Load more messages....")
            }
            onClicked: limitedProxyModel.limit = limitedProxyModel.limit + 50
        }
    }



    listModel: limitedProxyModel
    displaced: Transition {
        UbuntuNumberAnimation {
            property: "y"
        }
    }
    remove: Transition {
        ParallelAnimation {
            UbuntuNumberAnimation {
                property: "height"
                to: 0
            }

            UbuntuNumberAnimation {
                properties: "opacity"
                to: 0
            }
            ScriptAction {
                script: {
                    resetSwipe()
                }
            }
        }
    }
    listDelegate: MessageListDelegate {
        id: delegate
        anchors {
            left: parent.left
            right: parent.right
        }
        selectedColor: color
        onMarkMessageDeleted: mailboxUtils.markMessageDeleted(
                                  mailboxUtils.deproxifiedIndex(
                                      root.model.modelIndex(
                                          (model.index))),
                                  !model.isMarkedDeleted)
        onMarkMessageFlagged: flaggedAction.trigger()
        // Our actions require a network connection to work
        // So stop them if no network is available.
        locked: !dekko.currentAccount.imapModel.isNetworkOnline

        onItemClicked: {
            if (selectionMode) {
                if (selected) {
                    unselect();
                } else {
                    select();
                }
                return;
            }

            if (model.hasModelChildren) {
                root.model.rootIndex = root.model.modelIndex(index)
            } else {
                messageSelected(messageUid)
            }
        }

        onItemPressAndHold: {
            // For now not's let the user search and perform multi-actions
            // until we have well tested it.
            if (messageListPage.state !== "search") {
                messagesListView.startSelection();
                select();
            }
        }

        selected: messagesListView.isSelected(delegate)
        selectionMode: messagesListView.isInSelectionMode
        isFirst: model.index === 0

        active: ListView.isCurrentItem

        leftSideAction: Action {
            id: deleteMsg
            iconName: "delete"
            text: "Delete"
            onTriggered: delegate.markMessageDeleted()
        }

        rightSideActions: [
            Action {
                id: flaggedAction
                text: model.isMarkedFlagged ? qsTr("Un-mark") : qsTr("Mark")
                iconName: model.isMarkedFlagged ? "favorite-unselected" : "favorite-selected"

                onTriggered: mailboxUtils.markMessageAsFlagged(mailboxUtils.deproxifiedIndex(root.model.modelIndex(model.index)),
                                                               !model.isMarkedFlagged)
            },
            Action {
                id: readAction
                text: model.isMarkedRead ? qsTr("Mark un-read") : qsTr("Mark read")
                iconSource: model.isMarkedRead ? "qrc:///actions/mail-unread.svg" : "qrc:///actions/mail-read.svg"

                onTriggered:mailboxUtils.markMessageAsRead(
                                mailboxUtils.deproxifiedIndex(
                                    root.model.modelIndex(
                                        (model.index))),
                                !isMarkedRead);
            },
            Action {
                id: extras
                iconName: "add"
                onTriggered: PopupUtils.open(Qt.resolvedUrl("./MessageListActionPopover.qml"), delegate, {mboxName: messageListPage.mailBox, uid: model.messageUid})

            }

        ]

        function select() {
            if (messagesListView.selectItem(delegate)) {
                root.updateSelectOperations();
            }
        }
        function unselect() {
            if (messagesListView.deselectItem(delegate)) {
                root.updateSelectOperations();
            }
        }
    }

    MailboxUtils {
        id: mailboxUtils
        account: dekko.currentAccount
    }

    Action {
        id: multiDeleteAction
        iconName: "delete"
        text: qsTr("Delete")
        onTriggered: {
            for (var i = 0; i < messagesListView.selectedItems.count; i++) {
                mailboxUtils.appendToSelectedIndexList(mailboxUtils.deproxifiedIndex(
                                                           root.model.modelIndex(
                                                               messagesListView.selectedItems.get(i).itemsIndex)));

            }
            mailboxUtils.markSelectedMessagesDeleted(true);
            mailboxUtils.clearSelectedMessages(); // Ensure cleared before proceeding
            messagesListView.clearSelection();
        }
    }

    Action {
        id: multiFlagAction

        property int selectedUnFlaggedCount: 0

        iconName: selectedUnFlaggedCount > 0 || messagesListView.selectedItems.count == 0 ? "favorite-selected" : "favorite-unselected"
        text: selectedUnFlaggedCount > 0 || messagesListView.selectedItems.count == 0 ? qsTr("Star") : qsTr("Remove star")
        onTriggered: {
            for (var i = 0; i < messagesListView.selectedItems.count; i++) {
                if (messagesListView.selectedItems.get(i).model.isMarkedFlagged !== selectedUnFlaggedCount > 0) {
                    mailboxUtils.appendToSelectedIndexList(
                                mailboxUtils.deproxifiedIndex(
                                    root.model.modelIndex(messagesListView.selectedItems.get(i).itemsIndex)));
                }
            }
            mailboxUtils.markSelectedMessagesFlagged(selectedUnFlaggedCount > 0);
            mailboxUtils.clearSelectedMessages();
            messagesListView.clearSelection();
        }
    }

    Action {
        id: multiReadAction

        property int selectedUnReadCount: 0

        iconSource: selectedUnReadCount > 0  || messagesListView.selectedItems.count == 0 ? "qrc:///actions/mail-unread.svg" : "qrc:///actions/mail-read.svg"
        text: selectedUnReadCount > 0 || messagesListView.selectedItems.count == 0 ? qsTr("Mark read") : qsTr("Mark unread")
        onTriggered: {
            for (var i = 0; i < messagesListView.selectedItems.count; i++) {
                if (messagesListView.selectedItems.get(i).model.isMarkedRead !== selectedUnReadCount > 0) {
                    mailboxUtils.appendToSelectedIndexList(
                                mailboxUtils.deproxifiedIndex(
                                    root.model.modelIndex(messagesListView.selectedItems.get(i).itemsIndex)));
                }
            }
            mailboxUtils.markSelectedMessagesAsRead(selectedUnReadCount > 0);
            mailboxUtils.clearSelectedMessages();
            messagesListView.clearSelection();
        }
    }

    Action {
        id: selectAllAction
        property bool allSelected: root.selectedItems.count === root.model.count
        iconSource: allSelected ? "qrc:///actions/select-none.svg" : "qrc:///actions/select.svg"
        text: allSelected ? qsTr("Unselect all") : qsTr("Select all")
        onTriggered: {
            if (!allSelected) {
                messagesListView.selectAll();
            } else {
                messagesListView.clearSelection();
            }
            root.updateSelectOperations();
        }
    }

    //-----------------------------------
    // PRIVATE PROPERTIES & FUNCTIONS
    //-----------------------------------
    QtObject {
        id: internal

        property bool wasAtYBeginning: false
        property bool toBeReloaded: false
        property int initialContentY: 0

        function resyncMailbox() {
            dekko.currentAccount.imapModel.resyncMailbox(dekko.currentAccount.msgListModel.currentMailbox());
        }

        function openMessage(msgUid) {
            pageStack.push(Qt.resolvedUrl("../MessageView/OneMessagePage.qml"),
                           {
                               mailboxName: messageListPage.mailBox,
                               uid: msgUid,
                               messageListModel: messageListFilterModel
                           }
                           )
        }
    }

    //---------------------------------
    // COMPONENTS
    //---------------------------------
    Label {
        anchors { left: parent.left; right: parent.right; bottom: parent.top }
        anchors.bottomMargin: root.contentY - root.originY
        height: units.gu(3)
        fontSize: 'medium'
        horizontalAlignment: Text.AlignHCenter
        text: internal.toBeReloaded ? qsTr("Release to refresh...") : qsTr("Pull to refresh...")
    }

    Scrollbar {
        id: listViewScrollBar
        visible: false
        flickableItem: root
        align: Qt.AlignTrailing
    }
}
