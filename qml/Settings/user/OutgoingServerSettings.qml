import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../Components"
import "../common"

SettingsGroupBase {

    onSave: {
        console.log("Save called")
        smtpPassword.setPassword(smtpPasswordField.text)
        smtpSettings.submissionMethod = smtpEncryptionSelector.method
        smtpSettings.username = smtpEmailField.text
        smtpSettings.server = smtpServerField.text
        smtpSettings.port = smtpPortField.text
        smtpSettings.save()
        saveComplete()
    }

    property SmtpSettings smtpSettings: root.account.smtpSettings
    // This is a crude hack to ensure we don't change the current saved port value
    // when the UI is loading. The encryption selector changes index twice during loading
    // which overwrites the portfield value if it isn't using a standard port for the connection method
    function __updatePortField(port) {
        smtpPortField.text = smtpEncryptionSelector.port
    }

    function determineIfSettingsChanged() {
        if (smtpEncryptionSelector.method === smtpSettings.submissionMethod
                && smtpServerField.text === smtpSettings.server
                && smtpPortField.text === parseInt(smtpSettings.port)
                && smtpEmailField.text === smtpSettings.username
                && smtpPasswordField.text === smtpPassword.currentPassword) {
            settingsChanged(false)
        } else {
            settingsChanged(true)
        }
    }

    TitledTextField {
        id: smtpServerField
        title: qsTr("Host name")
        text: smtpSettings.server
        onTextChanged: determineIfSettingsChanged()
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: !text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpPortField.textFieldFocusHandle
    }


    TitledTextField {
        id: smtpEmailField
        title: qsTr("Username")
        text: smtpSettings.username
        onTextChanged: determineIfSettingsChanged()
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
        sendTabEventOnEnter: true
        requiredField: !text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpPasswordField.textFieldFocusHandle
    }

    PasswordManager {
        id: smtpPassword
        property string currentPassword
        accountId: root.account.accountId
        type: PasswordManager.SMTP
        onPasswordNowAvailable: {
            currentPassword = passwordObject.password
            smtpPasswordField.text = passwordObject.password
        }
        Component.onCompleted: requestPassword()
    }

    TitledTextField {
        id: smtpPasswordField
        title: qsTr("Password")
        sendTabEventOnEnter: true
        onTextChanged: smtpPassword.currentPassword ? determineIfSettingsChanged() : undefined
        inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        echoMode: showSmtpPassword.checked ? TextInput.Normal : TextInput.Password
        KeyNavigation.priority: KeyNavigation.BeforeItem

    }

    CheckboxWithLabel {
        id: showSmtpPassword
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            right: parent.right
            rightMargin: units.gu(2)
        }
        text: qsTr("Show password")
    }

    TitledTextField {
        id: smtpPortField
        title: qsTr("Port")
        text: parseInt(smtpSettings.port)
        onTextChanged: determineIfSettingsChanged()
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
        sendTabEventOnEnter: true
        requiredField: !text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpEmailField.textFieldFocusHandle
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
                id: smtpEncryptionSelector
                type: "smtp"
                anchors {
                    left: parent.left
                    right: parent.right
                }
                onMethodChanged: determineIfSettingsChanged()
                onPortChanged: __updatePortField()
            }
        }
    }

}
