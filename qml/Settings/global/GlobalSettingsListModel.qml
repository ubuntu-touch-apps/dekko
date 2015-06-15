import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../AccountsView"
import "../../Components"

VisualItemModel {
    id: globSettingsModel

    SectionDivider {
        anchors {
            left: parent.left
            right: parent.right
        }
        text: qsTr("Account settings")
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
        }
        Repeater {
            width: parent.width
            model: dekko.accountsManager
            delegate: AccountsDelegate {
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

                onItemClicked: {
                    pageStack.push(Qt.resolvedUrl("../user/AccountsListPage.qml"), {account: dekko.accountsManager.get(model.index)})
                }

                leftSideAction: Action {
                    text: qsTr("Delete")
                    iconName: "delete"
                    onTriggered: dekko.accountsManager.removeRow(model.index)
                }
            }
        }
    }
    SectionDivider {
        anchors {
            left: parent.left
            right: parent.right
        }
        text: qsTr("Display settings")
    }
    Selector {
        id: markAsReadSelector
        anchors {
            left: parent.left
            right: parent.right
        }
        description: qsTr("Mark messages read")
        contents: MarkReadSelector {
            onSelectedTextChanged: markAsReadSelector.selectedText = selectedText
        }
    }
    Selector {
        id: previewSelector
        anchors {
            left: parent.left
            right: parent.right
        }
        description: qsTr("Show preview")
        contents: PreviewSelector {
            onSelectedTextChanged: previewSelector.selectedText = selectedText
        }
    }
    ListItem.Standard {
        text: qsTr("Prefer plain text messages")
        control: DekkoSwitch {
            id: s
            onClicked: GlobalSettings.preferences.preferPlainText = checked
            Component.onCompleted: checked = GlobalSettings.preferences.preferPlainText
        }
    }
    ListItem.Standard {
        text: qsTr("Auto load images")
        control: DekkoSwitch {
            onClicked: GlobalSettings.preferences.autoLoadImages = checked
            Component.onCompleted: checked = GlobalSettings.preferences.autoLoadImages
        }
    }
    ListItem.Standard {
        text: qsTr("Hide messages marked for deletion")
        control: DekkoSwitch {
            onClicked: GlobalSettings.preferences.hideMarkedDeleted = checked
            Component.onCompleted: checked = GlobalSettings.preferences.hideMarkedDeleted
        }
    }
    ListItem.Standard {
        text: qsTr("Enable avatars")
        control: DekkoSwitch {
            onClicked: GlobalSettings.preferences.gravatarAllowed = checked
            Component.onCompleted: checked = GlobalSettings.preferences.gravatarAllowed
        }
    }

    SectionDivider {
        anchors {
            left: parent.left
            right: parent.right
        }
        text: qsTr("Privacy and security")
    }
    ListItem.Standard {
        text: qsTr("Show you are using Dekko")
        control: DekkoSwitch {
            onClicked: GlobalSettings.preferences.imapEnableId = checked
            Component.onCompleted: checked = GlobalSettings.preferences.imapEnableId
        }
    }

    SectionDivider {
        anchors {
            left: parent.left
            right: parent.right
        }
        text: qsTr("Offline settings")
    }
    ListItem.Standard {
        text: qsTr("Start offline")
        control: DekkoSwitch {
            onClicked: GlobalSettings.offline.startOffline = checked
            Component.onCompleted: checked = GlobalSettings.offline.startOffline
        }
    }
    Selector {
        id: cacheSelector
        anchors {
            left: parent.left
            right: parent.right
        }
        description: qsTr("Cache offline")
        contents: CachePolicySelector {
            onSelectedTextChanged: cacheSelector.selectedText = selectedText
        }
    }
}
