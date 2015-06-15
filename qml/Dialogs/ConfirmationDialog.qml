import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0

Dialog {
    id: confirmationDialog

    property Action confirmAction
    property Action cancelAction
    property alias confirmButtonText: confirmButton.text
    property alias cancelButtonText: cancelButton.text

    signal confirmClicked
    signal cancelClicked

    contents: [
        Button {
            id: confirmButton
            text: qsTr("Confirm")
            color: UbuntuColors.green
            onClicked: {
                if (confirmAction) {
                    confirmAction.triggered(confirmButton)
                }
                PopupUtils.close(confirmationDialog)
                confirmClicked()
            }
        },
        Button {
            id: cancelButton
            text: qsTr("Cancel")

            color: UbuntuColors.red
            onClicked: {
                if (cancelAction) {
                    cancelAction.triggered(cancelButton)
                }
                PopupUtils.close(confirmationDialog)
                cancelClicked()
            }
        }
    ]
}
