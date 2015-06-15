/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Touch/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItems
import Ubuntu.PushNotifications 0.1
import Ubuntu.Connectivity 1.0
import DekkoCore 0.2
import TrojitaCore 0.1
import Ubuntu.Content 0.1
import "./AccountsView"
import "./Dialogs"
import "./MailboxView"
import "./MessageListView"
import "./Utils"
import "./Contact"
import "./Notifications"

MainView{
    id: dekko

    signal startService()
    signal stopService()

    //-----------------------------------
    // PROPERTIES
    //-----------------------------------
    // This property holds the currently selected account
    // and should be used throughout all other views
    // e.g For a listview set the model to dekko.currentAccount.imapModel
    property alias currentAccount: accountsManager.currentAccount
    property bool msgListPushed: false

    property var mailboxListPage;

    // So we can access it further into the pageStack
    property alias accountsManager: accountsManager

    property bool networkAvailable: accountsManager.networkStatus === NetworkingStatus.online
    property var submissionManager;

    //-----------------------------------
    // SIGNALS
    //-----------------------------------
    signal reconnectAttemptScheduled(int timeout)
    signal showAccounts()
    signal autoConfigComplete(var config, string username, string password)
    signal manualConfig(string username, string password)

    //----------------------------------
    // SLOTS
    //----------------------------------
    onReconnectAttemptScheduled: console.log("Reconnect in: " + timeout)

    //----------------------------------
    // OBJECT PROPERTIES
    //----------------------------------
    objectName: "dekko" // object name for functional testing
    applicationName: "dekko.dekkoproject"
    automaticOrientation: false
    anchorToKeyboard: true // ensure page doesn't end up behind the OSK
    // Random values to simulate a phone sized window on desktop
    // THis get's overriden on device
    width: units.gu(45); height: units.gu(71)
    useDeprecatedToolbar: false // disable use of the "old style" toolbar

    backgroundColor: Style.common.background
    //--------------------------------
    // FUNCTIONS
    //--------------------------------
    function setAuthHasFailed(failed) {
        alertQueue.authenticateHasFailed = false
    }

    function openSettings(viewToReturnTo) {
        pageStack.clear()
        pageStack.push(Qt.resolvedUrl("./Settings/global/GlobalSettingsListView.qml"), {previousView: viewToReturnTo})
    }

    function openMailTo(url) {
        console.log("Opening mailto: ", url)
        pageStack.push(Qt.resolvedUrl("./Composer/ComposePage.qml"),
                       {
                           replyMode: ReplyMode.REPLY_NONE,
                           pageTitle:qsTr("New Message"),
                           mailtoUrl: url
                       });
    }

    function shareImportAttachments(files) {
        var composePage = pageStack.push(Qt.resolvedUrl("./Composer/ComposePage.qml"),
                                         {
                                             replyMode: ReplyMode.REPLY_NONE,
                                             pageTitle:qsTr("New Message"),
                                         });
        composePage.addAttachmentsFromUrls(files);
    }

    //--------------------------------
    // ACTIONS
    //--------------------------------

    //--------------------------------
    // PRIVATE PROPERTIES & FUNCTIONS
    //--------------------------------
    AccountsManager {
        id: accountsManager
        networkStatus: NetworkingStatus.online
        onImapError: alertQueue.showImapError(error)
        // FIXME: we are to display a non invasive inline status for this that doesn't prevent app
        // interaction
        // see: https://docs.google.com/a/canonical.com/presentation/d/1woHjO8K4iqyVZZlfQ4BXL0DhYbwkEmZ7wvcUhYzHDRk/edit#slide=id.g183c60488_08 for example
        //        onNetworkError: alertQueue.showNetworkError(error)
        onAlertReceived: alertQueue.showImapAlert(message)
        onAuthRequested: {
            alertQueue.authenticateRequested = true
            passwordManager.requestPassword(account.accountId, PasswordManager.IMAP)
        }
        //        onConnectionStateChanged: console.log("STATE CHANGED")
        onAuthAttemptFailed: alertQueue.authAttemptFailed(account)
        onCheckSslPolicy: {
            // FIXME: We need to re-enable this once we have re-implemented the ssl sheet
            // also it needs to work nicely with the opening of an imap connection and
            // auto opening the inbox for a new account
            console.log("CHECK SSL INFO CALLED")
            alertQueue.showSslInfo(sslInfo)
        }
        onError: console.log(msg)
        onCurrentAccountChanged: {
            if (currentAccount !== null) {
                console.log("Current Account Changed \o/ The id is: " + currentAccount.accountId)
                //                currentAccount.network.setNetworkExpensive()
            } else {
                console.log("[WARNING] Unloaded current account.... the next account that opens a connection
                            is going to steal the msglistview. Be sure this is what you want!!")
            }
        }
        onNoAccountsConfigured: {
            pageStack.pop()
            pageStack.push(Qt.resolvedUrl("./AccountsView/AccountsPage.qml"))
        }

        Component.onCompleted: openAccounts()
    }

    PasswordManager {
        id: passwordManager
        accountsManager: accountsManager
        onPasswordNowAvailable: alertQueue.passwordAvailable(passwordObject)
        onAccountUsesOnlineAccounts: alertQueue.accountUsesUOA(accountObject)
        onError: console.log(message)
        onReadingFailed: console.log(message)
        onWritingFailed: console.log(message)
        onJobCountChanged: console.log("Number of running password jobs: " + jobsInProgress)
        onPasswordStored: console.log("Password stored for accountId: " + id)
    }

    QueuedApplicationAlertManager {
        id: alertQueue
        accountsManager: dekko.accountsManager
    }

    NotificationsService {
        id: notificationsService
    }

    // TODO: Change this as sheets are going to be deprecated
    Component{
        id: sslSheetPage
        SslSheet {
            id: sslSheet
            property string accountId
            onConfirmClicked: accountsManager.setSslPolicy(accountId, true)
            onCancelClicked: accountsManager.setSslPolicy(accountId, false)
        }
    }

    PageStack{
        id:pageStack
        onDepthChanged: console.log("[PAGESTACK] depth = " + depth)
        Component.onCompleted: {
            pageStack.push(Qt.resolvedUrl("./MessageListView/MessageListPage.qml"))
        }
    }

    AddressbookModel {
        id: addressbookModel
        databaseName: "addressbook"
    }

    ContactFilterModel {
        id: addressbookModelFiltered
        sourceModel: addressbookModel
    }

    Connections {
        target: ContentHub
        onImportRequested: shareImportAttachments(transfer.items)
        onShareRequested: shareImportAttachments(transfer.items)
    }

    property var uri: undefined

    Connections {
        target: UriHandler
        onOpened: {
            dekko.uri = uris[0];
            console.log("uri opened: " + dekko.uri);
            processUri();
        }
    }

    function processUri() {
        if (typeof dekko.uri === "undefined") {
            return;
        }
        var mailto = dekko.uri.split(":")[0];
        if (mailto === "mailto") {
            openMailTo(dekko.uri)
        }
        var commands = dekko.uri.split("://")[1].split("/")
        if (commands) {
            switch(commands[0].toLowerCase()) {
            case "notify":
                console.log("notify received")
                pageStack.clear()
                accountsManager.openMailbox(commands[1], commands[2]);
                pageStack.push(Qt.resolvedUrl("./MessageListView/MessageListPage.qml"))
                break;
            }
        }
    }


    Component.onCompleted: {
        if (appUris) {
            dekko.uri = appUris[0]
            processUri()
        }
    }
}
