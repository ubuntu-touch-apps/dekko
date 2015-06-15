import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0
import DekkoCore 0.2

Base{
    id: root
    height: expanded ? col.height : fromField.height
    property alias model: identities.model
    property alias delegate: identities.delegate
    property alias selectedIndex: identities.selectedIndex
    property bool expanded: false
    Behavior on height {
        UbuntuNumberAnimation {}
    }

    __mouseArea.visible: false
    property alias hasFocus: input.focus
    onHasFocusChanged: expanded = hasFocus
    Column {
        id: col
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        width: composer.width

        Item {
            id: fromField
            width: root.width
            height: units.gu(6)
            Label {
                id: label
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                text: qsTr("From") + " : "
            }

            Label {
                id: input
                anchors {
                    left: label.right
                    verticalCenter: parent.verticalCenter
                    margins: units.gu(1)
                    right: parent.right
                }
                font.bold: true
                text: senderIdentityModel.get(identities.selectedIndex).name
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.expanded = !root.expanded
            }
        }

        ItemSelector {
            id: identities
            expanded: true
            visible: root.expanded
            anchors {
                left: parent.left
                right: parent.right
            }
            // show maximum of 4
            containerHeight: itemHeight * 4
            onSelectedIndexChanged: {
                root.expanded = false
                input.text = senderIdentityModel.get(identities.selectedIndex).name
            }

        }
    }

}

