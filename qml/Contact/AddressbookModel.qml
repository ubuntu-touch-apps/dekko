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
import DekkoCore 0.2

DekkoContactsModel {
    id: root

    function containsEmail(address) {
        if (address.indexOf(">") == address.length - 1) {
            address = address.split("<")[1].replace(">","").trim()
        }
        var tmp = findEmailContact(address);
        return tmp !== false;
    }

    function saveEmail(name, address) {
        newContact(name, "", "", [address]);
    }

    function savePrettyEmail(prettyAddress) {
        if (prettyAddress.indexOf(">") == prettyAddress.length - 1) {
            var components = prettyAddress.split("<");
            var emailAddress = components[1].split(">")[0];
            var name = components[0].trim();
            saveEmail(name, emailAddress);
        } else {
            saveEmail("", prettyAddress);
        }
    }

    function findEmailContact(address) {
        var i;
        for (i = 0; i < root.length; i++) {
            if (root.getByIndex(i).emails[0] == address) {
                return true;
            }
        }
        return false;
    }
}
