import QtQuick 2.3
import Ubuntu.Components 1.1
import DekkoCore 0.2
import "../../Components"

Page {
    id: newIdentityPage

    flickable: null

    property SenderIdentity identity
    property bool edit: false
    Component.onCompleted: {
        if (edit) {
            input.nameField = identity.name
            input.emailField = identity.email
            input.organizationField = identity.organization
            input.signatureField = identity.signature
        }
    }

    DekkoHeader {
        id: header
        title: edit ? qsTr("Edit identity") : qsTr("New identity")
        backAction: Action {
            iconName: "back"
            onTriggered: pageStack.pop()
        }
        primaryAction: Action {
            id: saveAction
            iconName: "ok"
            onTriggered: {
                if (edit) {
                    identity.name = input.nameField
                    identity.email = input.emailField
                    identity.organization = input.organizationField
                    identity.signature = input.signatureField
                } else {
                    model.appendRow(newIdentity)
                }
                closing()
                pageStack.pop()
            }
        }
    }

    property QtObject model

    signal closing()

    SenderIdentity {
        id: newIdentity
        name: input.nameField
        email: input.emailField
        organization: input.organizationField
        signature: input.signatureField

    }
    Flickable {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentHeight: input.height + units.gu(5)
        SenderIdentityInput {
            id: input
        }
    }

}
