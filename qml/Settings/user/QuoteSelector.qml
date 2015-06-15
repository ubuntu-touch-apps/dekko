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
            if (selectedIndex === 0) {
                mboxSettings.write(mboxSettings.keys.replyQuotePosition, "before")
            } else if (selectedIndex === 1) {
                mboxSettings.write(mboxSettings.keys.replyQuotePosition, "after")
            } else if (selectedIndex === 2) {
                mboxSettings.write(mboxSettings.keys.replyQuotePosition, "inline")
            }
        }
        updateSelectedTextString()
    }
    function updateSelectedTextString() {
        switch (selectedIndex) {
        case 0:
            selectedText = hide.text;
            break;
        case 1:
            selectedText = oneLine.text
            break
        case 2:
            selectedText = twoLines.text
        }
    }

    Component.onCompleted: {

        var position = mboxSettings.read(mboxSettings.keys.replyQuotePosition, "before")
        if (position === "before") {
            selectedIndex = 0
        } else if (position === "after") {
            selectedIndex = 1
        } else if (position === "inline") {
            selectedIndex = 2
        } else {
            selectedIndex = 0
        }
        mboxSettings.dataChanged.connect(updateSelectedTextString)
    }

    MailboxSettings {
        id: mboxSettings
        account: root.account
        mailboxName: "global"
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
            id: hide
            text: qsTr("Above quoted message")
            property bool isSelected: selectedIndex === 0
            checked: selectedIndex === 0
            onCheckedChanged: if (checked) {selectedIndex = 0}
                              else if (selectedIndex === 0) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    hide.checked = hide.isSelected
                }
            }
        }
        CheckboxWithLabel {
            id: oneLine
            text: qsTr("Below quoted message")
            property bool isSelected: selectedIndex === 1
            checked: selectedIndex === 1
            onCheckedChanged: if (checked) {selectedIndex = 1}
                              else if (selectedIndex === 1) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    oneLine.checked = oneLine.isSelected
                }
            }
        }
        CheckboxWithLabel {
            id: twoLines
            text: qsTr("Inline")
            property bool isSelected: selectedIndex === 2
            checked: selectedIndex === 2
            onCheckedChanged: if (checked) {selectedIndex = 2}
                              else if (selectedIndex === 2) {checked = true;}
            Connections {
                target: radioSelector
                onSelectedIndexChanged: {
                    twoLines.checked = twoLines.isSelected
                }
            }
        }
    }
}
