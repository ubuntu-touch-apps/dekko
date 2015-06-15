import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import DekkoCore 0.2
import "../../Components"
import "../common"

SettingsGroupBase {

    onSave: {
        console.log("Save called")
        root.account.profile.organization = companyField.text
        root.account.profile.description = descriptionField.text
        root.account.profile.save()
        saveComplete()
    }

    function determineIfSettingsChanged() {
        if (companyField.text === profile.organization && descriptionField.text === profile.description) {
            settingsChanged(false)
            return
        }
        settingsChanged(true)
    }

    property AccountProfile profile: root.account.profile

    TitledTextField {
        id: descriptionField
        title: qsTr("Description")
        text: profile.description
        onTextChanged: determineIfSettingsChanged()
        sendTabEventOnEnter: true
        placeholderText: qsTr("Eg: \"Personal\" or \"Work\"")
        font.capitalization: Font.Capitalize
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: companyField.textFieldFocusHandle
    }

    TitledTextField {
        id: companyField
        title: qsTr("Company (optional)")
        text: profile.organization
        onTextChanged: determineIfSettingsChanged()
        sendTabEventOnEnter: true
        KeyNavigation.priority: KeyNavigation.BeforeItem
    }
}
