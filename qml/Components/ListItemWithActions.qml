/*
 * Copyright (C) 2012-2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.3
import QtFeedback 5.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2

Item {
    id: root

    property Action leftSideAction: null
    property list<Action> rightSideActions
    property double defaultHeight: units.gu(8)
    property bool locked: false
    property Action activeAction: null
    property var activeItem: null
    property bool triggerActionOnMouseRelease: false
    property color color: Theme.palette.normal.background
    property color selectedColor: "#E6E6E6"
    property bool selected: false
    property bool selectionMode: false
    property alias showDivider: divider.visible
    property alias internalAnchors: mainContents.anchors
    default property alias contents: mainContents.children

    readonly property double actionWidth: units.gu(4)
    readonly property double leftActionWidth: units.gu(10)
    readonly property double actionThreshold: actionWidth * 0.4
    readonly property double threshold: 0.4
    readonly property string swipeState: main.x == 0 ? "Normal" : main.x > 0 ? "LeftToRight" : "RightToLeft"
    readonly property alias swipping: mainItemMoving.running
    readonly property bool _showActions: mouseArea.pressed || swipeState != "Normal" || swipping

    /* internal */
    property var _visibleRightSideActions: filterVisibleActions(rightSideActions)

    signal itemClicked(var mouse)
    signal itemPressAndHold(var mouse)

    function returnToBoundsRTL()
    {
        var actionFullWidth = actionWidth + units.gu(2)
        var xOffset = Math.abs(main.x)
        var index = Math.min(Math.floor(xOffset / actionFullWidth), _visibleRightSideActions.length)

        if (index < 1) {
            main.x = 0
        } else if (index === _visibleRightSideActions.length) {
            main.x = -(rightActionsView.width - units.gu(2))
        } else {
            main.x = -(actionFullWidth * index)
        }
    }

    function returnToBoundsLTR()
    {
        var finalX = leftActionWidth
        if (main.x > (finalX * root.threshold))
            main.x = finalX
        else
            main.x = 0
    }

    function returnToBounds()
    {
        if (main.x < 0) {
            returnToBoundsRTL()
        } else if (main.x > 0) {
            returnToBoundsLTR()
        }
    }

    function contains(item, point, marginX)
    {
        var itemStartX = item.x - marginX
        var itemEndX = item.x + item.width + marginX
        return (point.x >= itemStartX) && (point.x <= itemEndX) &&
               (point.y >= item.y) && (point.y <= (item.y + item.height));
    }

    function getActionAt(point)
    {
        if (contains(leftActionView, point, 0)) {
            return leftSideAction
        } else if (contains(rightActionsView, point, 0)) {
            var newPoint = root.mapToItem(rightActionsView, point.x, point.y)
            for (var i = 0; i < rightActionsRepeater.count; i++) {
                var child = rightActionsRepeater.itemAt(i)
                if (contains(child, newPoint, units.gu(1))) {
                    return i
                }
            }
        }
        return -1
    }

    function updateActiveAction()
    {
        if ((main.x <= -(root.actionWidth + units.gu(2))) &&
            (main.x > -(rightActionsView.width - units.gu(2)))) {
            var actionFullWidth = actionWidth + units.gu(2)
            var xOffset = Math.abs(main.x)
            var index = Math.min(Math.floor(xOffset / actionFullWidth), _visibleRightSideActions.length)
            index = index - 1
            if (index > -1) {
                root.activeItem = rightActionsRepeater.itemAt(index)
                root.activeAction = root._visibleRightSideActions[index]
            }
        } else {
            root.activeAction = null
        }
    }

    function resetSwipe()
    {
        main.x = 0
    }

    function filterVisibleActions(actions)
    {
        var visibleActions = []
        for(var i = 0; i < actions.length; i++) {
            var action = actions[i]
            if (action.visible) {
                visibleActions.push(action)
            }
        }
        return visibleActions
    }

    states: [
        State {
            name: "select"
            when: selectionMode || selected
            PropertyChanges {
                target: selectionIcon
                source: Qt.resolvedUrl("ListItemWithActionsCheckBox.qml")
                anchors.leftMargin: units.gu(2)
            }
            PropertyChanges {
                target: root
                locked: true
            }
            PropertyChanges {
                target: main
                x: 0
            }
        }
    ]

    height: defaultHeight
    clip: height !== defaultHeight
    HapticsEffect {
        id: clickEffect
        attackIntensity: 0.0
        attackTime: 50
        intensity: 1.0
        duration: 10
        fadeTime: 50
        fadeIntensity: 0.0
    }

    Rectangle {
        id: leftActionView

        anchors {
            top: parent.top
            bottom: parent.bottom
            right: main.left
        }
        width: root.leftActionWidth + actionThreshold
        visible: leftSideAction
        color: UbuntuColors.red

        Icon {
            anchors {
                centerIn: parent
                horizontalCenterOffset: actionThreshold / 2
            }
            name: leftSideAction && _showActions ? leftSideAction.iconName : ""
            color: Theme.palette.selected.field
            height: units.gu(3)
            width: units.gu(3)
        }
    }

    Item {
       id: rightActionsView

       anchors {
           top: main.top
           left: main.right
           bottom: main.bottom
       }
       visible: _visibleRightSideActions.length > 0
       width: rightActionsRepeater.count > 0 ? rightActionsRepeater.count * (root.actionWidth + units.gu(2)) + root.actionThreshold + units.gu(2) : 0
       Row {
           anchors{
               top: parent.top
               left: parent.left
               leftMargin: units.gu(2)
               right: parent.right
               rightMargin: units.gu(2)
               bottom: parent.bottom
           }
           spacing: units.gu(2)
           Repeater {
               id: rightActionsRepeater

               model: _showActions ? _visibleRightSideActions : []
               Item {
                   id: actItem
                   property alias image: img

                   height: rightActionsView.height
                   width: root.actionWidth

                   Icon {
                       id: img

                       anchors.centerIn: parent
                       width: units.gu(3)
                       height: units.gu(3)
                       source: modelData.iconSource ? modelData.iconSource : null
                       name: modelData.iconName ? modelData.iconName : null
                       color: root.activeAction === modelData || !root.triggerActionOnMouseRelease ? Style.header.background : Style.common.text
                   }
                   Rectangle {
                       id: underscore
                       width: actItem.width
                       height: units.gu(0.2)
                       anchors {
                           bottom: actItem.bottom
                           bottomMargin: units.gu(1.5)

                       }
                       // Both this and the action icon should match the header color when active
                       color: Style.header.background
                       visible: root.activeAction === modelData
                       onVisibleChanged: {
                           if (visible) {
                               clickEffect.start()
                           }
                       }
                   }
              }
           }
       }
    }

    Item {
        id: main
        objectName: "mainItem"

        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        width: parent.width

        Loader {
            id: selectionIcon

            anchors {
                left: main.left
                verticalCenter: main.verticalCenter
            }
            width: (status === Loader.Ready) ? item.implicitWidth : 0
            visible: (status === Loader.Ready) && (item.width === item.implicitWidth)
            Behavior on width {
                NumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                }
            }
        }


        Item {
            id: mainContents

            anchors {
                left: selectionIcon.right
                leftMargin: units.gu(2)
                top: parent.top
                topMargin: units.gu(1)
                right: parent.right
                rightMargin: units.gu(2)
                bottom: parent.bottom
                bottomMargin: units.gu(1)
            }
        }

        Behavior on x {
            UbuntuNumberAnimation {
                id: mainItemMoving

                easing.type: Easing.OutElastic
                duration: UbuntuAnimation.SlowDuration
            }
        }

    }

    SequentialAnimation {
        id: triggerAction

        property var currentItem: root.activeItem ? root.activeItem.image : null

        running: false
        ParallelAnimation {
            UbuntuNumberAnimation {
                target: triggerAction.currentItem
                property: "opacity"
                from: 1.0
                to: 0.0
                duration: UbuntuAnimation.SlowDuration
                easing {type: Easing.InOutBack; }
            }
            UbuntuNumberAnimation {
                target: triggerAction.currentItem
                properties: "width, height"
                from: units.gu(3)
                to: root.actionWidth
                duration: UbuntuAnimation.SlowDuration
                easing {type: Easing.InOutBack; }
            }
        }
        PropertyAction {
            target: triggerAction.currentItem
            properties: "width, height"
            value: units.gu(3)
        }
        PropertyAction {
            target: triggerAction.currentItem
            properties: "opacity"
            value: 1.0
        }
        ScriptAction {
            script: {
                root.activeAction.triggered(root)
                root.activeAction = null;
            }
        }
        PauseAnimation {
            duration: 500
        }
        UbuntuNumberAnimation {
            target: main
            property: "x"
            to: 0

        }
    }

    MouseArea {
        id: mouseArea

        property bool locked: root.locked || ((root.leftSideAction === null) && (root._visibleRightSideActions.count === 0))
        property bool manual: false

        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
            left: parent.left
            leftMargin: mouseArea.drag.active ? units.gu(4) : 0
        }
        drag {
            target: locked ? null : main
            axis: Drag.XAxis
            minimumX: rightActionsView.visible ? -(rightActionsView.width) : 0
            maximumX: leftActionView.visible ? leftActionView.width : 0
            threshold: root.actionThreshold
        }

        onReleased: {
            // if the mouse reach the safe are we should handle it as full swipe
            if (mouse.x < 0) {
                main.x = -(rightActionsView.width - units.gu(2))
            } else if (root.triggerActionOnMouseRelease && root.activeAction) {
                clickEffect.start()
                triggerAction.start()
            } else {
                root.returnToBounds()
                root.activeAction = null
            }
        }
        onClicked: {
            clickEffect.start()
            if (main.x === 0) {
                root.itemClicked(mouse)
            } else if (main.x > 0) {
                var action = getActionAt(Qt.point(mouse.x, mouse.y))
                if (action && action !== -1) {
                    action.triggered(root)
                }
            } else {
                var actionIndex = getActionAt(Qt.point(mouse.x, mouse.y))
                if (actionIndex !== -1) {
                    root.activeItem = rightActionsRepeater.itemAt(actionIndex)
                    root.activeAction = root._visibleRightSideActions[actionIndex]
                    triggerAction.start()
                    return
                }
            }
            root.resetSwipe()
        }

        onPositionChanged: {
            if (mouseArea.pressed) {
                updateActiveAction()
            }
        }
        onPressAndHold: {
            if (main.x === 0) {
                root.itemPressAndHold(mouse)
            }
        }
        z: -1
    }

    ListItem.ThinDivider {
        id: divider
        visible: false
        width: parent.width + units.gu(4)
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
