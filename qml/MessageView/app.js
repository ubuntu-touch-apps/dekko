/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko mail client

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
var UI;
window.onload = function () {
    function addClass(elem, className) {
        elem.className += ' ' + className;
    };
    function removeClass(elem, className) {
        elem.className = elem.className.replace(className, '');
    };
    if (!UI) {
        UI = new UbuntuUI();
        UI.init();
    }
};

var previousHeight = 0
var iframeTimeOut = 250
var iframes = document.getElementsByTagName("iframe");
window.setTimeout(setupIFrame(iframes), 10);
function setupIFrame(obj) {
    if (!obj.length) {
        return;
    }
    // This is probably the completely wrong way of doing this
    // but i'm not a web developer and this works, so i'm going with it :-D
    // Open to a better way of doing this.
    var resizeEvent = window.setInterval(function() {
        for(var ifr = 0; ifr < obj.length; ifr++) {
            if (obj[ifr].contentDocument.body.style.fontFamily != "Ubuntu") { // Means we havn't set this yet
                if (obj[ifr].id == "textHtml") {
                    obj[ifr].contentDocument.body.style.fontFamily = "Ubuntu";
                    obj[ifr].contentDocument.body.style.fontSize = "0.9rem";
                } else if (obj[ifr].id == "textPlain") {
                    obj[ifr].contentDocument.body.style.fontSize = "1.1rem";
                }
                obj[ifr].contentDocument.body.style.weight = "normal";
                obj[ifr].contentDocument.body.style.color = "#787878";
            }
            // Lets set it to the height of the message body and take up all available width
            if (obj[ifr].style.height != obj[ifr].contentDocument.body.scrollHeight+'px') {
                console.log("Changing iframe height");
                console.log("StyleHeight: " + obj[ifr].style.height);
                console.log("ScrollHeight: " + obj[ifr].contentDocument.body.scrollHeight)
                console.log("------------------------------------------------------------")
                obj[ifr].style.height = obj[ifr].contentDocument.body.scrollHeight +'px';
                obj[ifr].style.width = "100%";
                // So by default an iframe usually navigates within it's frame.
                // So for us to get the navigation requests to prompt to open in the browser
                // We change all the 'a' elements target attributes to be handled by the parent window.
                // The allow-top-navigation needs to be set on the iframe for this to work.
                var as = obj[ifr].contentDocument.getElementsByTagName('a');
                for(var i=0;i<as.length;i++){
                    if (as[i].target != '_parent') {
                        as[i].setAttribute('target','_parent');
                    }
                }
            } else {
                if (iframeTimeOut == 250) {
                    console.log("[IFRAME] No resize needed")
                    console.log("[IFRAME] Clearing timeout interval and restarting to wait 1000...");
                    console.log("that should be long enough that were no longer spanking the webview");
                    clearInterval(resizeEvent);
                    iframeTimeOut = 1000;
                    setupIFrame(iframes);
                }
            }
        }
    }, iframeTimeOut);
}

var activeAttachmentPopover;
var currentAttachment;
function attachmentClicked(attachment) {
    currentAttachment = attachment;/*
    activeAttachmentPopover = UI.popover(attachment, "attachment_options_popover");
    activeAttachmentPopover.show();
    document.getElementById("popover_inverse_mousearea").style.display = 'block';*/
    // if mimetype is message/rfc822, display attachment in a new messageview, otherwise download the attachment
    var mimetype = attachment.attributes["data-mimetype"].value;
    if (mimetype === "message/rfc822") {
        var partId = attachment.id.substring("attachment_".length);
        displayEmbeddedMessage(partId);
    } else {
        downloadAttachment();
    }
}

function closeAttachmentPopover() {
    activeAttachmentPopover.hide();
    document.getElementById("popover_inverse_mousearea").style.display = 'none';
}

function downloadAttachment() {
//    closeAttachmentPopover();
    // This is a neat little trick. Change the title
    // and use the onTitleChanged in qml to get the attachment id
    // Awesome!!
    document.title = currentAttachment.id;
    // Set a timeout to reset the page title
    // So that it doesn't stop this attachment being clicked again straight after
    window.setTimeout(function() {
        document.title = "";}
    , 50);
}

function toggleExpandAttachment() {
    var elem = document.getElementById("attachments_list");
    if (elem.className.indexOf("expanded") >= 0) {
        elem.className = elem.className.replace("expanded", '');
    } else {
        elem.className += " " + "expanded";
    }
}

function displayEmbeddedMessage(partId) {
    document.title = "viewEmbeddedMessage_" + partId;
    window.setTimeout(function() {
        document.title = "";}
    , 50);
}

var recipientPopover;
var addressOfRecipientSelected;
function recipientClicked(recipient) {
    if (!UI) {
        UI = new UbuntuUI();
        UI.init();
    }
    recipientPopover = UI.popover(recipient, "recipient-popover");
    addressOfRecipientSelected = recipient.attributes["data-mailto"].value;
    var contactSaved = recipient.className.indexOf("recipient-saved") >= 0;
    var recipientPopoverElement = document.getElementById("recipient-popover");
    recipientPopoverElement.className = recipientPopoverElement.className.replace("recipient-saved", "");
    if (contactSaved) {
        recipientPopoverElement.className += " recipient-saved";
    }

    recipientPopover.show();
    document.getElementById("popover_inverse_mousearea").style.display = 'block';
}

function closePopovers() {
    if (recipientPopover) {
        recipientPopover.hide();
        recipientPopover = false;
    }
    document.getElementById("popover_inverse_mousearea").style.display = 'none';
}

function composeMessageTo() {
    document.title = "composeMessageTo_" + addressOfRecipientSelected;
    window.setTimeout(function() {
        document.title = "";}
    , 50);
    closePopovers();
}

function addToAddressbook() {
    document.title = "addToAddressbook_" + addressOfRecipientSelected;
    window.setTimeout(function() {
        document.title = "";}
    , 50);
    closePopovers();
}

function askForContactSaved() {
    var recipientsLinks = document.getElementsByClassName("recipient");
    var emails = [];
    for (var i = 0; i < recipientsLinks.length; i++) {
        emails.push(recipientsLinks[i].attributes["data-mailto"].value);
    }
    document.title = "askForContactSaved_" + JSON.stringify(emails)
    window.setTimeout(function() {
        document.title = "";}
    , 50);
}

askForContactSaved();
