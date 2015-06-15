/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>
   Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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

#include "MessageListFilterModel.h"
#include "3rdParty/trojita/Imap/Model/ItemRoles.h"

namespace Dekko {
namespace Models {

MessageListFilterModel::MessageListFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent), m_filterDeleted(false)
{
    connect(this, &QSortFilterProxyModel::rowsInserted, [this]() {emit rowsChanged();});
    connect(this, &QSortFilterProxyModel::rowsRemoved, [this]() {emit rowsChanged();});
}

bool MessageListFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (filterRole() == FILTER_DELETED) {
        return index.data(Imap::Mailbox::RoleMessageIsMarkedDeleted).toBool();
    } else if (filterRole() == FILTER_TAGGED) {
        // FIXME: For now return false as we don't have a tags backend
        return false;
    }
    if (m_filterDeleted && index.data(Imap::Mailbox::RoleMessageIsMarkedDeleted).toBool()) {
        return false;
    }
    if (filterRole() == FILTER_UNREAD)
        return !index.data(Imap::Mailbox::RoleMessageIsMarkedRead).toBool();
    else if (filterRole() == FILTER_FLAGGED)
        return index.data(Imap::Mailbox::RoleMessageIsMarkedFlagged).toBool();
    return true;
}

}
}
