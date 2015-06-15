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
import Ubuntu.Components.ListItems 1.0 as ListItem

Dialog {
    id: loadingOverlay

    property bool loadingFinished: !imapAccess.visibleTasksModel.hasVisibleTasks
    onLoadingFinishedChanged: loadingFinished ? PopupUtils.close(loadingOverlay) : undefined

    Connections {
        target: dekko
        onShowAccounts: PopupUtils.close(loadingOverlay)
    }

    Column {
        id: containerLayout
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        ListView {
            id: view
            height: units.gu(6)
            model: imapAccess.visibleTasksModel ? imapAccess.visibleTasksModel : null
            interactive: false
            anchors {
                right: parent.right
                left: parent.left
            }
            delegate: ListItem.Standard {
                text: compactName
                highlightWhenPressed: false
                showDivider: false
                visible: index < 1
                __mouseArea.visible: false
                control: ActivityIndicator {
                    height: units.gu(4)
                    width: height
                    visible: imapAccess.visibleTasksModel.hasVisibleTasks
                    running: visible
                }
            }
        }
    }

}
