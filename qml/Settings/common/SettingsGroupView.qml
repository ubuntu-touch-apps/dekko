import QtQuick 2.3

Flickable {
    id: settingsFlicker
    property var component: null
    property var view: null
    // This signal is emitted when settings have been changed
    // The created settings group which inherits SettingsGroupBase will
    // connect to this signal and should be used as a suggestion to prompt about
    // saving any changes
    signal settingsChanged(bool changed)
    signal componentLoaded()
    signal error(string error)
    signal save()
    signal saveComplete()

    signal destroyChildren()

    clip: true
    contentHeight: view ? view.height + units.gu(5) : 0

    function createSettingsObject(objPath) {
        console.log("Create component")
        component = Qt.createComponent(objPath);
        if (component.status === Component.Ready) {
            addObjectToView();
        } else {
            component.statusChanged.connect(addObjectToView);
        }
    }

    function addObjectToView() {
        if (component.status === Component.Ready) {
            view = component.createObject(settingsFlicker.contentItem);
            if(view === null) {
                error("Error creating settings view")
                return;
            } else {
                view.settingsChanged.connect(settingsFlicker.settingsChanged)
                view.saveComplete.connect(settingsFlicker.saveComplete)
                settingsFlicker.save.connect(view.save)
                settingsFlicker.destroyChildren.connect(view.selfDestroy)
                console.log("Component created")
                componentLoaded()
            }
        } else if (component.status === Component.Error) {
            error("Error loading settings view: " + component.errorString())
        }
    }

}
