import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import DekkoCore 0.2
import "../Dialogs"

QtObject {
    id: internal

    property AccountsManager accountsManager
    property bool networkOffline: true
    property bool authenticateRequested: false
    property bool authenticateHasFailed: false
    property bool networkErrorRecieved: false
    property PopupQueue popupQueue: PopupQueue {}


    function showImapError(message) {
        console.log("[IMAP ERROR] Imap error recieved with message: " + message.error)
        showInfoMessageToUser(message.username, message.error)
    }
    function showNetworkError(message) {

        console.log("[NETWORK ERROR] Network error recieved with message " + message.error)
        // We only want to show this if
        // 1) We are not trying to connect without an sslMode i.e first run while setting up connections
        // 2) the dialog isn't already visible, and further reconnect attempts are firing the networkError signal
        if (!networkErrorRecieved) {
            popupQueue.queue(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"), dekko, {
                                 networkError: true,
                                 title: message.username,
                                 text: message.error
                             })
        }
    }
    function showImapAlert(message) {
        console.log("[IMAP ALERT] imap alert recieved with message: " + message)
        showInfoMessageToUser(message.username, message.message)
    }

    function passwordReadingFailed(message) {
        console.log("[LOG] Password Reading Failed:" + message)
        authAttemptFailed(message)
        PopupUtils.open(passwordDialogComponent)
    }

    function authAttemptFailed(account) {
        // TODO: display the correct info from the account object
//            console.log("[IMAP ERROR] Authentication failed with message: " + message)

        if (!authenticateHasFailed) {
            authenticateHasFailed = true

            popupQueue.queue(Qt.resolvedUrl("../Dialogs/PasswordDialog.qml"), dekko, {
                                 type: "imap",
                                 accountId: account.accountId,
                                 authErrorMessage: account.authError
                             }
                             )
        }
    }

    function showInfoMessageToUser(title, message) {
        popupQueue.queue(Qt.resolvedUrl("../Dialogs/InfoDialog.qml"), dekko, {
                             title: title,
                             text: message
                         })
    }

    function passwordAvailable(pwdObj) {
        if (authenticateHasFailed) {
            // The authAttemptFailed function has already been called and
            // Is now handling this so let's not
            // Add an additional pwd dialog to the queue with a wrong password
            // The OpenConnectionTask has asked for another authentication attempt after
            // informing us the last one failed. Now they produce signals at *around* the same time
            // So we let the first one win i.e authAttemptFailed.
            console.log("authAttemptFailed has already been called")
            return;
        } else if (pwdObj.password) {
            if (pwdObj.type === "imap") {
                authenticateRequested = false
                accountsManager.setPassword(pwdObj.accountId, pwdObj.password)
            } else {
                dekko.submissionManager.setPassword(pwdObj.password);
            }
        } else {
            // Along the way we lost some account info like username. The Password dialog
            // should contain a relevant Settings instance which we set the pwdObj.accountId
            // Then we can load the imap/smtp settings to get the username and server to display to the user when
            // promting for password.

            // The only time we should reach here is when we currently have no knowledge of a password.
            popupQueue.queue(Qt.resolvedUrl("../Dialogs/PasswordDialog.qml"), dekko, {
                                 type: pwdObj.type,
                                 accountId: pwdObj.accountId,
                             }
                             )
        }
    }

    function accountUsesUOA(accountObj) {
        // This accountObj contains:
        // - Our accountId
        // - the uoaAccountId whichs is the accountId that online accounts uses
        // - and a bool OAUTH value to state wether the return type we get from UOA is going to be a oauth token or a password
        console.log("Still needs implementing")

    }

    function showSslInfo(sslInfo) {
        PopupUtils.open(sslSheetPage, dekko,
                        {
                            accountId: sslInfo.accountId,
                            title: sslInfo.title,
                            htmlText: sslInfo.message
                        }
                        )
    }
}
