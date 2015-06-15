/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the Dekko email client for Ubuntu Touch/

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
import QtGraphicalEffects 1.0
import "../Utils/Utils.js" as Utils
import DekkoCore 0.2
import "../Components"


ListItemWithActions {
    id: normalMessageItemDelegate

    //---------------------------------
    // PROPERTIES
    //---------------------------------
    property bool isFirst: false
    property bool active: false
    //---------------------------------
    // SIGNALS
    //---------------------------------
    signal markMessageDeleted()
    signal markMessageFlagged()

    //---------------------------------
    // FUNCTIONS
    //---------------------------------

    //---------------------------------
    // OBJECT PROPERTIES
    //---------------------------------
    showDivider: loader_main.status === Loader.Ready
    height: units.gu(8)
    triggerActionOnMouseRelease: true

    //---------------------------------
    // COMPONENTS
    //---------------------------------
    property Component component: Component {
        id: component_main
        Item {
            property alias infoCol: inner_infoCol
            property alias fromLabel: inner_fromLabel
            property alias avatar: inner_avatar
            property alias infoDelegate: inner_infoDelegate
            property alias previewTextLabel: inner_previewTextLabel
            property alias previewTextLabelText: inner_previewTextLabel.text
            property alias subjectLabel: inner_subjectLabel
            property alias timeLabel: inner_timeLabel

            id: main
            anchors { fill: parent; /*margins: units.gu(0.2)*/ }
            Column {
                id: inner_infoDelegate
                anchors {
                    left: infoCol.right
                    right: parent.right
                    leftMargin: units.gu(1)
                }
                visible: model.isFetched
                spacing: units.gu(0.3)
                Label {
                    id: inner_fromLabel
                    text: Utils.formatMailAddresses(model.from, true)
                    fontSize: "large"
                    font.bold: !model.isMarkedRead
                    font.strikeout: model.isMarkedDeleted
                    font.weight: model.isMarkedRead ? Font.Light : Font.DemiBold
                    clip: true
                    width: parent.width - timeLabel.width - units.gu(5)
                    elide: Text.ElideRight
                }

                Label {
                    id: inner_subjectLabel
                    width: parent.width - timeLabel.width
                    text: model.subject
                    fontSize: "medium"
                    clip: true
                    color: Style.common.text
                    font.bold: !model.isMarkedRead
                    font.strikeout: model.isMarkedDeleted
                    elide: Text.ElideRight
                }

                Label {
                    id: inner_previewTextLabel
                    width: parent.width
                    wrapMode: Text.WordWrap
                    fontSize: "small"
                    maximumLineCount: GlobalSettings.preferences.previewLines
                    visible: text != ""
                    text: GlobalSettings.preferences.previewLines > 0 && model.preview ? model.preview  : ""
                    elide: Text.ElideRight
                    clip: true
                    // Bind this here as asking preview ? outside of the loader causes strange
                    // segfaults in ThreadingMsgListModel::handleDataChanged when trying to
                    // emit dataChanged on the translated indexes.
                    Binding {
                        target: normalMessageItemDelegate
                        property: "height"
                        // The idea here is that each line of the preview requires 2 gu
                        // So we only increase the height from 8 by 2 or 4 gu
                        value: units.gu(8 + (GlobalSettings.preferences.previewLines * 2))
                        when: inner_previewTextLabel.text
                    }
                }
            }

            Item {
                id: inner_avatar
                anchors {
                    top: parent.top
                    right: parent.right
                }
                width: units.gu(4)
                height: width
                visible: model.isFetched

                // TODO: Move position bindings from the component to the Loader.
                //       Check all uses of 'parent' inside the root element of the component.
                //       Rename all outer uses of the id 'inner_av' to 'loader_inner_av.item'.
                Component {
                    id: component_inner_av
                    CachedImage {
                        id: avatarCircle
                        height: units.gu(4)
                        width: height
                        anchors.centerIn: parent
                        color: Utils.getIconColor(fromLabel.text, Style.avatar.avatarColorList)
                        visible: model.isFetched
                        name: "avatar-circle"
                        Label {
                            color: "#FFFFFF"
                            visible: avatarCircle.status === Image.Ready
                            text: model.from ? Style.avatar.getInitialsForName(fromLabel.text) : ""
                            anchors.centerIn: parent
                            fontSize: "large"
                        }
                    }
                }
                Loader {
                    id: loader_inner_av
                    active: !inner_name.visible
                    sourceComponent: component_inner_av
                }
                CircleGravatar {
                    id: inner_name
                    emailAddress: model.from ? Utils.formatPlainArrayMailAddresses(model.from, false, true)[0] : ""
                    clip: true
                    anchors.fill: parent
                    visible: status === Image.Ready
                }
            }

            Column {
                id: inner_infoCol
                anchors {
                    left: parent.left
                    top: parent.top
                    topMargin: units.gu(0.4)
                }

                width: timeLabel.width
                spacing: units.gu(0.5)
                Label {
                    id: inner_timeLabel
                    text: Qt.formatTime(model.date, "hh:mm");
                    font.pixelSize: FontUtils.sizeToPixels("small")
                }
                CachedImage {
                    height: units.gu(2)
                    width: height
                    anchors.right: parent.right
                    visible: model.isFetched
                    name: model.isMarkedFlagged ? "starred" : "favorite-unselected"
                    color: model.isMarkedFlagged ? Style.actions.starred : "#888888"
                }

                CachedImage {
                    height: units.gu(2)
                    width: height
                    anchors.right: parent.right
                    visible: model.hasAttachments
                    name: "attachment"
                    color: "#888888"
                }

                CachedImage {
                    height: units.gu(2)
                    width: height
                    anchors.right: parent.right
                    visible: model.isMarkedReplied
                    name: "mail-replied"
                    color: "#888888"
                }
            }
        }
    }
    Loader {
        id: loader_main
        anchors.fill: parent
        visible: status == Loader.Ready
        sourceComponent: model.from ? component_main : undefined
    }


    //---------------------------------
    // TRANSITIONS
    //---------------------------------
    transitions: [
        Transition {
            UbuntuNumberAnimation {
                properties: "opacity"
            }
        }
    ]
}
