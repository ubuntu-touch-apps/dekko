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
    property alias delegate: repeater.delegate
    property alias flow: flow.flow
    property alias model: repeater.model
    property alias recipientsListModel: recipientFilterModel.sourceModel
    property alias text: input.text
    property alias placeholderText: input.placeholderText
    property alias hasFocus: input.focus
    property alias _input: input
    // TODO: change to use replymode enum
    property bool replyMode: false
    property var recipientsList
    property string fromName
    property variant fromAddr
    property variant ccAddr
    property variant bccAddr
    property var oneMessageModel
    property var recipientKind;
    property bool displayInput: true
    property real contentHeight: collapsed ? units.gu(6) : Math.max(units.gu(6), flow.height + recipientHintList.height + units.gu(2))
    property bool collapsed: false
    property int showedRecipientsCount: 0
    property var _errorPopover: false

    signal requestVisible;

    width: parent.width
    height: contentHeight
    __mouseArea.visible: false
    clip: true

    onReplyModeChanged: {
        submissionManager.recipientsModel.buildRecipientList(ReplyMode.REPLY_PRIVATE, oneMessageModel.messageIndex)
    }
    onCollapsedChanged: {
        _layoutBasedOnCollapsed();
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
                input.focus = false;
                if (!_errorPopover) {
                    _errorPopover = PopupUtils.open(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"),
                               dekko,
                               {
                                   title: qsTr("Ooops!"),
                                   text: qsTr("'%1' doesn't seem to be a valid email address").arg(input.text)
                               }
                               );
                    _errorPopover.close.connect(function() {
                       _errorPopover = false;
                       focusInputTimer.start();
                    });
                }
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

    function reset() {
        placeholderText = qsTr("Enter recipient address")
        text = "";
    }

    function getFilterRole() {
        switch (recipientKind) {
        case Recipient.ADDRESS_TO:
            return RecipientListFilterModel.FILTER_TO;
        case Recipient.ADDRESS_CC:
            return RecipientListFilterModel.FILTER_CC;
        case Recipient.ADDRESS_BCC:
            return RecipientListFilterModel.FILTER_BCC;
        }
    }

    function getRecipientKindName() {
        switch (recipientKind) {
        case Recipient.ADDRESS_TO:
            return "To";
        case Recipient.ADDRESS_CC:
            return "Cc";
        case Recipient.ADDRESS_BCC:
            return "Bcc";
        }
    }

    function _layoutBasedOnCollapsed() {
        if (!collapsed) {
            flow.width = flow.fullWidth
            for (var i = 0; i < repeater.count; i++) {
                repeater.itemAt(i).visible = true;
            }
        } else {
            var flowMaximalWidth = flow.fullWidth - moreLabel.implicitWidth - units.gu(1);
            //guess showed recipients count
            var widthSum = 0;
            var i = 0;
            for (; i < repeater.count; i++) {
                if (widthSum  + repeater.itemAt(i).width + flow.spacing > flowMaximalWidth) {
                    break;
                }
                widthSum += repeater.itemAt(i).width + flow.spacing;
            }
            if (widthSum == 0) {
                flow.width = flowMaximalWidth;
                showedRecipientsCount = 1;
            } else {
                flow.width = widthSum;
                showedRecipientsCount = i;
            }
            if (showedRecipientsCount == repeater.count - 1) {
                //If last email can fit in the first line, there is no need for "& ... More"
                if (repeater.itemAt(repeater.count - 1).width + widthSum + units.gu(1) < flow.fullWidth) {
                    flow.width = flow.fullWidth
                    showedRecipientsCount = repeater.count;
                }
            }

            for (i = 0; i < repeater.count; i++) {
                if (i >= showedRecipientsCount) {
                    repeater.itemAt(i).visible = false
                }
            }
        }
    }

    Behavior on height { UbuntuNumberAnimation { } }

    RecipientListFilterModel {
        id: recipientFilterModel
        sourceModel: submissionManager.recipientsModel
        filterRole: getFilterRole()
    }

    Item {
        id: typeLabelContainer
        height: units.gu(6)
        width: typeLabel.width
        Label {
            id: typeLabel
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            text: getRecipientKindName() + " : "
        }
    }

    Flow {
        id: flow
        property int fullWidth: parent.width - typeLabel.width - units.gu(1)
        width: fullWidth
        anchors {
            left: typeLabelContainer.right
            top: parent.top
            topMargin: units.gu(1.5)
            leftMargin: units.gu(1)
        }
        spacing: units.gu(1)
        Repeater {
            id: repeater
            model: recipientFilterModel
            delegate: Item {
                id: addrComp
                height: units.gu(3)
                width: label.width
                Label {
                    id: label
                    property int maximalWidth: flow.fullWidth - units.gu(3) - (collapsed && repeater.count > 1?  moreLabel.width : 0)
                    text: Utils.abbreviateNameForPrettyAddress(recipientAddress) + (index == repeater.count - 1 || (collapsed && index == showedRecipientsCount - 1) ? "" : ",")
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                    elide: Text.ElideRight
                    width: implicitWidth > maximalWidth ? maximalWidth : implicitWidth
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        PopupUtils.open(recipientPopoverComponent, addrComp,
                                        {
                                            emailAddress: Utils.emailAddressFromPrettyAddress(recipientAddress),
                                            recipientIndex: recipientIndex
                                        });
                    }
                }
            }
            onCountChanged: _layoutBasedOnCollapsed();
        }
        Item {
            width: displayInput ? input.width : 0
            height: displayInput ? units.gu(4) : 0
            clip: true
            Behavior on height { UbuntuNumberAnimation { } }
            RecipientInput {
                id: input
                anchors {
                    top: parent.top
                    topMargin: units.gu(0.5)
                }
                onTextChanged: {
                    flattenContactFilterModel.setFilterFixedString(input.text)
                    flattenContactFilterModel.invalidate();
                    requestVisibleTimer.start()
                }

                onActiveFocusChanged: {
                    if (activeFocus) {
                        showHintTimer.start();
                    } else {
                        hideHintTimer.start();
                    }

                    if (!activeFocus && text) {
                        createAddressFromInputTextTimer.start();
                    }
                }
                Keys.onReturnPressed: createAddressObject(formatAddresses(input.text), recipientKind)
            }
        }
    }

    Label {
        id: moreLabel
        width: collapsed ? paintedWidth : 0
        visible: collapsed && (repeater.count - showedRecipientsCount > 0)
        text: "&amp; <b>%1 More</b>".arg(repeater.count - showedRecipientsCount)
        anchors {
            left: flow.right
            verticalCenter: parent.verticalCenter
        }
    }

    ListView {
        id: recipientHintList
        anchors.top: flow.bottom
        width: parent.width
        visible: false
        height: (visible && input.text.length > 0) ? Math.min(3, count) * units.gu(6) : 0
        model: flattenContactFilterModel
        interactive: false
        clip: true
        delegate: ListItem.Subtitled {
            text: name
            subText: email
            onClicked: {
                createAddressFromInputTextTimer.stop();
                var prettyAddress;
                if (name) {
                    prettyAddress = name + " <" + email + ">";
                } else {
                    prettyAddress = email;
                }
                createAddressObject(prettyAddress, recipientKind);
                input.text = "";
                input.forceActiveFocus();
            }
        }
    }

    Timer {
        id: createAddressFromInputTextTimer
        interval: 500
        repeat: false
        onTriggered: {
            createAddressObject(formatAddresses(input.text), recipientKind)
        }
    }

    Timer {
        id: showHintTimer
        interval: 500
        repeat: false
        onTriggered: recipientHintList.visible = true
    }

    Timer {
        id: hideHintTimer
        interval: 500
        repeat: false
        onTriggered: recipientHintList.visible = false
    }
    Timer {
        // wait for animation to settle and display the whole thing including the contact hint
        id: requestVisibleTimer
        interval: 500
        repeat: false
        onTriggered: recipientField.requestVisible()
    }
    Timer {
        id: focusInputTimer
        interval: 500
        repeat: false
        onTriggered: input.forceActiveFocus();
    }

    Component {
        id: recipientPopoverComponent
        Popover {
            id: recipientPopover
            property string emailAddress
            property int recipientIndex
            Column {
                width: parent.width
                ListItem.Header {
                    text: recipientPopover.emailAddress
                }
                ListItem.Standard {
                    text: qsTr("Remove")
                    onTriggered: recipientsListModel.removeRow(recipientPopover.recipientIndex)
                }
            }
        }
    }
}
