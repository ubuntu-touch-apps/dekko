/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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

#include "RecipientListFilterModel.h"
#include "../Models/RecipientListModel.h"

namespace Dekko {
namespace Models {

bool RecipientListFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (filterRole() == FILTER_TO && index.data(RecipientListModel::RecipientKindRole) == Recipient::ADDRESS_TO) {
        return true;
    } else if (filterRole() == FILTER_CC && index.data(RecipientListModel::RecipientKindRole) == Recipient::ADDRESS_CC) {
        return true;
    } else if (filterRole() == FILTER_BCC && index.data(RecipientListModel::RecipientKindRole) == Recipient::ADDRESS_BCC) {
        return true;
    } else {
        return false;
    }
}

}
}
