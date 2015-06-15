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
import QtGraphicalEffects 1.0
import DekkoCore 0.2

Item {
    id: item
    property string emailAddress
    property alias source: inner_image.source
    property alias status: inner_image.status
    property alias sourceSize: inner_image.sourceSize
//    property url gravatarUrl: emailAddress ? "http://www.gravatar.com/avatar/%1?s=80&d=404".arg(Qt.md5(emailAddress)) : ""
    // The cool thing here is libravatar will redirect us to gravatar if no avatar is found in their database :-)
    // See https://wiki.libravatar.org/api/ for reference.
    property url libravatarUrl: emailAddress ? "http://www.gravatar.com/avatar/%1?d=404&s=%2".arg(Qt.md5(emailAddress)).arg(width) : ""
    readonly property url circleMask: "circle.png"

    width: units.gu(4)
    height: width


    Image {
        id: inner_image
        anchors.fill: parent
        smooth: true
        visible: false
        asynchronous: true
        ImageHelper on source {
            id: helper
            gravatarUrl: GlobalSettings.preferences.gravatarAllowed ? libravatarUrl : ""
            gravatarEmail: emailAddress
            size: item.width
        }
    }

    Image {
        id: inner_mask
        source: circleMask
        anchors.fill: inner_image
        smooth: true
        visible: false
        mipmap: true
    }

    OpacityMask {
        anchors.fill: inner_image
        source: inner_image
        maskSource: inner_mask
    }
}
