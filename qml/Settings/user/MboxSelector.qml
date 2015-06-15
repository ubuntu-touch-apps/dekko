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
    property string disableString
    property string selectedString
    property Action selectAction
    onSelectedIndexChanged: {
        updateSelectedTextString()
    }
    function updateSelectedTextString() {
        switch (selectedIndex) {
        case 0:
            selectedText = never.text;
            break;
        case 1:
            selectedText = oneSec.text;
            break;
        }
    }

    Column {
        id: radioColumn
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        spacing: units.gu(1)
        CheckboxWithLabel {
            id: never
            text: disableString
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
            text: selectedString
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
            AbstractButton {
                height: parent.height
                width: height
                anchors {
                    right: parent.right
                    rightMargin: units.gu(1)
                    top: parent.top
                    topMargin: -units.gu(0.5)
                }
                Icon {
                    name: "edit"
                    anchors.fill: parent
                }
                onClicked: selectAction.trigger()
                visible: selectedIndex === 1
            }
        }
    }
}
