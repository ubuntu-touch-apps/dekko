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
import DekkoCore 0.2
import "../../AccountsView"
import "../../Components"
import "../common"

VisualItemModel {
    id: accountSettingsModel
    property ImapSettings imapSettings: root.account.imapSettings
    property SmtpSettings smtpSettings: root.account.smtpSettings
    signal openImapSettings()
    signal openSmtpSettings()
    signal openAccountDescription()
    signal openIdentity(string id)
    signal addNewIdentity()
    signal openMailboxBehaviour()

    SettingsListItem {
        text: qsTr("Incoming mail server")
        subText: imapSettings.server
        onClicked: openImapSettings()
    }
    SettingsListItem {
        text: qsTr("Outgoing mail server")
        subText: smtpSettings.server
        onClicked: openSmtpSettings()
    }
    SettingsListItem {
        text: qsTr("Account description")
        subText: root.account.profile.description
        onClicked: openAccountDescription()
    }

    SectionDivider {
        text: qsTr("Sender identities")
    }
    Column {
//        height: identities.height
        width: parent.width
        Repeater {
            id: identities
            model: root.senderIdentityModel
            delegate: ListItem.Standard {
                text: model.formattedString
                removable: true
                confirmRemoval: true
                onItemRemoved: root.senderIdentityModel.removeRow(model.index)
                onClicked: pageStack.push(Qt.resolvedUrl("NewIdentityPage.qml"),
                                          {
                                              model: root.senderIdentityModel,
                                              edit: true,
                                              identity: root.senderIdentityModel.getIdentity(model.index)
                                          })
            }
        }
    }
    SettingsListItem{
        text: qsTr("Add new identity")
        onClicked: pageStack.push(Qt.resolvedUrl("NewIdentityPage.qml"), {model: root.senderIdentityModel})
    }

    SectionDivider {
        text: qsTr("Mailbox settings")
    }
    SettingsListItem {
        text: qsTr("Mailbox behaviour")
        onClicked: openMailboxBehaviour()
    }
    ListItem.Standard {
        text: qsTr("Enable thunderbird tags")
        control: DekkoSwitch {
//            onClicked: GlobalSettings.offline.startOffline = checked
//            Component.onCompleted: checked = GlobalSettings.offline.startOffline
        }
    }

}
