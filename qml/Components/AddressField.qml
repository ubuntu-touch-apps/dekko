/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>
   Copyright (C) 2014 Giulio Collura <random.cpp@gmail.com>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

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
import Ubuntu.Components.Popups 1.0

import "../Utils/Utils.js" as Utils

TextArea {
    id: field
    maximumLineCount: 1
    autoSize: false
    height: units.gu(4)
    // This special field can manage several addresses
    property var addresses: new Array()
    signal createAddress(string address)

    inputMethodHints: Qt.ImhEmailCharactersOnly

    Keys.onReturnPressed: formatAddresses()
    onFocusChanged: !focus ? formatAddresses() : undefined

    function formatAddresses() {
        if (text) {
            var email = text.trim()
            if (validateAddress(email)) {
                addresses.push(email);
                createAddress(email);
                field.text = ""
                field.placeholderText = qsTr("Add another recipient");
            } else {
                PopupUtils.open(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"),
                                dekko,
                                {
                                    title: qsTr("Ooops!"),
                                    text: qsTr("'%1' doesn't seem to be a valid email address").arg(email)
                                }
                                );
                focus = true;
            }
        }
    }

    function validateAddress(email) {
        var re = /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
        return re.test(email);
    }
}
