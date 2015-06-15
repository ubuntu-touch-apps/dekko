/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

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
import QtFeedback 5.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import com.canonical.Oxide 1.3
import DekkoCore 0.2
import "../Dialogs"

WebView {
    id: webview
    signal openAttachment(var partId)
    signal openEmbeddedMessage(var partId)
    signal composeMessageTo(var address)
    signal addToAddressbook(var address)

    // LOCK IT DOWN!!
    preferences {
        javascriptEnabled: true
        javascriptCanAccessClipboard: true
        allowUniversalAccessFromFileUrls: true
        loadsImagesAutomatically: GlobalSettings.preferences.autoLoadImages
        allowScriptsToCloseWindows: false
        appCacheEnabled: true
        localStorageEnabled: false
        fixedFontFamily: "Ubuntu"
        serifFontFamily: "Ubuntu"
        sanSerifFontFamily:"Ubuntu"
    }

    context: WebContext {
        devtoolsEnabled: true
        devtoolsPort: 9232
        // These are our internal schemes. For the html parts inside
        // the sandboxed iframe we will be able to set the src attribute to our
        // trojita-imap://msg/0/1 scheme and it will fetch that part :-) no need to wait on
        // manual fetching here \o/
        allowedExtraUrlSchemes: ["trojita-imap", "cid"]
        popupBlockerEnabled: true
        userScripts: [
            UserScript {
                context: "dekko://"
                url: Qt.resolvedUrl("contact-message.js")
                matchAllFrames: true
            }
        ]
    }

    onContentHeightChanged: {
        console.log("[Dekko Web View] Content height change", contentHeight)
    }
    onNavigationRequested: {
        console.log("[Dekko Web View] RequestUrl", request.url)
        PopupUtils.open(linkClickedDialog, oneMessagePage, {externalLink: request.url})
        request.action = 255

    }

    onJavaScriptConsoleMessage: {
        var msg = "[Dekko Web View] [JS] (%1:%2) %3".arg(sourceId).arg(lineNumber).arg(message)
        console.log(msg)
    }
    onTitleChanged: {
        if (!title)
            return
        if (title.indexOf("attachment_") == 0) {
            // If the title changes then the attachment was clicked
            // so prompt for user to accept download
            console.log("[Dekko Web View] Attachment Clicked", title)
            // TODO: add a hapticfeedback buzz here, just to make it feel like it was clicked
            clickEffect.start()
            var downloadPartId = title.substring("attachment_".length);
            // open dialog
            openAttachment(downloadPartId);
        } else if (title.indexOf("viewEmbeddedMessage_") == 0) {
            clickEffect.start()
            var partId = title.substring("viewEmbeddedMessage_".length);
            openEmbeddedMessage(partId);
        } else if (title.indexOf("composeMessageTo_") == 0) {
            clickEffect.start()
            var address = title.substring("composeMessageTo_".length);
            composeMessageTo(address)
        } else if (title.indexOf("addToAddressbook_") == 0) {
            clickEffect.start()
            var address = title.substring("addToAddressbook_".length);
            addToAddressbook(address)
            var saveContactResult = {};
            saveContactResult[address] = true;
            webview.rootFrame.sendMessageNoReply("dekko://", "CONTACT_SAVED", saveContactResult);
        } else if (title.indexOf("askForContactSaved_") == 0) {
            var addresses = JSON.parse(title.substring("askForContactSaved_".length));
            var result = {}
            for (var i in addresses) {
                if (addressbookModel.containsEmail(addresses[i])) {
                    result[addresses[i]] =  true;
                }
            }
            webview.rootFrame.sendMessageNoReply("dekko://", "CONTACT_SAVED", result);
        }
    }

    HapticsEffect {
        id: clickEffect
        attackIntensity: 0.0
        attackTime: 50
        intensity: 1.0
        duration: 10
        fadeTime: 50
        fadeIntensity: 0.0
    }

    Component {
        id: linkClickedDialog
        ConfirmationDialog {
            id: linkDialog
            property string externalLink
            title: qsTr("Open in browser?")
            text: qsTr("Confirm to open %1 in web browser").arg(externalLink.substring(0, 30))
            onConfirmClicked: Qt.openUrlExternally(externalLink)
        }
    }
}

