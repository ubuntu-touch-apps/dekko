import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItems
import DekkoCore 0.2

ActionSelectionPopover {
    id: actionPopover
    property string mboxName
    property var uid
    delegate: ListItems.Standard {
        text: action.text
        onClicked: PopupUtils.close(actionPopover)
    }
    actions: ActionList {
        Action {
            id: reply
            text: qsTr("Reply")
            onTriggered: {
                pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                               {
                                   composerReplyMode: true,
                                   pageTitle: qsTr("Reply To Message"),
                                   replyMode: ReplyMode.REPLY_PRIVATE,
                                   replyUid: actionPopover.uid,
                                   replyMboxName: actionPopover.mboxName
                               });
            }
        }
        Action {
            id: replyAll
            text: qsTr("Reply all")
            onTriggered: {
                pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                               {
                                   composerReplyMode: true,
                                   pageTitle: qsTr("Reply To Message"),
                                   replyMode: ReplyMode.REPLY_ALL,
                                   replyUid: actionPopover.uid,
                                   replyMboxName: actionPopover.mboxName
                               });
            }
        }

        Action {
            id: forward
            text: qsTr("Forward")
            onTriggered: {
                pageStack.push(Qt.resolvedUrl("../Composer/ComposePage.qml"),
                               {
                                   composerReplyMode: true,
                                   pageTitle: qsTr("Forward"),
                                   replyMode: ReplyMode.REPLY_FORWARD,
                                   replyUid: actionPopover.uid,
                                   replyMboxName: actionPopover.mboxName
                               });
            }
        }
        Action {
            id: moveMessageAction
            text: qsTr("Move message to...")
            onTriggered: {
                // Rather annoyingly we can't call the *actual" action here as the caller get's destroyed
                // after this is called so the MoveActionDialog turns unresponsive as a result of this.
                root.moveMessage(actionPopover.uid)
            }
        }
    }

    InverseMouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: PopupUtils.close(actionPopover)
    }
}
