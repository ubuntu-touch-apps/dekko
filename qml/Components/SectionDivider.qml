import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2

Rectangle {
    property alias text: sectionLabel.text
    property alias showDivider: divider.visible
    anchors {
        left: parent.left
        right: parent.right
    }
    height: sectionLabel.height
    color: Theme.palette.normal.background
    Label {
        id: sectionLabel
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            right: parent.right
            rightMargin: units.gu(2)
        }
        color: Style.common.text
        height: units.gu(4)
        fontSize: "medium"
        font.weight: Font.DemiBold
        verticalAlignment: Text.AlignVCenter
        
    }
    ListItem.ThinDivider {
        id: divider
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
