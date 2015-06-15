import QtQuick 2.3
import Ubuntu.Components 1.1
import "../../Components"
import DekkoCore 0.2

Item {
    id: radioSelector
    anchors {left: parent.left; right: parent.right}
    height: radioColumn.height
    property string selectedText
    property int selectedIndex: 0
    onSelectedIndexChanged: {
        switch (selectedIndex) {
        case 0:
            GlobalSettings.preferences.markAsRead = selectedIndex
            break;
        case 1:
        case 2:
        case 3:
            GlobalSettings.preferences.markAsRead = 1
            GlobalSettings.preferences.markAsReadAfter = parseInt(selectedIndex)
            break;
        case 4:
            GlobalSettings.preferences.markAsRead = 2
            break;
        }
        updateSelectedTextString()
    }
    function updateSelectedTextString() {
        switch (selectedIndex) {
        case 0:
            selectedText = never.text;
            break;
        case 1:
            selectedText = oneSec.text
            break;
        case 2:
            selectedText = twoSec.text
            break;
        case 3:
            selectedText = threeSec.text
            break;
        case 4:
            selectedText = immediately.text
            break;
        }
    }

    Component.onCompleted: {
        switch (GlobalSettings.preferences.markAsRead) {
        case 0:
            selectedIndex = GlobalSettings.preferences.markAsRead
            break;
        case 1:
            selectedIndex = GlobalSettings.preferences.markAsReadAfter
            break;
        case 2:
            selectedIndex = 4
            break;
        }
        updateSelectedTextString()
        GlobalSettings.preferences.preferencesChanged.connect(updateSelectedTextString)
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
            id: never
            text: qsTr("Never")
            property bool isSelected: selectedIndex === 0
            checked: selectedIndex === 0
            onCheckedChanged: if (checked) {selectedIndex = 0}
                              else if (selectedIndex === 0) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    never.checked = never.isSelected
                }
            }
        }
        CheckboxWithLabel {
            id: oneSec
            text: qsTr("After 1 second")
            property bool isSelected: selectedIndex === 1
            checked: selectedIndex === 1
            onCheckedChanged: if (checked) {selectedIndex = 1}
                              else if (selectedIndex === 1) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    oneSec.checked = oneSec.isSelected
                }
            }
        }
        CheckboxWithLabel {
            id: twoSec
            text: qsTr("After 2 seconds")
            property bool isSelected: selectedIndex === 2
            checked: selectedIndex === 2
            onCheckedChanged: if (checked) {selectedIndex = 2}
                              else if (selectedIndex === 2) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    twoSec.checked = twoSec.isSelected
                }
            }
        }
        CheckboxWithLabel {
            id: threeSec
            text: qsTr("After 3 seconds")
            property bool isSelected: selectedIndex === 3
            checked: selectedIndex === 3
            onCheckedChanged: if (checked) {selectedIndex = 3}
                              else if (selectedIndex === 3) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    threeSec.checked = threeSec.isSelected
                }
            }
        }

        CheckboxWithLabel {
            id: immediately
            text: qsTr("Immediately")
            property bool isSelected: selectedIndex === 4
            checked: selectedIndex === 4
            onCheckedChanged: if (checked) {checked = true; selectedIndex = 4}
                              else if (selectedIndex === 4) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    immediately.checked = immediately.isSelected
                }
            }
        }
    }
}
