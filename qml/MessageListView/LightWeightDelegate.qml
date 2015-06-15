/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the Dekko email client for Ubuntu Touch/

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
import Ubuntu.Components.ListItems 1.0 as ListItems
import "../Utils/Utils.js" as Utils
import TrojitaCore 0.1
import DekkoCore 0.2
import "../Components"


ListItemWithActions {
    id: normalMessageItemDelegate

    //---------------------------------
    // PROPERTIES
    //---------------------------------
    property bool isFirst: false
    property bool active: false
    //---------------------------------
    // SIGNALS
    //---------------------------------
    signal markMessageDeleted()
    signal markMessageFlagged()

    //---------------------------------
    // FUNCTIONS
    //---------------------------------

    //---------------------------------
    // OBJECT PROPERTIES
    //---------------------------------
    showDivider: true
    height: units.gu(12)
    color: Theme.palette.normal.background
    triggerActionOnMouseRelease: true

    //---------------------------------
    // ACTIONS
    //---------------------------------

    function handleMarkMessageForDeletionByUserPrefs() {
        // eitherway we need to emit the markMessageDeleted signal so
        // lets do that first
        markMessageDeleted()
        // Now the user has the option to auto expunge on delete
        // at the moment this has a nasty side affect of resetting the listview
        // to the top, but should be fixed in Qt 5.3.1 so we will keep it here
        // for when that arrives
        if (dekko.currentAccount.preferences.AutoExpungeOnDelete) {
            dekko.currentAccount.imapModel.expungeMailbox(dekko.currentAccount.msgListModel.currentMailbox())
        } else {
            // so we only want to mark the message as deleted. In the future we
            // should probably provide a move to trash action. But we will stick with the
            // strikethrough for now
            return;
        }
    }

    //---------------------------------
    // COMPONENTS
    //---------------------------------

    Item {
        id: main
        anchors { fill: parent; margins: units.gu(0.2) }
        Column {
            id: infoDelegate
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(2)
            }
            visible: model.isFetched
            spacing: units.gu(0.3)
            Label {
                id: fromLabel
                text: model.from ? Utils.formatMailAddresses(model.from, true) : ""
                fontSize: "large"
                font.bold: !model.isMarkedRead
                font.strikeout: model.isMarkedDeleted
                font.weight: model.isMarkedRead ? Font.Light : Font.DemiBold
                clip: true
                width: parent.width - timeLabel.width - units.gu(5)
                elide: Text.ElideRight
            }

            Label {
                id: subjectLabel
                width: parent.width - timeLabel.width
                text: model.subject ? model.subject : qsTr('<No Subject>')
                fontSize: "small"
                clip: true
                color: model.isMarkedRead ? Theme.palette.selected.backgroundText : UbuntuColors.orange
                font.bold: !model.isMarkedRead
                font.strikeout: model.isMarkedDeleted
                elide: Text.ElideRight
            }

            Label {
                id: previewTextLabel
                width: parent.width
                wrapMode: Text.WordWrap
                fontSize: "small"
                maximumLineCount: 2
                visible: text != ""
                text: preview ? preview : ""
            }
        }
        Label {
            id: timeLabel
            anchors {
                top: infoDelegate.top
                right: infoDelegate.right
                rightMargin: units.gu(1)
            }
            font.bold: !model.isMarkedRead
            text: model ? Utils.prettyDate(model.date): "";
            font.pixelSize: FontUtils.sizeToPixels("small")
        }
        Column {
            x: -units.gu(1)
            y: units.gu(0.5)
            width: units.gu(2)
            spacing: units.gu(0.5)
            Item {
                height: units.gu(2)
                width: height
                visible: model.isFetched ? model.isMarkedFlagged : false
                Icon {
                    anchors.fill: parent
                    // TODO replace once this lands in suru
                    source:  Qt.resolvedUrl("qrc:///actions/mail-mark-important.svg")
                }
            }
            Item {
                height: units.gu(2)
                width: height
                visible: model.isFetched ? model.hasAttachments : false
                Icon {
                    anchors.fill: parent
                    name: "attachment"
                }
            }
            Item {
                height: units.gu(2)
                width: height
                visible: model.isFetched ? model.isMarkedReplied : false
                Icon {
                    anchors.fill: parent
                    source: Qt.resolvedUrl("qrc:///actions/mail-replied.svg")
                }
            }
        }

        ListItems.Standard {
            id: loadingIndicator
            height: parent.height
            text: model.isUnavailable ? qsTr("Unavailable") : qsTr("Loading...")
            //read model.subject to trigger fetch
            visible: !model.isFetched
            showDivider: false
        }
    }
}
