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
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import DekkoCore 0.2
import "../AccountsView"

ListView {
    id: accountTypeListView
    // Signal to start the process of setting up an Online Account
    signal configureOnlineAccount()
    // Signal to start the process for setting up an account using the given provider
    signal configureForProvider(var serviceInfo)
    // Signal to start the process for creating a generic account
    signal configureGenericAccount()

    // Use our PreSetProvidersModel here which loads supported configurations
    // from configuration/serviceProvider.conf
    model: PreSetProvidersModel { id: providersModel }

    /* The cool thing here is we can guarantee the online accounts and Other Imap & SMTP will always be available
      and will not be affected by Pre-sets being changed or not available in the future. So placing them in the header
      and footer ensures they are not affected by the model
    */
//    header: AccountsDrawerDelegate {
//        text: qsTr("Online Accounts");
//        iconSource: "qrc:///provider/online-account.svg"
//        onClicked: PopupUtils.open(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"), dekko, {title: qsTr("Coming Soon...")})
//    }

    delegate: AccountsDrawerDelegate {
        text: description
        iconSource: model.icon
        onClicked: configureForProvider(serviceInfo)
    }

    footer: AccountsDrawerDelegate {
        text: qsTr("Other account");
        iconName: "settings"
        onClicked: configureGenericAccount()
    }

}
