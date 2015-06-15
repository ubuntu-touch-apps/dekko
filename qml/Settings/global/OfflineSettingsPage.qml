import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Pickers 1.0
import DekkoCore 0.2
import "../../Components"

Page {
    id: offlineSettings
    flickable: null

    DekkoHeader {
        id: header
        title: qsTr("Offline Settings")
        backAction: Action {
            iconName: "back"
            onTriggered: pageStack.pop()
        }
    }

    Column {
        anchors {
            left: parent.left
            top: header.bottom
            right: parent.right
        }
        spacing: units.gu(1)

        ListItem.Standard {
            text: qsTr("Start offline")
            control: Switch {
                id: startOffline
                onClicked: GlobalSettings.offline.startOffline = checked
                Component.onCompleted: checked = GlobalSettings.offline.startOffline
            }
            showDivider: false
        }
        Label {
            visible: startOffline.checked
            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                right: parent.right
                rightMargin: units.gu(2)
            }
            color: UbuntuColors.red
            text: qsTr("Starting offline may cause the views to not fully load, but re-opening that view will fix it")
        }

        OptionSelector {
            id: cacheOffline
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                right: parent.right
                rightMargin: units.gu(2)
            }
            text: qsTr("Cache Offline")
            model: [qsTr("Do not cache"), qsTr("Cache offline for X days"), qsTr("Cache everything")]
            delegate: OptionSelectorDelegate {
                text: modelData
            }
            onSelectedIndexChanged: {
                switch (selectedIndex) {
                case 0:
                    GlobalSettings.offline.cacheOffline = OfflineSettings.NONE
                    break;
                case 1:
                    GlobalSettings.offline.cacheOffline = OfflineSettings.NUM_DAYS
                    break;
                case 2:
                    GlobalSettings.offline.cacheOffline = OfflineSettings.ALL
                    break;
                }
            }
            Component.onCompleted: {
                selectedIndex = GlobalSettings.offline.cacheOffline
            }
        }

        TitledTextField {
            id: numdays
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                right: parent.right
                rightMargin: units.gu(2)
            }
            title: qsTr("Number of days to cache offline")
            visible: cacheOffline.selectedIndex === 1
            onTextChanged: timer.start()
            Component.onCompleted: text = GlobalSettings.offline.cacheOfflineNumberOfDays
            Timer {
                id: timer
                interval: 500
                onTriggered: {GlobalSettings.offline.cacheOfflineNumberOfDays = numdays.text; stop()}
            }
        }
    }

}
