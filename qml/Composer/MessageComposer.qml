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
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import Ubuntu.Content 0.1
import U1db 1.0 as U1db
import "../Dialogs"
import "../Utils/Utils.js" as Utils
import TrojitaCore 0.1
import DekkoCore 0.2

Flickable {
    id: composer
    property alias replyingToMessage: recipientField.replyMode
    property alias replyMode: msgReplyMode.replyMode
    property QtObject draftsDatabase
    property var docId
    property var toAddresses
    property var ccAddresses
    property var bccAddresses
    property alias message: composerUtils.message
    property bool messageStarted: submissionManager.recipientsModel.count > 0
                                  || recipientField.text
                                  || subjectField.text
    property bool isEditingDraft: false
    property alias _recipientField: recipientField
//    property alias identityModel: identitySelector.model
    property var oneMessageModel
    property bool ccBccEnabled: false
    property bool isOpen

    signal submissionSuceeded

    clip: true
    anchors {
        fill: parent
        topMargin: units.gu(7)
        bottomMargin: units.gu(1)
    }
    contentHeight: composerItemModel.height
    contentWidth: composerItemModel.width

    function loseFocus() {
        recipientField.hasFocus = false
        ccField.hasFocus = false
        bccField.hasFocus = false
        subjectField.hasFocus = false
        messageField.hasFocus = false
    }

    function resetComposer() {
        submissionManager.recipientsModel.clear()
        subjectField.text = ""
        messageField.text = ""
        recipientField.reset();
        ccField.reset();
        bccField.reset();
        ccBccEnabled = false;
        docId = ""
        isEditingDraft = false
        if (replyMode === ReplyMode.REPLY_FORWARD) {
            if (composePage.replyPartId)
                submissionManager.attachmentsModel.addImapPartAttachment(oneMessageModel.messageIndex, dekko.currentAccount.imapModel);
            else
                submissionManager.attachmentsModel.addImapMessageAttachment(oneMessageModel.messageIndex, dekko.currentAccount.imapModel);
            subjectField.text = composerUtils.subject;
            subjectField.cursorPosition = 0;
            if (isOpen)
                recipientField._input.forceActiveFocus();
        } else if (replyMode !== ReplyMode.REPLY_NONE) {
            subjectField.text = composerUtils.subject;
            subjectField.cursorPosition = 0;
            if (dekko.currentAccount.accountId) {
                messageField.text = composerUtils.message
            } else {
                messageField.text = ""
            }
            submissionManager.recipientsModel.buildRecipientList(replyMode, oneMessageModel.messageIndex)
            if (isOpen)
                recipientField._input.forceActiveFocus();
        } else if (composePage.mailtoUrl) {
            mailtoUrl = mailtoUrl.replace(",", ""); // it doesn't work if mailtoUrl contains ","
            var subjectBody = submissionManager.parseMailtoUrl(composePage.mailtoUrl);
            subjectField.text = subjectBody.subject
            messageField.text = subjectBody.body
            if (isOpen)
                messageField._input.forceActiveFocus();
        } else {
            if (isOpen)
                recipientField._input.forceActiveFocus();
        }

        // manually reset this as clearing the model doesn't update the placeholder
        //signatureLabel.text = getSignature()

    }


    function sendEmail() {
        submissionManager.setAccount(getSenderAccount());
        loseFocus()
        // lets do a sanity check and see if there is an address not currently in the list model
        if (recipientField.text) {
            recipientField.createAddressObject(
                        recipientField.formatAddresses(recipientField.text),
                        recipientField.getRecipientKind()
                        )
        }
        if (ccField.text) {
            toField.createAddressObject(
                        ccField.formatAddresses(ccField.text),
                        ccField.getRecipientKind()
                        )
        }
        if (bccField.text) {
            toField.createAddressObject(
                        bccField.formatAddresses(bccField.text),
                        bccField.getRecipientKind()
                        )
        }
        switch (replyMode) {
        case ReplyMode.REPLY_NONE:
            break;
        case ReplyMode.REPLY_ALL:
        case ReplyMode.REPLY_LIST:
        case ReplyMode.REPLY_PRIVATE:
            // if oneMessageModel.messageId is empty, then oneMessageModel contains a embedded message
            if (oneMessageModel.messageId != "") {
                submissionManager.inReplyTo = oneMessageModel.messageId;
                submissionManager.setReferences(oneMessageModel.replyReferences())
                submissionManager.setReplyingToMessage(oneMessageModel.mailbox, oneMessageModel.uid)
            }
            break;
        }

        //TODO save recipients as recent contacts
/*        for (var i = 0; i < submissionManager.recipientsModel.count; i++) {
            var prettyEmailAddress = submissionManager.recipientsModel.get(i).recipientAddress;
            if (prettyEmailAddress.lastIndexOf(" <") > 0) {
                var tmp = prettyEmailAddress.lastIndexOf(" <");
                dekko.addressbookModel.saveEmail(prettyEmailAddress.substr(0, tmp),
                                           prettyEmailAddress.substr(tmp + 2,
                                                prettyEmailAddress.length - tmp - 3));
            } else {
                dekko.addressbookModel.saveEmail("", prettyEmailAddress);
            }
        }*/

        if (submissionManager.setupConnection()) {
            // if we fail building the message the messagebuildingfailed signal will raise the error dialog
            // so nothing more to do here.
            if (submissionManager.buildMessage(senderIdentityModel.get(identitySelector.selectedIndex).formattedString,
                                               subjectField.text,
                                               messageField.text + getSignature())) {
                submissionManager.send();
                showStatus();
                //console.log("SELECTED INDEX = " + identitySelector.selectedIndex)
            }
        }
    }

    function showStatus() {
        PopupUtils.open(statusDialog)
    }

    function saveNewDraft() {
        var toAddr = new Array()
        var ccAddr = new Array()
        var bccAddr = new Array()
        for (var i =0; i < submissionManager.recipientsModel.count; i++) {
            var kind = submissionManager.recipientsModel.get(i).recipientKind
            var addr = submissionManager.recipientsModel.get(i).recipientAddress
            switch (kind) {
            case Recipient.ADDRESS_TO:
                toAddr.push(addr)
                break
            case Recipient.ADDRESS_CC:
                ccAddr.push(addr)
                break
            case Recipient.ADDRESS_BCC:
                bccAddr.push(addr)
                break
            }
        }
        draftsDatabase.putDoc(
                    {
                        to: toAddr,
                        cc: ccAddr,
                        bcc: bccAddr,
                        recipientFieldText: recipientField.text,
                        subject: subjectField.text,
                        message: messageField.text,
                        date: Qt.formatDate(new Date(), "dd/MM/yyyy"),
                        timestamp: Qt.formatTime(new Date(), "hh:mm")
                    }
                    )
    }

    function openDraft(docId) {
        resetComposer()
        var doc = draftsDatabase.getDoc(docId)
        var hasRecipients = false;
        if (doc.to !== undefined) {
            recipientField.fromAddr = doc.to
            hasRecipients = true;
        }
        if (doc.cc !== undefined) {
            recipientField.ccAddr = doc.cc
            hasRecipients = true
        }
        if (doc.bcc !== undefined) {
            recipientField.bccAddr = doc.bcc
            hasRecipients = true
        }
        if (hasRecipients) {
            recipientField.buildReplyMessage()
        }
        recipientField.text = doc.recipientFieldText
        subjectField.text = doc.subject
        messageField.text = doc.message
        draftsPanel.close()
    }
   //TODO: FIXME!!!
    function updateDraft() {
        // let's just delete the current doc and save a new one
        draftsDatabase.deleteDoc(docId)
        saveNewDraft()
    }
    //TODO implement signature
    function getSignature() {
        var includeSig = mboxSettings.submission.includeSignature
        if (includeSig === "new" && replyMode !== ReplyMode.REPLY_NONE) {
            return ""
        } else if (includeSig === "never") {
            return ""
        }
        /* we want to force two empty lines then sig seperator then the signature
          So we always want to try and have two clear lines above the signature seperator

          >\n (initial break from message quote if ReplyMode.REPLY_ALL)
          \n (1st line)
          \n (2nd line)
          -- \n (3rd line, don't forget signature seperator requires a single space padding after the '--')
          <signature> (followed by signature)
        */
        if (senderIdentityModel.get(identitySelector.selectedIndex).signature) {
            var sigSetup = "\n\n-- \n"
            if (replyMode === ReplyMode.REPLY_ALL) {
                sigSetup = "\n" + sigSetup
            }
            return sigSetup  + senderIdentityModel.get(identitySelector.selectedIndex).signature
        } else {
            console.log("No signature for identity. Leaving Empty")
            return ""
        }
    }

    function buildingFailed(msg) {
        PopupUtils.open(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"), dekko, {
                            title: qsTr("Oops!"),
                            text: msg,
                            buttonText: qsTr("Close")
                        }
                        );
    }

    function getSenderAccount() {
        return dekko.accountsManager.getFromId(
                    senderIdentityModel.getAccountIdForIdentityIdx(
                        identitySelector.selectedIndex))
    }

    function positionViewAtIndex(idx) {
        console.log("TODO positionViewAtIndex", idx);
    }

    // FIXME: this is a workaround for the mailbox list view composer
    // We have to wait for the identity model to get populated fully so we can
    // Set the signature label correctly and the identity selector has a selected index
//    Connections {
//        target: currentAccount.identityModel
//        onCountChanged: {
//            if (count > 0) {
//                identityModel = []
//                identitySelector.model = currentAccount.identityModel
//                identitySelector.selectedIndex = 0
//            }
//        }
//    }

    ReplyMode { id: msgReplyMode }

    MailboxSettings {
        id: mboxSettings
        account: dekko.currentAccount
        mailboxName: dekko.currentAccount.msgListModel.mailboxName
    }

    ComposerUtils {
        id: composerUtils
        // force a new line here
        fromName: oneMessageModel ?
                      Utils.formatPlainArrayMailAddresses(oneMessageModel.from, true, false).toString() :
                      undefined
        date: oneMessageModel ? oneMessageModel.date : undefined
        subject: oneMessageModel ? oneMessageModel.subject : ""
        replyMode: composer.replyMode
        onMessageChanged: {
            if (replyMode != ReplyMode.REPLY_FORWARD)
                messageField.text = composerUtils.message + "\n"
        }
        onSubjectChanged: subjectField.text = composerUtils.subject
        quoteOriginalMessage: mboxSettings.submission.quoteOriginalMessage
        replyPosition: mboxSettings.submission.replyQuotePosition
    }

    Column {
        id: composerItemModel
        width: composer.width
        Column {
            id: headerColumn
            width: parentWidth
            Item {
                id: toReciptItem
                width: composer.width
                height: recipientField.height
                TypedRecipientField {
                    id: recipientField
                    recipientKind: Recipient.ADDRESS_TO
                    displayInput: !messageField.hasActiveFocus || count == 0;
                    anchors {
                        left: parent.left
                        right: ccBccToggle.left
                    }
                    showDivider: false
                    collapsed: !activeFocus
                    MouseArea {
                        id: recipientFieldAutoFocusMouseArea
                        anchors.fill: parent
                        enabled: false
                        onClicked: {
                            recipientField._input.forceActiveFocus();
                            recipientField._input.focus = true;
                        }
                        // Binding enabled to messageField.hasActiveFocus doesn't work
                        // because active focus change come before mouse event checking.
                        // So we can only disable this MouseArea a while after
                        // messageField.hasActiveFocus change
                        function disableLater() {
                            recipientFieldAutoFocusMouseAreaDisableTimer.start();
                        }
                        Timer {
                            id: recipientFieldAutoFocusMouseAreaDisableTimer
                            repeat: false
                            interval: 10
                            onTriggered: recipientFieldAutoFocusMouseArea.enabled = false;
                        }
                    }
                }
                Label {
                    id: ccBccToggle
                    text: (!ccField.visible ? "CC" : "")
                          + (!ccField.visible && !bccField.visible ? "/" : "")
                          + (!bccField.visible ? "BCC" : "")
                    visible: (!ccField.visible || !bccField.visible) && !messageField.hasActiveFocus
                    width: visible ? implicitWidth : 0
                    anchors {
                        right: parent.right
                        top: parent.top
                        margins: units.gu(2)
                    }

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -units.gu(1)
                        onClicked: {
                            ccBccEnabled = true;
                        }
                    }
                }
                ListItem.ThinDivider {
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.bottom
                    }
                }
            }
            Item {
                id: hideOnEditItem
                width: composer.width
                height: messageField.hasActiveFocus ? 0 : hideOnEditColumn.height
                clip: true
                Behavior on height { UbuntuNumberAnimation {} }
                Column {
                    id: hideOnEditColumn
                    width: composer.width

                    IdentitySelector {
                        id: identitySelector
                        width: composer.width
                        model: MultipleAccountSenderIdentityModel {
                            id: senderIdentityModel
                            accountIds: dekko.accountsManager.accountIds
                        }
                        delegate: selectorDelegate
                        selectedIndex: senderIdentityModel.getFirstIdentityIdxForAccountId(dekko.currentAccount.accountId)
                        Component {
                            id: selectorDelegate
                            OptionSelectorDelegate {
                                text: model.name;
                                subText: model.email
                                onClicked: identitySelector.expanded = false
                            }
                        }
                    }
                }
            }

            TypedRecipientField {
                id: ccField
                width: composer.width
                recipientKind: Recipient.ADDRESS_CC
                visible: (ccBccEnabled | count > 0) && !messageField.hasActiveFocus
                height: visible ? contentHeight : 0
                showDivider: visible
                collapsed: !activeFocus
                onRequestVisible: {
                    composer.positionViewAtIndex(2, ListView.Contain)
                }
            }

            TypedRecipientField {
                id: bccField
                width: composer.width
                recipientKind: Recipient.ADDRESS_BCC
                visible: (ccBccEnabled | count > 0) && !messageField.hasActiveFocus
                height: visible ? contentHeight : 0
                showDivider: visible
                collapsed: !activeFocus
                onRequestVisible: {
                    composer.positionViewAtIndex(3, ListView.Contain)
                }
            }

            SubjectField{
                id: subjectField
                width: composer.width
                height: units.gu(6)
                placeholderText: qsTr("Enter Subject")
                label: qsTr("Subject")
            }

            AttachmentField {
                width: composer.width
            }
        }

        MessageField {
            id: messageField
            width: composer.width
            placeholderText: qsTr("Enter Message")
            onHasActiveFocusChanged: {
                if (hasActiveFocus) {
//                    positionViewAtIndex(5, ListView.Beginning)
                    recipientFieldAutoFocusMouseArea.enabled = true;
                } else {
                    console.log("Message field lost active focus")
                    ccBccEnabled = false;
                    recipientFieldAutoFocusMouseArea.disableLater();
                }
            }
            onKeyPressed: {
                var targetY = -1;
                var p = messageField.getRectPos().y
                var positonOnScreen = p - (composer.contentY - headerColumn.height);
                if (positonOnScreen < 0) {
                    targetY = headerColumn.height + p;
                } else if (positonOnScreen > composer.height - units.gu(4)) {
                    targetY = headerColumn.height + p - (composer.height - units.gu(4))
                }

                if (targetY >= 0 && targetY < composer.contentHeight) {
                    composer.contentY = targetY;
                }
                composer.returnToBounds();
            }
        }

//        Item {
//            width: parent.width
//            height: signatureLabel.height
//            Label {
//                id: signatureLabel
//                anchors {
//                    left: parent.left
//                    leftMargin: units.gu(2)
//                    rightMargin: units.gu(2)
//                    right: parent.right
//                }
//                text: (bottomEdge.opened && text === "") ? signatureLabel.text = getSignature() : ""
//            }
//        }

        // empty item to position view to on active focus
        // if messageField visible area is greater than flickable visible area
        Item {
            height: units.gu(5)
            width: composer.width
        }
    }

    Component {
        id: statusDialog
        SubmissionStatusDialog {
            id: submissionStatusDialog
            onCloseClicked: {
                if (succeeded) {
                    submissionSuceeded()
                    //identitySelector.selectedIndex = 0
                    succeeded = false
                }
                PopupUtils.close(submissionStatusDialog)
            }
        }
    }

    Component.onCompleted: {
        submissionManager.messageBuildFailed.connect(buildingFailed)
        submissionManager.passwordRequested.connect(function (user, host) {
            dekko.submissionManager = submissionManager;
            passwordManager.requestPassword(getSenderAccount().accountId, PasswordManager.SMTP);
        });
        submissionManager.gotPassword.connect(function () { console.log("Password accepted.") })
    }
}
