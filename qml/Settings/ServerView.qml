import QtQuick 2.3
import Ubuntu.Components 1.1
import DekkoCore 0.1

Page {
    id: serverView
    title: qsTr("Server Settings")
    property alias accountId: imapAccountSettings.accountId
    property alias imapSettings: imapAccountSettings
    property alias imapUserName: imapSettings.imapUserName
    property bool imapSettingsValid: imapSettings.settingsValid
    property bool smtpSettingsValid: smtpSettings.settingsValid
    property bool imapPasswordGiven: imapSettings.imapPassword
    property alias imapSettingsState: imapSettings.state
    property alias imapSettingsModified: imapSettings.settingsModified

    function save() {
        imapSettings.saveImapSettings()
        smtpSettings.saveSettings()
    }

    function savePasswords() {
        imapPasswordWatcher.setPassword(currentAccount.accountName, "imap", imapSettings.imapPassword)
        smtpPasswordWatcher.setPassword(currentAccount.accountName, "smtp", smtpSettings.smtpPassword)
    }

    head {
        sections {
            model:[qsTr("IMAP"), qsTr("SMTP")]
        }
    }

    ImapSettings {
        id: imapAccountSettings
    }

    // serverView.head.sections.selectedIndex
    flickable: null
    ImapSettings {
        id: imapSettings
        visible: serverView.head.sections.selectedIndex === 0
        anchors.fill: parent
//        anchors.margins: units.gu(1)
        account: currentAccount
    }

    SmtpSettings {
        id: smtpSettings
        visible: serverView.head.sections.selectedIndex === 1
        anchors.fill: parent
        account: currentAccount
    }
}
