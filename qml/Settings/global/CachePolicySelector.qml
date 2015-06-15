import QtQuick 2.3
import Ubuntu.Components 1.1
import "../../Components"
import DekkoCore 0.2

Item {
    id: radioSelector
    anchors {left: parent.left; right: parent.right}
    height: radioColumn.height
    property string selectedText
    property int selectedIndex: -1
    onSelectedIndexChanged: {
        if (selectedIndex !== -1) {
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
        updateSelectedTextString()
    }
    function updateSelectedTextString() {
        switch (selectedIndex) {
        case 0:
            selectedText = none.text;
            break;
        case 1:
            selectedText = afterXDays.displayString
            break
        case 2:
            selectedText = all.text
        }
    }

    Component.onCompleted: {
        selectedIndex = GlobalSettings.offline.cacheOffline
        GlobalSettings.offline.offlineSettingsChanged.connect(updateSelectedTextString)
    }

    Column {
        id: radioColumn
        anchors {
            left: parent.left
//            leftMargin: units.gu(1)
            top: parent.top
            right: parent.right
        }

        spacing: units.gu(1)
        CheckboxWithLabel {
            id: none
            text: qsTr("Never")
            property bool isSelected: selectedIndex === 0
            checked: selectedIndex === 0
            onCheckedChanged: if (checked) {selectedIndex = 0}
                              else if (selectedIndex === 0) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    none.checked = none.isSelected
                }
            }
        }
        Item {
            id: afterXDays
            property string displayString: "%1 %2 %3".arg(label.text).arg(GlobalSettings.offline.cacheOfflineNumberOfDays).arg(label2.text)
            width: parent.width
            height: none.height
            property alias text: label.text
            ListItemWithActionsCheckBox {
                id: checkbox
                anchors.left: parent.left
                property bool isSelected: selectedIndex === 1
                checked: selectedIndex === 1
                onCheckedChanged: if (checked) {checked = true; selectedIndex = 1}
                                  else if (selectedIndex === 1) {checked = true;}
                isMultiSelect: false
                Connections {
                    target: radioSelector
                    onSelectedIndexChanged: {
                        checkbox.checked = checkbox.isSelected
                    }
                }
            }
            Row {
                anchors {
                    left: checkbox.right
                    leftMargin: units.gu(1)
                    top: parent.top
                    right: parent.right
                }
                height: checkbox.height
                spacing: units.gu(1)

                Label {
                    id: label
                    text: qsTr("Cache for")
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                }
                TextField {
                    id: txtField
                    width: units.gu(8)
                    height: checkbox.height
                    anchors.verticalCenter: parent.verticalCenter
                    onAccepted: focus = false
                    onTextChanged: GlobalSettings.offline.cacheOfflineNumberOfDays = numdays.text
                    Component.onCompleted: text = GlobalSettings.offline.cacheOfflineNumberOfDays
                }
                Label {
                    id: label2
                    text: qsTr("days")
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                }
            }

        }
        CheckboxWithLabel {
            id: all
            text: qsTr("Forever")
            property bool isSelected: selectedIndex === 2
            checked: selectedIndex === 2
            onCheckedChanged: if (checked) {checked = true; selectedIndex = 2}
                              else if (selectedIndex === 2) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    all.checked = all.isSelected
                }
            }
        }
    }
}
