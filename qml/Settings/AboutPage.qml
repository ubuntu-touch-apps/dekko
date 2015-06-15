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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import DekkoCore 0.2
import "../Components"

Page {
    id: root

    Action {
        id: developerMode
        onTriggered: GlobalSettings.developer.developerModeEnabled = !GlobalSettings.developer.developerModeEnabled
    }
    DekkoHeader {
        id: header
        title: qsTr("About")
        backAction: Action {
            iconName: "back"
            onTriggered: pageStack.pop()
        }
    }

    Column {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        anchors.topMargin: units.gu(5)

        Item {
            width: parent.width
            height: units.gu(6)
            Label {
                text: qsTr("Dekko Email Client")
                anchors.centerIn: parent
                fontSize: "x-large"
            }
        }

        Item {
            width: parent.width
            height: units.gu(15)

            UbuntuShape {
                radius: "medium"
                image: Image {
                    source: Qt.resolvedUrl("../../dekko.png")
                }
                height: units.gu(12); width: height
                anchors.centerIn: parent

                MouseArea {
                    anchors.fill: parent
                    onPressAndHold: {
                        PopupUtils.open(Qt.resolvedUrl("../Dialogs/ConfirmationDialog.qml"),
                                        root,
                                        {
                                            confirmButtonText: GlobalSettings.developer.developerModeEnabled ? "Disable" : "Enable",
                                            confirmAction: developerMode,
                                            title: "Developer Mode",
                                            text: GlobalSettings.developer.developerModeEnabled ? "Disable developer mode" : "Enable developer mode"
                                        }
                                        )
                    }
                }
            }
        }
        Item {
            width: parent.width
            height: visible ? units.gu(5) : units.gu(0)
            visible: GlobalSettings.developer.developerModeEnabled
            Label {
                text: "Developer mode is enabled"
                color: UbuntuColors.red
                anchors.centerIn: parent
            }
        }

        Item {
            width: parent.width
            height: units.gu(8)
            Label {
                text: "Version 0.5.4"
                fontSize: "large"
                color: UbuntuColors.lightAubergine
                anchors.centerIn: parent

            }
        }

        Item {
            width: parent.width
            height: units.gu(2)
        }

        Item {
            width: parent.width
            height: donateLabel.height + units.gu(2)
            Label {
                id: donateLabel
                text: qsTr("Want to say thanks?<br>Then please consider making a donation.")
                anchors.centerIn: parent
                wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - units.gu(12)
            }
        }
        Item {
            width: parent.width
            height: units.gu(7)
            Button {
                anchors.centerIn: parent
                width: units.gu(15)

                color: Style.header.background
                text: "Donate"
                onClicked: Qt.openUrlExternally("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dpniel%40ubuntu%2ecom&lc=GB&item_name=Dekko%20email%20client&item_number=Dekko&currency_code=GBP&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted")
            }
        }
        Item {
            width: parent.width
            height: recur.height
            Label {
                id: recur
                fontSize: "small"
                text: qsTr("If you wish to support the future development of Dekko, please consider making recurring donations.")
                anchors.centerIn: parent
                wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - units.gu(12)
            }
        }
    }
}
