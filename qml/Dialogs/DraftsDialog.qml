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
import Ubuntu.Components.Popups 1.0

Dialog {
    id: draftsDialog

    property bool isEditingDraft: false
    signal saveDraft
    signal updateDraft
    signal discardDraft

    title: qsTr("Message not sent")
    text: qsTr("What would you like to do?")

    contents: [
        Button {
            id: saveButton
            text: isEditingDraft ? qsTr("Save changes to drafts") : qsTr("Save to drafts")
            onClicked: {
                if (isEditingDraft) {
                    updateDraft()
                } else {
                    saveDraft()
                }
                PopupUtils.close(draftsDialog)
            }
        },
        Button {
            id: discard
            text: isEditingDraft ? qsTr("Discard changes") : qsTr("Discard message")
            onClicked: {
                discardDraft()
                PopupUtils.close(draftsDialog)
            }
        },
        Button {
            id: con
            text: qsTr("Continue editing")
            onClicked: PopupUtils.close(draftsDialog)
        }

    ]
}
