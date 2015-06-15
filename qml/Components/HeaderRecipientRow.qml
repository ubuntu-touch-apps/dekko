/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

   This file is part of the dekko

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

import QtQuick 2.3
import Ubuntu.Components 1.1

Row {
    id: root
    property alias address: addressWidget.address
    property alias label: captionLabel.text
    property real contentHeight: Math.max(captionLabel.height, addressWidget.height + addressWidget.anchors.topMargin)
    property real maxWidth;
    height: contentHeight
    spacing: units.gu(1)
    clip: true
    Label {
        id: captionLabel
        horizontalAlignment: Text.AlignRight
    }
    AddressWidgets {
        id: addressWidget
        width: (maxWidth ? maxWidth : root.parent.width) - captionLabel.width - root.spacing
    }
}
