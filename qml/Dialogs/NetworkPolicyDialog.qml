import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Connectivity 1.0

Dialog {
    id: networkPolicyDialog
    title: qsTr("Network Policy")
    text: qsTr("Bandwidth saving mode is a good choice for mobile data connections")
    OptionSelector {
        id: networkSelector
        // A simple enum type JS object
        // to avoid using numeric constants for ListItem index's
        property var connectionState: {'OFFLINE': 0, /*'EXPENSIVE': 1, */'ONLINE': 1}
        expanded: true
        model: [qsTr("Offline mode"), qsTr("Online mode")]
        onDelegateClicked: PopupUtils.close(networkPolicyDialog)
        onSelectedIndexChanged: {

            switch (selectedIndex){
            case connectionState.OFFLINE:
                if (dekko.currentAccount.imapModel.isNetworkOnline) {
                    break;
                }

                break
            case connectionState.ONLINE:
                dekko.accountsManager.networkStatus = NetworkingStatus.online
                break
            }
        }

        Component.onCompleted: {
            if (dekko.currentAccount.imapModel.isNetworkOnline) {
                networkSelector.selectedIndex = connectionState.ONLINE
            } else {
                networkSelector.selectedIndex = connectionState.OFFLINE
            }
        }
    }
}
