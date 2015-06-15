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
import DekkoCore 0.2
import "../UCSComponents"
import "../Components"

Page {
    id: accountSetup

    flickable: null

    DekkoHeader {
        id: header
        title: isPreset ? accountPreset.description : qsTr("New Account")
        backAction: Action {
            iconName: "back"
            onTriggered: {
                mboxSettings.removeAccount()
                pageStack.pop()
            }
        }
        primaryAction: Action {
            id: saveAction
            iconName: "ok"
            onTriggered: saveAccount()
        }
    }

    // The account preset QVariant we get from the PreSetProvidersModel
    property variant accountPreset
    readonly property bool isPreset: state === "pre-set-account"

    function saveAccount() {
        if (!profile.isValid) {
            setupWalkThrough.showInvalidFields()
            return;
        }
        if (state === "generic-account") {
            if (internal.waitingForAutoConfig) {
                // Lets just return here and let
                // AutoConfig recall save for us when it's done
                return;
            } else if (autoConfig.inProgress) {
                // We need to wait for autoconfig to finish
                internal.waitingForAutoConfig = true;
                return;
            }
            // Auto config has finished if we reached here
            if (autoConfig.status === AutoConfig.REQUEST_FAILED) {
                if (setupWalkThrough.state === "basic") {
                    setupWalkThrough.basicSetupFailed()
                    return;
                }
            }
        }

        if (!imapSettings.isValid || !smtpSettings.isValid) {
            console.log("Something wrong with imap/smtp settings")
            console.log("For basicSetup / pre-set providers we can't really do much as this is a bug on our part. so showing any warnings is useless")
            console.log("Prompting user to check the settings we have and modify as needed. Whatever happens we DO NOT let them get stuck on the basic setup view!!")
            if (setupWalkThrough.state === "basic") {
                setupWalkThrough.basicSetupFailed()
            }
            setupWalkThrough.showInvalidFields()
            return;
        }

        if (imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_UNKNOWN) {
            verifyImapSettingsIfComplete();
            internal.waitForImapSettingVerification = true;
            return;
        } else if (imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_RUNNING) {
            internal.waitForImapSettingVerification = true;
            return;
        } else if (imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_FAILED) {
            verifyImapSettingsIfComplete();
            internal.waitForImapSettingVerification = true;
            return;
        }

        searchForSpecialMailbox.complete.connect(function() {
            // Seems we have everything :-D Let's save it then!
            internal.finishSaving();
        })
        searchForSpecialMailbox.start();

    }

    function verifyImapSettingsIfComplete() {
        if (imapSettings.isValid && imapSettingVerifier.username !== "" && imapSettingVerifier.password !== "") {
            imapSettingVerifier.start();
        }
    }

    QtObject {
        id: internal

        property bool waitingForAutoConfig: false
        property bool waitForImapSettingVerification: false

        function determineConnectionMethods(conType) {
            if (conType === "imap") {
                if (accountPreset.imapSSL) {
                    return ImapSettings.SSL
                } else if (accountPreset.imapStartTls) {
                    return ImapSettings.STARTTLS
                } else {
                    return ImapSettings.NONE
                }
            } else if (conType === "smtp") {
                if (accountPreset.smtpSSL) {
                    return SmtpSettings.SSMTP;
                } else if (accountPreset.smtpStartTls) {
                    return SmtpSettings.SMTP_STARTTLS;
                } else {
                    return SmtpSettings.SMTP;
                }
            } else {
                console.log("Cannot determine connection for unknown type: " + conType);
            }
        }

        function finishSaving() {
            account.profile = profile
            account.imapSettings = imapSettings
            account.smtpSettings = smtpSettings
            imapPasswordManager.setPassword(setupWalkThrough.imapPassword)
            smtpPasswordManager.setPassword(setupWalkThrough.smtpPassword)
            // At this point the identity email field may actually just be
            // a username as we are stealing the value from imapSettings.username
            // If that is the case then we do not create a default identity and default
            // to just using the email address when composing. Unless the user sets an identity up, that is..
            // FIXME: we could try and prompt for an email address??
            var re = /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
            if (re.test(newIdentity.email)) {
                identityModel.appendRow(newIdentity)
                identityModel.saveIdentites()
            }

            // TODO: Move on from here to the "Finalizing setup view" we should be displaying that before appending the new account
            account.save()
            // TODO: move this to the accountsmanager to check if it's open or should open it
            // this way it get's done in the appendRow method and makes more sense as to why it's called
            account.openImapConnection()
            // TODO: double check account isValid
            dekko.accountsManager.appendRow(account)
            pageStack.clear()
            pageStack.push(Qt.resolvedUrl("../MessageListView/MessageListPage.qml"))
        }
    }

    // default to generic
    state: "generic-account"

    states: [
        State {
            name: "online-account"
        },
        State {
            name: "pre-set-account"
        },
        State {
            name: "generic-account"
        }
    ]

    Account {
        id: account
        // This will create a new accountId
        newAccount: true
    }

    AccountProfile {
        id: profile
        // - re-use the accountId
        property bool isValid: name
        readonly property string desc: setupWalkThrough.description ? setupWalkThrough.description : newIdentity.email
        accountId: account.accountId
        name: accountId ? setupWalkThrough.name : ""
        description: accountId ? profile.desc: "" //Account nickname
        organization: accountId ? setupWalkThrough.company : "" // Optional "company" field
    }

    ImapSettings {
        id: imapSettings
        property bool isValid: username && server && port
        accountId: account.accountId
        username: accountId ? setupWalkThrough.imapUsername : ""
        server: isPreset ? accountPreset.imapServer : setupWalkThrough.imapServer
        port: isPreset ? accountPreset.imapPort : setupWalkThrough.imapPort
        connectionMethod: isPreset ? internal.determineConnectionMethods("imap") : setupWalkThrough.imapEncryptionMethod
        //NOTE: The double LOGIN will change when we get to online accounts, which we will decide LOGIN or OAUTH
        authenticationMethod: isPreset ? ImapSettings.LOGIN : ImapSettings.LOGIN
        onAccountIdChanged: console.log(accountId)
    }

    SmtpSettings {
        id: smtpSettings
        property bool isValid: username && server && port
        accountId: account.accountId
        username: accountId ? setupWalkThrough.smtpUsername : ""
        server: isPreset ? accountPreset.smtpServer : setupWalkThrough.smtpServer
        port: isPreset ? accountPreset.smtpPort : setupWalkThrough.smtpPort
        submissionMethod: isPreset ? internal.determineConnectionMethods("smtp") : setupWalkThrough.smtpEncryptionMethod
        authenticationMethod: isPreset ? SmtpSettings.LOGIN : SmtpSettings.LOGIN
    }

    // For now let's just create seperate ones
    // I didn't quite think about setting both imap&smtp at the same time here when i implemented it  :-/ .....
    // anyway this should work just fine as QSettings is re-entrant neither will block each other
    PasswordManager {
        id: imapPasswordManager
        accountId: account.accountId
        type: PasswordManager.IMAP
    }

    PasswordManager {
        id: smtpPasswordManager
        accountId: account.accountId
        type: PasswordManager.SMTP
    }

    SenderIdentityModel {
        id: identityModel
        accountId: account.accountId
    }

    SenderIdentity {
        id: newIdentity
        name: profile.name
        email: setupWalkThrough.email
        organization: profile.organization
        useDefaultSignature: true
    }

    AutoConfig {
        id: autoConfig
        // TODO: hook up to non invasive progress bar
        onInProgressChanged: console.log("In Progress: " + inProgress)
        emailAddress: setupWalkThrough.imapUsername
        onSuccess: {
            console.log("Success, autoconfig found useful settings")
            imapSettings.server = config.imapHost
            imapSettings.port = config.imapPort
            imapSettings.connectionMethod = config.imapMethod
            smtpSettings.server = config.smtpHost
            smtpSettings.port = config.smtpPort
            smtpSettings.submissionMethod = config.smtpMethod
            // If the save process is waiting for autoconfig let's recall it
            if (internal.waitingForAutoConfig) {
                internal.waitingForAutoConfig = false
                saveAccount()
            } else {
                verifyImapSettingsIfComplete();
            }
        }
        onFailed: {
            console.log("FAILED Autoconfig")
            if (internal.waitingForAutoConfig) {
                internal.waitingForAutoConfig = false
                // This will open the manualSetup view
                saveAccount()
            }
        }
    }

    ImapSettingVerifier {
        id: imapSettingVerifier
        username: setupWalkThrough.imapUsername
        password: setupWalkThrough.imapPassword
        _imapSettings: imapSettings
        onResultChanged: {
            if (internal.waitForImapSettingVerification) {
                internal.waitForImapSettingVerification = false;
                if (result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_SUCCEED) {
                    saveAccount();
                }
            }
            if (result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_FAILED && setupWalkThrough.state === "extended") {
                PopupUtils.open(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"),
                    dekko,
                    {
                        title: qsTr("Failed to login in to the IMAP server"),
                        text: errorMessage
                    });
            }
        }
    }

    MailboxSettings {
        id: mboxSettings
        account: account.accountId ? account : null
        // Create the defaultvalues
        onAccountChanged: if (mboxSettings.account) {createDefaults()}
    }

    SearchForSpecialMailbox {
        id: searchForSpecialMailbox
        model: imapSettingVerifier.imapModel
        onSpecialMailboxFound: {
            console.log("find special", mailboxName, mailboxSpecialRole)
            if (mailboxSpecialRole === SearchForSpecialMailbox.MAILBOX_IS_SENT) {
                smtpSettings.saveToImap = true
                smtpSettings.sentMailboxName = mailboxName
                mboxSettings.write(mboxSettings.keys.saveToImap, true)
                mboxSettings.write(mboxSettings.keys.sentMailbox, mailboxName)
            }
        }
    }

    TaskProgressBar {
        anchors.top: header.bottom
        visible: imapSettingVerifier.result === ImapSettingVerifier.IMAP_SETTINGS_CONNECT_RUNNING
                    || searchForSpecialMailbox.isRunning
    }

    SetupWalkThrough {
        id: setupWalkThrough
        visible: !internal.waitingForAutoConfig
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onStartAutoConfig: {
            // We only run this on a generic account
            // There is no point for a pre-set as we have the info were looking for
            if (accountSetup.state === "generic-account") {
                autoConfig.start()
            }
        }

        onImapEncryptionMethodChanged: imapSettingVerifier.result = ImapSettingVerifier.IMAP_SETTINGS_CONNECT_UNKNOWN
        onImapPasswordChanged: imapSettingVerifier.result = ImapSettingVerifier.IMAP_SETTINGS_CONNECT_UNKNOWN
        onImapPortChanged: imapSettingVerifier.result = ImapSettingVerifier.IMAP_SETTINGS_CONNECT_UNKNOWN
        onImapServerChanged: imapSettingVerifier.result = ImapSettingVerifier.IMAP_SETTINGS_CONNECT_UNKNOWN
        onImapUsernameChanged: imapSettingVerifier.result = ImapSettingVerifier.IMAP_SETTINGS_CONNECT_UNKNOWN
    }
    // The idea here is to show something if save was clicked but we are
    // still waiting for autoconfig to complete.
    EmptyState {
        id: searchingMessage
        visible: internal.waitingForAutoConfig
        iconName: "reload" //TODO: decide on correct icon with design
        title: qsTr("Searching for account settings...")
        anchors {
            left: parent.left
            top: header.bottom
            topMargin: units.gu(15)
            right: parent.right
        }
    }

    // Let's not do any complex logic. If the Accounts validity criteria passed then it's going to
    // the accountsManager so let's just wait for the countChanged before going anywhere.
    Component.onCompleted: dekko.accountsManager.countChanged.connect(function() {pageStack.pop()})
}
