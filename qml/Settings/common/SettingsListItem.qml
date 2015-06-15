import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../Components"

ListItem.Empty {
    property alias text: text.text
    property alias subText: subText.text
    Item {
        height: parent.height
        anchors { fill: parent; topMargin: subText.text ? units.gu(1) : units.gu(2); bottomMargin: units.gu(1); leftMargin: units.gu(2); rightMargin: units.gu(2)}
        
        Label {
            id: text
            anchors { left: parent.left; right: parent.right; top: parent.top}
            fontSize: "medium"
            color: Style.common.text
        }
        Label {
            id: subText
            anchors { left: parent.left; right: parent.right; top: text.bottom}
            fontSize: "small"
            color: Qt.lighter(Style.common.text)
        }
    }
}
