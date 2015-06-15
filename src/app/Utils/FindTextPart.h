/* Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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

#ifndef FIND_TEXT_PART_H
#define FIND_TEXT_PART_H

#include <QObject>
#include "3rdParty/trojita/Imap/Model/FindInterestingPart.h"
 
namespace Dekko {

namespace Utils {

class FindTextPart 
{
public:
    static Imap::Mailbox::FindInterestingPart::MainPartReturnCode findTextPartOfMessage(
        const QModelIndex &message, QModelIndex &mainPartIndex, QString &partMessage, QString *partData);
    static QString findTextPart( QModelIndex &part );
};

}
}

#endif // FIND_TEXT_PART_H
