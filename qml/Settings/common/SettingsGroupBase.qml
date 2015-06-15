import QtQuick 2.3
import Ubuntu.Components 1.1

Column {
    id: settingsBase
    // This is required to be emitted on any settings that have changed
    // The bool changed value is to let parent's know that the settings were changed
    // but the values are actually still the same
    signal settingsChanged(bool changed)
    // This signal get's emitted when the back button is pressed
    signal save()
    signal saveComplete()

    readonly property alias hasInvalidFields: internal.invalidFields

    function selfDestroy() {
        settingsBase.destroy()
    }

    anchors {
        left: parent.left
        top: parent.top
        topMargin: units.gu(2)
        right: parent.right
    }
    spacing: units.gu(1)

    QtObject {
        id: internal
        property bool invalidFields: false
    }
}
