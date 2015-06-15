/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the dekko

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

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
import TrojitaCore 0.1
import DekkoCore 0.2
import "../Components"
import "../Dialogs"
import "../Utils/Utils.js" as Utils
import "../UCSComponents"
import "../Composer"

Page {
    id: oneMessagePage

    property alias uid: messageModel.uid
    property alias mailboxName: messageModel.mailbox
    readonly property alias messageIndex: messageModel.messageIndex
    property bool hasAttachments: false
    property bool displayingHTML: false

    //Composer properties
    property string fromName: qsTr("Reply to %1").arg(Utils.formatPlainArrayMailAddresses(messageModel.from, true, false))
    property string replyMessage: ""// bind in PlainTextPart
    property var messageListModel
    property bool editable: true
    property string partId // store the part of the embedded message to display

    flickable: null

    DekkoHeader {
        id: header
        title: qsTr("Message")
        backAction: Action {
            id: backAction
            enabled: !htmlView.isScrolling
            iconName: "back"
            onTriggered: pageStack.pop()
        }
        primaryAction: editable ? deleteAction : null
        secondaryActions: editable ? [prevMsgAction, nextMsgAction] : null
    }

    function load() {
        if (editable)
            startMarkAsReadTimerPerSetting();
        messageModel.openMessage();
        msgPartNetAccessManagerFactory.setMessageIndex(messageModel.messageIndex)
        msgPartNAM.setModelMessage(messageModel.messageIndex)
        if (partId) {
            messageBuilder.msgIndex = messageModel.getModelIndexFromPartId(partId)
        } else {
            messageBuilder.msgIndex = messageModel.messageIndex
        }

        messageBuilder.buildMessage()
    }

    function startMarkAsReadTimerPerSetting() {
        switch (GlobalSettings.preferences.markAsRead) {
        case Preferences.NEVER:
            break;
        case Preferences.AFTER_X_SECS:
            markAsReadTimer.interval = GlobalSettings.preferences.markAsReadAfter * 1000
            markAsReadTimer.start()
            break;
        case Preferences.IMMEDIATELY:
            markAsReadTimer.start()
            break;
        }
    }

    MessageModel {
        id: messageModel
        imapModel: dekko.currentAccount.imapModel
        Component.onCompleted: load();
    }

    MessageBuilder {
        id: messageBuilder
        // This property protects us from the network connection being reset
        // We want to only allow the message to be loaded once, unless goNext/Prev is called
        // Which will reset this to allow the next message to load.
        property bool allowLoading: true
        messageModel: messageModel
        preferPlainText: GlobalSettings.preferences.preferPlainText
        onMessageChanged: {
            if (allowLoading) {
                htmlView.loadHtml(message, Qt.resolvedUrl("."))
                // We've loaded so block unless the uid changes via goNext/Prev
                allowLoading = false;
            }
        }
    }
    DekkoWebView {
        id: htmlView
        property bool isScrolling
        onContentYChanged: {
            isScrolling = true
            webviewTimer.start()
        }
        onContentXChanged: {
            isScrolling = true
            webviewTimer.start()
        }
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        onOpenAttachment: attachmentDownloader.fetchPart(partId);
        onOpenEmbeddedMessage: {
            pageStack.push(Qt.resolvedUrl("../MessageView/OneMessagePage.qml"),
               {
                   mailboxName: oneMessagePage.mailboxName,
                   uid: oneMessagePage.uid,
                   partId: partId,
                   editable: false
               });
        }
        onComposeMessageTo: {
            pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
               {
                   replyMode: ReplyMode.REPLY_NONE,
                   pageTitle:qsTr("New Message"),
                   mailtoUrl: "mailto:" + address,
                   isBottomEdgeMode: false

               });
        }
        onAddToAddressbook: {
            addressbookModel.savePrettyEmail(address)
        }
    }

    Timer {
        id: webviewTimer
        repeat: false
        interval: 1000
        onTriggered: htmlView.isScrolling = false
    }

    Timer {
        id: markAsReadTimer
        repeat: false
        interval: 50
        onTriggered: {
            messageModel.markMessageAsRead();
            if (dekko.currentAccount.isGmail) {
                console.log("Starting gmail hack to double mark \Seen")
                gmailHack.start()
            }
        }
    }

    Timer {
        id: gmailHack
        repeat: false
        interval: 500
        onTriggered: messageModel.markMessageAsRead();
    }

    Action {
        id: deleteAction
        iconName: "delete"
        text: qsTr("Delete")
        enabled: !htmlView.isScrolling && editable
        visible: dekko.currentAccount.imapModel.isNetworkOnline
        onTriggered: {
            messageModel.markMessageDeleted(messageModel.messageIndex, true)
            if (GlobalSettings.preferences.autoExpungeOnDelete) {
                dekko.currentAccount.imapModel.expungeMailbox(dekko.currentAccount.msgListModel.currentMailbox())
            }
            backAction.trigger()
        }

    }

    Action {
        id: nextMsgAction
        iconName: "down"
        text: qsTr("Next message")
        enabled: uidIterator.nextUid > 0 && editable
        onTriggered: {
            messageBuilder.allowLoading = true
            uid = uidIterator.goNext();
            if (uid > 0) {
                load();
            }
        }
    }

    Action {
        id: prevMsgAction
        iconName: "up"
        text: qsTr("Previous message")
        enabled: uidIterator.prevUid > 0 && editable
        onTriggered: {
            messageBuilder.allowLoading = true
            uid = uidIterator.goPrev();
            if (uid > 0) {
                load();
            }
        }
    }

    RadialAction {
        id: replyAll
        iconSource: "qrc:///actions/mail-reply-all.svg"
        iconColor: UbuntuColors.coolGrey
        text: qsTr("Reply all")
        onTriggered: {
            pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                           {
                               composerReplyMode: true,
                               pageTitle: fromName.length > 25 ? (fromName.slice(0,25) + "...") : fromName,
                               replyMode: ReplyMode.REPLY_ALL,
                               replyUid: oneMessagePage.uid,
                               replyMboxName: oneMessagePage.mailboxName,
                               replyPartId: partId,
                               isBottomEdgeMode: false
                           });
        }
    }
    RadialAction {
        id: reply
        iconSource: "qrc:///actions/mail-reply.svg"
        iconColor: UbuntuColors.coolGrey
        top: true
        text: qsTr("Reply")
        onTriggered: {
            pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                           {
                               composerReplyMode: true,
                               pageTitle: fromName.length > 25 ? (fromName.slice(0,25) + "...") : fromName,
                               replyMode: ReplyMode.REPLY_PRIVATE,
                               replyUid: oneMessagePage.uid,
                               isBottomEdgeMode: false,
                               replyMboxName: oneMessagePage.mailboxName,
                               replyPartId: partId
                           });
        }
    }

    RadialAction {
        id: replyList
        iconSource: "qrc:///actions/mail-reply-all.svg"
        iconColor: UbuntuColors.coolGrey
        text: qsTr("Reply List")
        onTriggered: {
            pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                           {
                               composerReplyMode: true,
                               pageTitle: fromName.length > 25 ? (fromName.slice(0,25) + "...") : fromName,
                               replyMode: ReplyMode.REPLY_LIST,
                               replyUid: oneMessagePage.uid,
                               replyMboxName: oneMessagePage.mailboxName,
                               replyPartId: partId,
                               isBottomEdgeMode: false
                           });
        }
    }

    RadialAction {
        id: forward
        iconSource: "qrc:///actions/mail-forward.svg"
        iconColor: UbuntuColors.coolGrey
        text: qsTr("Forward")
        onTriggered: {
            pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                           {
                               composerReplyMode: true,
                               pageTitle: qsTr("Forward"),
                               replyMode: ReplyMode.REPLY_FORWARD,
                               replyUid: oneMessagePage.uid,
                               replyMboxName: oneMessagePage.mailboxName,
                               replyPartId: partId,
                               isBottomEdgeMode: false
                           });
        }
    }

    RadialBottomEdge {
        visible: dekko.currentAccount.imapModel.isNetworkOnline
        actions: messageModel.isListPost ? [reply, replyAll, replyList, forward] : [reply, replyAll, forward]
    }


    Component {
        id: attachmentDialog
        Dialog {
            id: attachmentDownloadProgressDialog

            property alias progress: attachmentDownloadProgressbar.value

            title: qsTr("Downloading")
            contents: [
                ProgressBar {
                    id: attachmentDownloadProgressbar
                    maximumValue: 1
                    minimumValue: 0
                }
            ]
        }
    }

    AttachmentDownloader {
        id: attachmentDownloader
        messageModel: messageModel
        msgPartNAM: msgPartNAM

        property var progressDialog;
        property string fileUrl;

        onDownloadProgress: {
            if (!progressDialog) {
                progressDialog = PopupUtils.open(attachmentDialog);
            }
            progressDialog.progress = downloaded / total;
        }
        onDownloadComplete: {
            if (progressDialog) {
                progressDialog.progress = 1;
                openContentPickerTimer.start();
                fileUrl = url;
            } else {
                openContentPicker(url)
            }
        }
        onDownloadFailed: {
            console.log("Download Failed");
            if (progressDialog) {
                PopupUtils.close(progressDialog);
                progressDialog = false;
            }
        }

        function openContentPicker(url) {
            if (!url) {
                url = fileUrl;
            }
            var contentDialog = PopupUtils.open(Qt.resolvedUrl("../Dialogs/ContentPickerDialog.qml"));
            contentDialog.downloadFilePath = url;
            contentDialog.complete.connect(function() {
                attachmentDownloader.cleanTmpFile();
            });
        }
    }

    Timer {
        id: openContentPickerTimer
        interval: 1000
        repeat: false
        onTriggered: {
            PopupUtils.close(attachmentDownloader.progressDialog);
            attachmentDownloader.progressDialog = false;
            attachmentDownloader.openContentPicker();
        }
    }

    MsgPartNetAccessManager {
        id: msgPartNAM
    }

    UidIterator {
        id: uidIterator
        messageListModel: oneMessagePage.messageListModel ? oneMessagePage.messageListModel : null
        uid: oneMessagePage.uid
    }
}
