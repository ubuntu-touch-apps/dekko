/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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
import "../Components"
ListItem.Base {
    height: addAttachmentButton.height + addedAttachmentsView.height
    ListView {
        id: addedAttachmentsView
        width: parent.width
        height: units.gu(6) * submissionManager.attachmentsModel.count
        model: submissionManager.attachmentsModel
        delegate:
            ListItem.Base {
                id: attachList
                height: units.gu(6)
                __contentsMargins: 0
                anchors.top: addedAttachmentsView.bottom
                removable: true
                confirmRemoval: true
                showDivider: false
                Row {
                    id: row
                    height: parent.height
                    spacing: units.gu(2)
                    ImageWithFallBack {
                        id: attachIcon
                        source: "image://theme/" + mimeTypeIconName + "-symbolic"
                        fallbackSource: "image://theme/empty-symbolic"
                        height: units.gu(3)
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Label {
                        text: caption
                        anchors.verticalCenter: parent.verticalCenter
                        elide: Text.ElideRight
                        width: Math.min(implicitWidth, attachList.width - attachIcon.width - attachSize.width - row.spacing * 2);
                    }
                    Label {
                        id: attachSize
                        text: " (" + humanReadableSize + ")"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                onItemRemoved: {
                    submissionManager.attachmentsModel.removeAttachmentAtIndex(index);
                }
            }
    }
    ListItem.Base {
        id: addAttachmentButton
        height: units.gu(6)
        __contentsMargins: 0
        anchors.top: addedAttachmentsView.bottom
        Row {
            height: parent.height
            spacing: units.gu(2)
            Icon {
                name: "attachment"
                height: units.gu(3)
                width: height
                anchors.verticalCenter: parent.verticalCenter
            }
            Label {
                text: qsTr("Add attachment")
                anchors.verticalCenter: parent.verticalCenter
            }

        }
        showDivider: false
        onTriggered: {
            var contentDialog = PopupUtils.open(Qt.resolvedUrl("../Dialogs/ContentPickerDialog.qml"), dekko, {
                                                    isExport: false
                                                });
            contentDialog.filesImported.connect(composePage.addAttachmentsFromUrls);
        }
    }
}
