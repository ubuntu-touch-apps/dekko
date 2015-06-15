/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import TrojitaCore 0.1
import DekkoCore 0.2
import "../Utils/Utils.js" as Utils

ListItem.Base {
    id: recipientField

    property alias count: repeater.count
    property int currentIndex: -1
    property variant currentItem;
    property alias delegate: repeater.delegate
    property alias flow: flow.flow
    property alias model: repeater.model
    property alias recipientsListModel: repeater.model
    property alias text: input.text
    property alias placeholderText: input.placeholderText
    property alias hasFocus: input.focus
    // TODO: change to use replymode enum
    property bool replyMode: false
    property var recipientsList
    property string fromName
    property variant fromAddr
    property variant ccAddr
    property variant bccAddr
    property var oneMessageModel
    showDivider: false

    width: parent.width
    height: flow.height + s.height + input.height
    __mouseArea.visible: false

    onCurrentIndexChanged: {
        currentItem = model.get(currentIndex)
    }
    onReplyModeChanged: {
        submissionManager.recipientsModel.buildRecipientList(ReplyMode.REPLY_PRIVATE, oneMessageModel.messageIndex)
    }

    function buildReplyMessage() {
        for (var i = 0; i < fromAddr.length; i++) {
            createAddressObject(formatAddresses(fromAddr[i]), Recipient.ADDRESS_TO)
        }
        if (ccAddr) {
            for (var i = 0; i < ccAddr.length; i++) {
                createAddressObject(formatAddresses(ccAddr[i]), Recipient.ADDRESS_CC)
            }
        }
        if (bccAddr) {
            for (var i = 0; i < bccAddr.length; i++) {
                createAddressObject(formatAddresses(bccAddr[i]), Recipient.ADDRESS_BCC)
            }
        }
    }

    function formatAddresses(text) {
        if (text) {
            var email = text.trim()
            if (validateAddress(email)) {
                input.text = ""
                input.placeholderText = qsTr("Add another recipient");
                return email
            } else {
                PopupUtils.open(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"),
                                dekko,
                                {
                                    title: qsTr("Ooops!"),
                                    text: qsTr("'%1' doesn't seem to be a valid email address").arg(email)
                                }
                                );
                input.activeFocus = true;
            }
        }
    }

    function validateAddress(email) {
        var re = /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/
        return re.test(email);
    }
    Component {
        id: recipient
        Recipient {}
    }

    function createAddressObject(addr, kind) {
        var r = recipient.createObject(this)
        r.recipientAddress = addr
        r.recipientKind = kind
        submissionManager.recipientsModel.appendRow(r)
    }

    function getRecipientKind() {
        return getRecipientKindFromIndex(s.currentIndex);
    }
    function getRecipientKindFromIndex(index) {
        var kind;
        switch (index) {
        case 0:
            kind = Recipient.ADDRESS_TO;
            break;
        case 1:
            kind = Recipient.ADDRESS_CC;
            break;
        case 2:
            kind = Recipient.ADDRESS_BCC;
            break;
        }
        console.log("Kind: " + kind)
        return kind;
    }
    function getIndexFromRecipientKind(recipientKind) {
        var index;
        switch (recipientKind) {
        case Recipient.ADDRESS_TO:
            index = 0;
            break;
        case Recipient.ADDRESS_CC:
            index = 1;
            break;
        case Recipient.ADDRESS_BCC:
            index = 2;
            break;
        }
        return index;
    }

    function resetSelector() {
        s.selectItem("To")
    }

    Column {
        id: column
        anchors {
            left: parent.left
            right:parent.right
            top: parent.top
        }
        Flow {
            id: flow
            width: parent.width
            spacing: units.gu(1)

            Repeater {
                id: repeater
                model: submissionManager.recipientsModel
                delegate: Item {
                    id: addrComp
                    height: units.gu(5)
                    width: row.width + units.gu(2)
                    UbuntuShape {
                        anchors {
                            fill: parent
                            bottomMargin: units.gu(0.5)
                            topMargin: units.gu(0.5)
                        }
                        color: UbuntuColors.warmGrey
                    }
                    Row{
                        id: row
                        width: recipientKindLabel.width + label.width
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        Label {
                            id: recipientKindLabel
                            text: {
                                switch(recipientKind) {
                                case Recipient.ADDRESS_TO:
                                    return "To: ";
                                case Recipient.ADDRESS_CC:
                                    return "Cc: ";
                                case Recipient.ADDRESS_BCC:
                                    return "Bcc: ";
                                }
                            }
                            color: UbuntuColors.darkAubergine
                        }
                        Label {
                            id: label
                            text: recipientAddress
                            color: UbuntuColors.lightAubergine
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: PopupUtils.open(recipientPopover, addrComp)
                    }
                    Component {
                        id: recipientPopover
                        Popover {
                            id: popver
                            Column {
                                width: parent.width
                                ListItem.Standard {
                                    text: qsTr("Delete")
                                    onClicked: {
                                        recipientsListModel.removeRow(index)
                                    }
                                }
                                ListItem.Standard {
                                    text: qsTr("Recipient kind")
                                    control:
                                        RecipientTypeSelectorListView {
                                            width: units.gu(15)
                                            height: units.gu(4)
                                            model: ["To", "Cc", "Bcc"]
                                            currentIndex: getIndexFromRecipientKind(recipientKind);
                                            onCurrentIndexChanged: {
                                                recipientKind = getRecipientKindFromIndex(currentIndex);
                                                PopupUtils.close(popver);
                                            }
                                        }
                                }
                                ListItem.Standard {
                                    text: qsTr("Add to contact")
                                    visible: currentAccount.devModeEnableContacts
                                    onClicked: {
                                        pageStack.push(Qt.resolvedUrl("../Contact/ContactChoicePage.qml"),
                                                        {   mode: "add",
                                                            prettyEmailAddress: recipientAddress
                                                        });
                                        PopupUtils.close(popver);
                                    }
                                }
                            }
                        }
                    }
                }
                onCountChanged: {
                    // filter out the fact that the repeater is at the first index
                    // and let currentIndex display our actual index of the recipient model
                    if (recipientField.currentIndex === -1 && count > 0) {
                        recipientField.currentIndex = 0
                    }
                    if (recipientField.currentIndex >= count) {
                        recipientField.currentIndex = count - 1
                    }
                }
            }
        }

        RecipientTypeSelector {
            id: s
            width: parent.width
            readOnly: false
            visible: (currentIndex != -1)
            active: true
            height: units.gu(4)
            values: ["To", "Cc", "Bcc"]
        }
        Item {
            height: input.height
            width: parent.width
            RecipientInput {
                id: input
                anchors {
                    left: parent.left
                    right: contactButton.left
                }

                onActiveFocusChanged: {
                    if (!activeFocus && text) {
                        createAddressObject(formatAddresses(input.text), getRecipientKind())
                    }
                }
                Keys.onReturnPressed: createAddressObject(formatAddresses(input.text), getRecipientKind())
            }
            ToolbarButton {
                id: contactButton
                visible: false
                //visible: dekko.currentAccount.devModeEnableContacts
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                iconName: "contact"
                MouseArea {
                    anchors.fill: parent
                    visible: parent.visible
                    onClicked: {
                        var contactPage = Qt.createComponent(Qt.resolvedUrl("../Contact/ContactChoicePage.qml")).createObject();
                        contactPage.recipientKindIndex = s.currentIndex;
                        contactPage.contactSelected.connect(function (emailAddress, displayLabel, recipientKindIndex){
                            createAddressObject(displayLabel + " <" + emailAddress + ">", getRecipientKindFromIndex(recipientKindIndex))
                        });
                        pageStack.push(contactPage);
                    }
                }
            }
        }
    }
}
