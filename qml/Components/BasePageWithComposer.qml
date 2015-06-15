/* Copyright (C) 2014 Dan Chapman <dpniel@ubuntu.com>

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
import TrojitaCore 0.1
import DekkoCore 0.2

Page {
    id: pageWithEdge
//    clip: true
    property alias tipText: bottomEdge.tipText
    property alias bottomEdgeOpened: bottomEdge.opened
    property alias bottomContents: bottomEdge.contents
    property alias composerReplyMode: composer.replyingToMessage
    property alias replySubject: composer.subject
    property alias replyMessage: composer.message
    property alias replyMode: composer.replyMode
    property alias replyUid: composer.replyUid
    property alias replyMboxName: composer.replyMboxName
    property alias identityModel: composer.identityModel
    property string pageTitle: ""
    property Item pageTools
    property var pageHead
    property var initialReplyMode

    head.contents: Label {
        width: parent ? parent.width : undefined
        anchors.verticalCenter: parent ? parent.verticalCenter : undefined
        text: pageTitle
        fontSize: "x-large"
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
        maximumLineCount: fontSize === "large" ? 2 : 1
        onTruncatedChanged: {
            if (truncated) {
                fontSize = "large"
            }
        }
    }

    function closeBottomEdge() {
        if (bottomEdge.opened) {
            if (initialReplyMode !== undefined) {
                replyMode = initialReplyMode
            }

            bottomEdge.close()
            //clear attachments
            if (submissionManager.attachmentsModel.count > 0) {
                submissionManager.attachmentsModel.clear();
            }
        }
    }

    function openDrafts() {
        bottomEdge.open()
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
        bottomEdge.open()
    }

    state: "panelHidden"

    states: [
        State {
            name: "panelHidden"
            PropertyChanges {
                target: pageWithEdge.head
                actions: pageHead.actions
                backAction: pageHead.backAction
            }
            PropertyChanges {
                target: pageWithEdge.head.sections
                enabled: pageHead.sections.enabled
                model: pageHead.sections.model
            }

            PropertyChanges {
                target: pageWithEdge
                title: pageTitle
            }
        },
        State {
            name: "panelOpen"
            PropertyChanges {
                target: pageWithEdge.head
                actions: composerHeader.actions
                backAction: composerHeader.backAction
            }
            PropertyChanges {
                target: pageWithEdge
                title: tipText
            }
        },
        State {
            name: "draftsOpen"
            PropertyChanges {
                target: pageWithEdge.head
                actions: draftsHeader.actions
                backAction: draftsHeader.backAction
            }
            PropertyChanges {
                target: pageWithEdge
                title: qsTr("Drafts")
            }
        },
        State {
            name: "editingDraft"
            PropertyChanges {
                target: pageWithEdge.head
                tools: composerHeader.actions
            }
            PropertyChanges {
                target: pageWithEdge
                title: qsTr("Edit draft")
            }
        }
    ]

    Timer {
        id: closePanelTimer
        interval: 500
        onTriggered: pageWithEdge.closeBottomEdge()
    }

    Rectangle {
        id: panelOpenBackground
        visible: bottomEdge.pressed || bottomEdge.animating
        color: "black"
        z: 1
        anchors.fill: pageWithEdge
        opacity: 0.3 * ((pageWithEdge.height - bottomEdge.position) / pageWithEdge.height)
    }

    ComposePageHeadConfiguration {
        id: composerHeader
        onSendClicked: {
            composer.sendEmail()
        }
        onCancelClicked: {
            Qt.inputMethod.hide()
            //TODO: FIXME: only saving drafts for new messages atm
            if (composer.messageStarted) {
                PopupUtils.open(draftsDialog, dekko, {isEditingDraft: composer.docId})
            } else {
                closePanelTimer.start()
            }
        }
        onDraftsClicked: {
            draftsPanel.composer = true
            draftsPanel.open()
            pageWithEdge.state = "draftsOpen"
        }
    }

    ComposePanel {
        id: bottomEdge
        __closeOnContentsClicks: false
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: pageWithEdge.height
        onOpenedChanged: {
            if (opened) {
                pageWithEdge.state = "panelOpen"
            } else {
                pageWithEdge.state = "panelHidden"
                composer.resetComposer()
                draftsPanel.composer = false
            }
        }

        contents: MessageComposer {
            id: composer
            draftsDatabase: draftsPanel.draftsData
            onSubmissionSuceeded: {
                if (isEditingDraft) {
                    draftsDatabase.deleteDoc(docId)
                }
                pageWithEdge.closeBottomEdge()
            }
        }

        DraftsPanel {
            id: draftsPanel
            property bool composer: false
            onOpenedChanged: opened ? pageWithEdge.state = "draftsOpen" : pageWithEdge.state = "panelOpen"
            onOpenDraft: {
                composer.openDraft(docId)
                // TODO: find out issue with deleting documents not removing list view delegates
                // re-enable once fixed
                composer.docId = docId
                composer.isEditingDraft = true
            }
        }
    }
    Component {
        id: draftsDialog
        DraftsDialog {
            onSaveDraft: {
                composer.saveNewDraft()
                pageWithEdge.closeBottomEdge()
            }
            onDiscardDraft: {
                pageWithEdge.closeBottomEdge()
            }
            onUpdateDraft: {
                composer.updateDraft()
                pageWithEdge.closeBottomEdge()
            }
        }
    }

    PageHeadConfiguration {
        id: draftsHeader
        backAction: Action {
            iconName: "back"
            onTriggered: draftsPanel.composer ? draftsPanel.close() : pageWithEdge.closeBottomEdge()
        }
    }
}
