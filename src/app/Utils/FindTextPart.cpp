/* Some functions copied from FindInterestingPart in Trojita

   Certain enhancements (www.xtuple.com/trojita-enhancements)
   are copyright © 2010 by OpenMFG LLC, dba xTuple.  All rights reserved.

   Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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

#include "FindTextPart.h"
#include <QTimer>
#include <QDebug>
#include "3rdParty/trojita/Imap/Model/ItemRoles.h"

namespace Dekko {
namespace Utils {

Imap::Mailbox::FindInterestingPart::MainPartReturnCode FindTextPart::findTextPartOfMessage(
        const QModelIndex &message, QModelIndex &mainPartIndex, QString &partMessage, QString *partData)
{
    mainPartIndex = message.child( 0, 0 );
    if ( ! mainPartIndex.isValid() ) {
        return Imap::Mailbox::FindInterestingPart::MAINPART_MESSAGE_NOT_LOADED;
    }

    partMessage = FindTextPart::findTextPart( mainPartIndex );
    if ( ! mainPartIndex.isValid() ) {
        return Imap::Mailbox::FindInterestingPart::MAINPART_PART_CANNOT_DETERMINE;
    }

    if (partData) {
        QVariant data = mainPartIndex.data( Imap::Mailbox::RolePartData );
        if ( ! data.isValid() ) {
            return Imap::Mailbox::FindInterestingPart::MAINPART_PART_LOADING;
        }

        *partData = data.toString();
        return Imap::Mailbox::FindInterestingPart::MAINPART_FOUND;
    } else {
        return mainPartIndex.data(Imap::Mailbox::RoleIsFetched).toBool() ?
                    Imap::Mailbox::FindInterestingPart::MAINPART_FOUND :
                    Imap::Mailbox::FindInterestingPart::MAINPART_PART_LOADING;
    }
}

QString FindTextPart::findTextPart( QModelIndex &part )
{
    if ( ! part.isValid() )
        return QLatin1String("Invalid index");

    QString mimeType = part.data( Imap::Mailbox::RolePartMimeType ).toString().toLower();

    if ( mimeType == QLatin1String("text/plain") ) {
        // found it, no reason to do anything else
        return QString();
    }

    //Omit html text
//    if ( mimeType == QLatin1String("text/html") ) {
//        // HTML without a text/plain counterpart is not supported
////        part = QModelIndex();
//        return QString();
//    }

    if ( mimeType == QLatin1String("message/rfc822") ) {
        if ( part.model()->rowCount( part ) != 1 ) {
            part = QModelIndex();
            return QLatin1String("Unsupported message/rfc822 formatting");
        }
        part = part.child( 0, 0 );
        return FindTextPart::findTextPart( part );
    }

    if ( mimeType.startsWith( QLatin1String("multipart/") ) ) {
        QModelIndex target;
        QString str;
        for ( int i = 0; i < part.model()->rowCount( part ); ++i ) {
            // Walk through all children, try to find a first usable item
            target = part.child( i, 0 );
            str = FindTextPart::findTextPart( target );
            if ( target.isValid() ) {
                // Found a usable item
                part = target;
                return QString();
            }

        }
        part = QModelIndex();
        return QString::fromUtf8("This is a %1 formatted message whose parts are not suitable for diplaying here").arg(mimeType);
    }

    part = QModelIndex();
    return QString::fromUtf8("MIME type %1 is not supported").arg(mimeType);
}

}
}
