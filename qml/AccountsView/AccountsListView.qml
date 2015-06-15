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
import "../Components"

// The main reason for creating a seperate listview here is so
// we can re-use it without it being tied to the AccountsPage
// i.e a "Quick switch" popover or something
// Or when we come to convergence it will become part of a page and not
// a page to itself

MultipleSelectionListView {
    id: accountsListView

    property Action accountClickedAction

    property var _currentSwipedItem: null

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


    Connections {
        target: Qt.application
        onActiveChanged: {
            if (!Qt.application.active) {
                accountsListView.currentIndex = -1
            }
        }
    }

    currentIndex: -1

    onSelectionDone: {
        for (var i=0; i < items.count; i++) {
            accountsListModel.removeRow(items.get(i).index)
        }
    }

    onIsInSelectionModeChanged: {
        if (isInSelectionMode && _currentSwipedItem) {
            _currentSwipedItem.resetSwipe()
            _currentSwipedItem = null
        }
    }
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

    listDelegate: AccountsDelegate {
        id: delegate
        // TODO: replace with correctIcon
        accountImagePath: model.domainIcon
        organization: model.organization
        description: model.displayName
        showDivider: true
        anchors {
            left: parent.left
            right: parent.right
        }
        selected: accountsListView.isSelected(delegate)
        selectionMode: accountsListView.isInSelectionMode
        isFirst: model.index === 0
        locked: accountsListView.isInSelectionMode
        active: ListView.isCurrentItem

        onItemClicked: {
            openAccount(model.index)
        }

        onSwippingChanged: accountsListView._updateSwipeState(delegate)
        onSwipeStateChanged: accountsListView._updateSwipeState(delegate)

        leftSideAction: Action {
            text: qsTr("Delete")
            iconName: "delete"
            onTriggered: dekko.accountsManager.removeRow(model.index)
        }
    }
}
