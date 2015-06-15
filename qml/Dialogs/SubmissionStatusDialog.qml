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
    id: statusDialog

    property string statusMessage: submissionManager.statusMessage
    property int progressMin: submissionManager.progressMin
    property int progressMax: submissionManager.progressMax
    property int progressActual: submissionManager.progress
    property bool succeeded: false

    signal closeClicked

    state: "InProgress"

    states: [
        State {
            name: "InProgress"
            PropertyChanges { target: statusDialog; title: qsTr("Sending...") }
        },
        State {
            name: "Success"
            PropertyChanges { target: statusDialog; title: qsTr("Success ") }
            PropertyChanges { target: close; visible: false }
            PropertyChanges { target: statusDialog; succeeded: true }
        },
        State {
            name: "Failed"
            PropertyChanges { target: statusDialog; title: qsTr("Sending Failed") }
            PropertyChanges { target: pbar; height: 0; visible: false}
            PropertyChanges { target: close; visible: true }
        }

    ]

    title: qsTr("Sending....")
    text: statusMessage

    contents: [
        ProgressBar {
            id: pbar
            indeterminate: submissionManager.progressIndeterminate
            minimumValue: progressMin
            maximumValue: progressMax
            value: progressActual
        },
        Button {
            id: close
            visible: false
            text: qsTr("Close")
            onClicked: closeClicked()
        }
    ]

    Component.onCompleted: {
        submissionManager.failed.connect(function(msg) {statusMessage = msg; state = "Failed"});
        submissionManager.succeeded.connect(function () {
            state = "Success";
            closeTimer.start();
        });
    }

    Timer {
        id: closeTimer
        interval: 1000
        repeat: false
        onTriggered: closeClicked();
    }
}
