import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import "../../Components"

Page {
    id: compositionPage

    flickable: null

    DekkoHeader {
        id: header
        title: qsTr("Composition")
        backAction: Action {
            iconName: "back"
            onTriggered: pageStack.pop()
        }
    }

    Label {
        anchors.centerIn: parent
        text: "Not implemented"
    }
}
