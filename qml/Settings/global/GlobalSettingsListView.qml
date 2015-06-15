import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../Components"

Page {
    id: globalSettings

    flickable: null

    DekkoHeader {
        id: header
        title: qsTr("Settings")
        backAction: settingsBackAction
        primaryAction: Action {
            iconName: "info"
            onTriggered: pageStack.push(Qt.resolvedUrl("../AboutPage.qml"))
        }
    }

    property string previousView

    Action {
        id: settingsBackAction
        iconName: "back"
        onTriggered: {
            pageStack.clear()
            console.log("AccountsManagerCount", dekko.accountsManager.count)
            if (!dekko.accountsManager.count) {
                pageStack.push(Qt.resolvedUrl("../../AccountsView/AccountsPage.qml"))
                return;
            }

            if (previousView === "mbox") {
                pageStack.push(Qt.resolvedUrl("../../MailboxView/MailboxListPage.qml"))
            } else {
                pageStack.push(Qt.resolvedUrl("../../MessageListView/MessageListPage.qml"))
            }
        }
    }

    ListView {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        model: GlobalSettingsListModel {}
    }
}

