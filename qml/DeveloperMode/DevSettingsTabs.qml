/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import TrojitaCore 0.1
import DekkoCore 0.1
import "../Settings"
import "../Components"


Tabs {
    id: settingsTabs
    visible: false
    // during the password saving there is a short period where password is an empty string and trying to
    // either using the binding to determine if password exists or reloadPassword cause undesired events.
    // This helps to determine if we need to show error dialog on saving failed signals
    property bool passwordGiven

    property string accountName
    property bool newAccount
    property alias imapAddress: serverSettings.imapUserName
    property var settingsActions: [cancelAction, saveAction]

    Component.onCompleted: {
        imapPasswordWatcher.savingFailed.connect(showSavingPasswordFailed)
        smtpPasswordWatcher.savingFailed.connect(showSavingPasswordFailed)
        // ensure we have the smtpPassword or at least started the FETCH job
        if (accountName) {
            imapPasswordWatcher.reloadPassword(accountName, "imap")
            //            smtpPasswordWatcher.reloadPassword(accountName, "smtp")
        }
        if (newAccount) {
            currentAccount.reset()
        }
    }

    function showSavingPasswordFailed(message) {
        // if no passwordGiven then we can safely just continue and ignore
        // the error message
        if (!passwordGiven) {
            continueSavingAction.trigger()
        } else {
            PopupUtils.open(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"), dekko, {
                                title: qsTr("Failed saving password"),
                                text: message,
                                buttonText: qsTr("Continue"),
                                buttonAction: continueSavingAction
                            })
        }
    }

    Action {
        id: continueSavingAction
        onTriggered: {
            serverSettings.save()
            mailPreferences.save()
            console.log("[Settings] Writing all settings to file")
            currentAccount.save()
            console.log("[Settings] Saving to file completed")
            console.log("[Settings] Reloading account listmodel")
            accountsListModel.reloadAccounts()
            console.log("[Settings] Saving account passwords")
            serverSettings.savePasswords()
            // update state
            serverSettings.imapSettingsState = "HAS_ACCOUNT"
            serverSettings.imapSettingsModified = false
            pageStack.clear()
            console.log("[Settings] Saving complete.........")
            if(accountName || newAccount) {
                dekko.showAccounts()
            } else {
                dekko.openCurrentAccount()
            }
        }
    }

    Action {
        id: cancelAction
        iconName: "close"
        text: qsTr("Close")
        onTriggered: {
            if (!newAccount) {
                console.log("[Settings] reloading settings due to user cancelation")
                currentAccount.reloadSettings()
                console.log("[Settings] saving just to be sure")
                currentAccount.save()
            }
            console.log("[Settings] Reloading accounts list model")
            accountsListModel.reloadAccounts()
            pageStack.clear()
            console.log("[Settings] Going back to accounts page now")
            if(accountName || newAccount) {
                dekko.showAccounts()
            } else {
                pageStack.push(mailboxList)
            }
        }
    }

    Action {
        id: saveAction
        iconName: "ok"
        text: qsTr("Save")
        onTriggered: {
            console.log("[Settings] Saving account settings......")
            console.log("[Settings] Validating all settings")
            if (!serverSettings.imapSettingsValid) {
                console.log("[Settings] IMAP settings not valid. Opening IMAP settings page......")
                settingsTabs.selectedTabIndex = 1
                serverSettings.head.sections.selectedIndex = 0
            } else if (!serverSettings.smtpSettingsValid) {
                console.log("[Settings] SMTP settings not valid. Opening SMTP settings page......")
                settingsTabs.selectedTabIndex = 1
                serverSettings.head.sections.selectedIndex = 1
            } else if (currentAccount.identityModel.count === 0) {
                console.log("[Settings] Identity settings not valid. Opening Identity settings page......")
                settingsTabs.selectedTabIndex = 3
            } else {
                console.log("[Settings] Settings valid..... Continuing save process")
                passwordGiven = serverSettings.imapPasswordGiven
                senderIdentities.saveIdentity()
                continueSavingAction.trigger()
            }
        }
    }

    Tab {
        id: devSettingsTab
        title: qsTr("Developer Settings")
        page: DeveloperSettings {
            id: devSettings
            visible: true
            head.actions: settingsActions
        }
    }


    Tab {
        id: serverView
        title: qsTr("Server Settings")
        page: ServerView {
            id: serverSettings
            visible: true
            account: currentAccount
            head.actions: settingsActions
        }
    }

    Tab {
        id: mailPreferencesTab
        title: qsTr("Mail Preferences")
        page: MailPreferences {
            id: mailPreferences
            visible: true
            account: currentAccount
            head.actions: settingsActions
        }
    }
    Tab {
        id: senderIdentitiesTab
        title: qsTr("Sender Identity")
        page: SenderIdentities {
            id: senderIdentities
            visible: true
            account: currentAccount
            head.actions: settingsActions
        }
    }

    Tab {
        id: imapCapabilites
        title: qsTr("Capabilities")
        page: ImapCapabilitiesView {
            id: imapCapabilitesView
            visible: true
            head.actions: settingsActions
        }
    }
}





