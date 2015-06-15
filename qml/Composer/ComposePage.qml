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
import Ubuntu.Components.Popups 1.0
import "../Composer"
import "../Dialogs"
import "../Components"
import TrojitaCore 0.1
import DekkoCore 0.2

Page {
    id: composePage
    property alias composerReplyMode: composer.replyingToMessage
    property alias replyMode: composer.replyMode
    property alias replyUid: replyToMessageModel.uid
    property alias replyMboxName: replyToMessageModel.mailbox
    property string replyPartId
    property string pageTitle: qsTr("Message")
    property Item pageTools
    property var initialReplyMode
    property string mailtoUrl
    property alias isOpen: composer.isOpen
    property bool isBottomEdgeMode: true

    flickable: null

    onIsOpenChanged: {
        if (isOpen) {
            flattenContactModel.update();
        }
    }

    DekkoHeader {
        id: header
        title: draftsPanel.opened ? qsTr("Drafts") : pageTitle
        backAction: closeAction
        primaryAction: sendAction
        secondaryActions: draftsAction
        pageFlickable: composer
    }

    Action {
        id: draftsAction
        text: qsTr("Drafts")
        onTriggered: {
            draftsPanel.composer = true
            draftsPanel.open()
        }
        iconName: "edit"
    }

    Action {
        id: sendAction
        text: qsTr("Send")
        onTriggered: {
            composer.sendEmail()
        }

        iconSource: Qt.resolvedUrl("qrc:///actions/send.svg")
    }

    Action {
        id: closeAction
        text: qsTr("Cancel")
        onTriggered: {
            if (draftsPanel.opened) {
                draftsPanel.close()
            } else {

                Qt.inputMethod.hide()
                composer.loseFocus();
                //TODO: FIXME: only saving drafts for new messages atm
                if (composer.messageStarted) {
                    PopupUtils.open(draftsDialog, dekko, {isEditingDraft: composer.docId})
                } else {
                    closePanelTimer.start()
                }
            }
        }
        iconName: draftsPanel.opened || !isBottomEdgeMode ? "back" : "down"
    }

    Component.onCompleted: reloadComposer();
    function closeBottomEdge() {
        pageStack.pop();
    }

    function openDrafts() {
        draftsPanel.open()
    }

    function reloadComposer() {
        composer.resetComposer()
    }

    function writeTo(address) {
        if (replyMode != ReplyMode.REPLY_NONE) {
            initialReplyMode = replyMode;
            replyMode = ReplyMode.REPLY_NONE;
        }
        reloadComposer();
        composer._recipientField.createAddressObject(address, Recipient.ADDRESS_TO)
    }

    function addAttachmentsFromUrls(files) {
        for (var i in files) {
            submissionManager.attachmentsModel.addFileAttachment(files[i].url.toString().replace("file://", ""));
        }
    }

    function addAttachment(file, caption) {
        submissionManager.attachmentsModel.addFileAttachment(file);
    }

    SubmissionManager {
        id: submissionManager
    }

    state: "panelOpen"
    states: [
        State {
            name: "panelOpen"
        },
        State {
            name: "draftsOpen"
        }
    ]

    Timer {
        id: closePanelTimer
        interval: 500
        onTriggered: closeBottomEdge();
    }
    DraftsPanel {
        id: draftsPanel
        anchors {
            top: header.bottom
            left: parent.left
            bottom: parent.bottom
        }
        height: composePage.height
        width: parent.width
        property bool composer: false
        onOpenedChanged: opened ? composePage.state = "draftsOpen" : composePage.state = "panelOpen"
        onOpenDraft: {
            composer.openDraft(docId)
            // TODO: find out issue with deleting documents not removing list view delegates
            // re-enable once fixed
            composer.docId = docId
            composer.isEditingDraft = true
        }
        z: 10
    }

    MessageComposer {
        id: composer
        draftsDatabase: draftsPanel.draftsData
        onSubmissionSuceeded: {
            if (isEditingDraft) {
                draftsDatabase.deleteDoc(docId)
            }
            closePanelTimer.start();
        }
    }

    Component {
        id: draftsDialog
        DraftsDialog {
            onSaveDraft: {
                composer.saveNewDraft()
                closePanelTimer.start()
            }
            onDiscardDraft: {
                closePanelTimer.start()
            }
            onUpdateDraft: {
                composer.updateDraft()
                closePanelTimer.start()
            }
        }
    }

    MessageModel {
        id: replyToMessageModel
        imapModel: dekko.currentAccount.imapModel
        Component.onCompleted: {
            if (mailbox && (uid != undefined)) {
                openMessage();
                if (!replyPartId) {
                    messageTextContentFetcher.msgPartIndex = replyToMessageModel.mainTextPartIndex;
                    composer.oneMessageModel = replyToMessageModel;
                } else {
                    replyToPartMessageModel.setMessageModelIndex(
                                replyToMessageModel.getModelIndexFromPartId(replyPartId))
                    messageTextContentFetcher.msgPartIndex = replyToPartMessageModel.mainTextPartIndex;
                    composer.oneMessageModel = replyToPartMessageModel;
                }
            }
        }
    }

    MessageModel {
        id: replyToPartMessageModel
        imapModel: dekko.currentAccount.imapModel
    }

    PlainTextFormatter {
        id: messageTextContentFetcher
        msgIndex: replyToMessageModel.messageIndex
        format: PlainTextFormatter.PLAIN
        onMessageChanged: {
            composer.message = rawMessage;
        }
    }

    FlattenContactModel{
        id: flattenContactModel
        model: addressbookModelFiltered
    }
    FlattenContactFilterModel{
        id: flattenContactFilterModel
        source: flattenContactModel
    }
}
