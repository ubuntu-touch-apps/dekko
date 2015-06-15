import QtQuick 2.3
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2

ListItem.Expandable {
    id: expandingItem1
    expandedHeight: contentCol1.height + units.gu(1)

    property alias selectedText: selectedString.text
    property alias description: description.text

    property alias contents: contentLoader.sourceComponent

    __mouseArea.visible: false

    Column {
        id: contentCol1
        anchors { left: parent.left; right: parent.right }

        AbstractButton {
            anchors { left: parent.left; right: parent.right}
            height: expandingItem1.collapsedHeight
            onClicked: {
                expanded = !expanded;
            }
            Item {
                height: parent.height
                anchors { left: parent.left; right: icon.left}

                Label {
                    id: description
                    anchors { left: parent.left; right: parent.right; top: parent.top; topMargin: expanded ? units.gu(2) : units.gu(1)}
                    fontSize: "medium"
                    Behavior on anchors.topMargin {
                        UbuntuNumberAnimation{}
                    }

                }
                Label {
                    id: selectedString
                    anchors { left: parent.left; right: parent.right; top: description.bottom}
                    opacity: !expanded && text ? 1 : 0
                    fontSize: "small"
                    color: Qt.lighter(Style.common.text)
                    Behavior on opacity {
                        UbuntuNumberAnimation{}
                    }
                }
            }
            Icon {
                id: icon
                anchors {
                    right: parent.right
                    rightMargin: units.gu(1)
                    verticalCenter: parent.verticalCenter
                }
                name: "down"
                color: Style.common.text
                rotation: expanded ? 180 : 0
                height: units.gu(2); width: height
                state: expanded ? "rotated" : "normal"
                states: [
                    State {
                        name: "rotated"
                        PropertyChanges { target: icon; rotation: 180 }
                    },
                    State {
                        name: "normal"
                        PropertyChanges { target: icon; rotation: 0 }
                    }
                ]
                transitions: Transition {
                    RotationAnimation { duration: UbuntuAnimation.FastDuration; direction: icon.state === "normal"  ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise }
                }

            }

        }

        Loader {
            id: contentLoader
            anchors {left: parent.left; right: parent.right}
        }
    }
}
