import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../Components"
import "../common"

SettingsGroupBase {

    onSave: {
        console.log("Save called")
        imapPassword.setPassword(imapPasswordField.text)
        imapSettings.connectionMethod = imapEncryptionSelector.method
        imapSettings.username = imapEmailField.text
        imapSettings.server = imapServerField.text
        imapSettings.port = imapPortField.text
        imapSettings.save()
        saveComplete()
    }

    property ImapSettings imapSettings: root.account.imapSettings
    // This is a crude hack to ensure we don't change the current saved port value
    // when the UI is loading. The encryption selector changes index twice during loading
    // which overwrites the portfield value if it isn't using a standard port for the connection method
    function __updatePortField(port) {
        imapPortField.text = imapEncryptionSelector.port
    }

    function determineIfSettingsChanged() {
        if (imapEncryptionSelector.method === imapSettings.connectionMethod
                && imapServerField.text === imapSettings.server
                && imapPortField.text === parseInt(imapSettings.port)
                && imapEmailField.text === imapSettings.username
                && imapPasswordField.text === imapPassword.currentPassword) {
            settingsChanged(false)
        } else {
            settingsChanged(true)
        }
    }



    TitledTextField {
        id: imapServerField
        title: qsTr("Host name")
        text: imapSettings.server
        onTextChanged: determineIfSettingsChanged()
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: !text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapPortField.textFieldFocusHandle
    }

    TitledTextField {
        id: imapEmailField
        title: qsTr("Username")
        text: imapSettings.username
        onTextChanged: determineIfSettingsChanged()
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: !text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapPasswordField.textFieldFocusHandle
    }

    PasswordManager {
        id: imapPassword
        property string currentPassword
        accountId: root.account.accountId
        type: PasswordManager.IMAP
        onPasswordNowAvailable: {
            currentPassword = passwordObject.password
            imapPasswordField.text = passwordObject.password
        }
        Component.onCompleted: requestPassword()
    }

    TitledTextField {
        id: imapPasswordField
        title: qsTr("Password")
        sendTabEventOnEnter: true
        onTextChanged: imapPassword.currentPassword ? determineIfSettingsChanged() : undefined
        inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        echoMode: showImapPassword.checked ? TextInput.Normal : TextInput.Password
        KeyNavigation.priority: KeyNavigation.BeforeItem

    }

    CheckboxWithLabel {
        id: showImapPassword
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            right: parent.right
            rightMargin: units.gu(2)
        }
        text: qsTr("Show password")
    }

    TitledTextField {
        id: imapPortField
        title: qsTr("Port")
        text: parseInt(imapSettings.port)
        onTextChanged: determineIfSettingsChanged()
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
        sendTabEventOnEnter: true
        requiredField: !text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapEmailField.textFieldFocusHandle
    }
    // Because we are starting a ListItem half way down a column we have no divider on the top
    // So we need to wrap the first listitem in an item and add a custom divider
    // Then the rest to follow can just work as normal
    Item {
        width: parent.width
        height: col.height
        Column {
            id: col
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            Rectangle {
                width: parent.width
                height: units.dp(2)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.1) }
                    GradientStop { position: 0.49; color: Qt.rgba(0, 0, 0, 0.1) }
                    GradientStop { position: 0.5; color: Qt.rgba(1, 1, 1, 0.4) }
                    GradientStop { position: 1.0; color: Qt.rgba(1, 1, 1, 0.4) }
                }
            }
            EncryptionSelector {
                id: imapEncryptionSelector
                type: "imap"
                anchors {
                    left: parent.left
                    right: parent.right
                }
                onMethodChanged: determineIfSettingsChanged()
                onPortChanged: __updatePortField()
            }
        }
    }


    ListItem.Standard {
        text: qsTr("Show only subscribed folders")
        control: DekkoSwitch {
            id: s
            //            onClicked: GlobalSettings.preferences.preferPlainText = checked
            //            Component.onCompleted: checked = GlobalSettings.preferences.preferPlainText
        }
    }

    ListItem.Standard {
        text: qsTr("Server capabilities")
        onClicked: pageStack.push(Qt.resolvedUrl("../../DeveloperMode/ImapCapabilitiesView.qml"), {account: root.account})
    }

}
