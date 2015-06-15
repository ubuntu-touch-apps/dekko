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
#include "MailboxFilterModel.h"
#include "Imap/Model/ItemRoles.h"
#include <QDebug>
namespace Dekko {
namespace Models {

MailboxFilterModel::MailboxFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true); // This is pretty much what you always want for qml
}


bool MailboxFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (filterRole() == FILTER_QUERY) {
        if (m_queryString.isEmpty()) {
            return true;
        }
        QString mbox = index.data(Imap::Mailbox::RoleMailboxName).toString().toLower();
        return mbox.contains(m_queryString.toLower().trimmed());
    }
    if (filterRole() == FILTER_INBOX) {
        return index.data(Imap::Mailbox::RoleMailboxIsINBOX).toBool();
    } else if (filterRole() == FILTER_SELECTABLE) {
        return index.data(Imap::Mailbox::RoleMailboxIsSelectable).toBool();
    } else if (filterRole() == FILTER_NONE) {
        // We set this in qml just for clarity of reading so just return true
        return true;
    }
    return true;
}
}
}
