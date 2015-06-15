/* Copyright (C) 2014 Dan Chapman <dpniel@ubuntu.com>

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
import Ubuntu.Components.Popups 1.0

Dialog {
    id: activityDialog

    Component.onCompleted: {
        appWindow.authenticated.connect(authenticationComplete)
    }

    function authenticationComplete() {
        state = "Finished"
    }

    state: "Running"

    states: [
        State {
            name: "Running"
            PropertyChanges {
                target: activityIndicator
                running: true
            }
        },
        State {
            name: "Finished"
            PropertyChanges {
                target: activityIndicator
                running: false
            }
        }
    ]

    onStateChanged: {
        if (state === "Finished") {
            PopupUtils.close(activityDialog)
        }
    }

    contents: ActivityIndicator {
        id: activityIndicator
    }
}
