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

#include "ContactFilterModel.h"
#include "ContactsModel.h"
#include <QDebug>

namespace Dekko {
namespace Models {

ContactFilterModel::ContactFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

void ContactFilterModel::sortByTag()
{
    sort(0);
}

bool ContactFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    ContactsModel *model = qobject_cast<ContactsModel *>(sourceModel());
    QString firstName = model->data(model->index(source_row), ContactsModel::RoleFirstName).toString().toLower();
    if (filterRegExp().indexIn(firstName) >= 0) {
        return true;
    }
    QString lastName = model->data(model->index(source_row), ContactsModel::RoleLastName).toString().toLower();
    if (filterRegExp().indexIn(lastName) >= 0) {
        return true;
    }
    QList<QString> emails = model->data(model->index(source_row), ContactsModel::RoleEmails).value<QList<QString> >();
    for (int i = 0; i < emails.length(); i++) {
        if (filterRegExp().indexIn(emails[i].toLower()) >= 0) {
            return true;
        }
    }
    return false;
}

bool ContactFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    ContactsModel *model = qobject_cast<ContactsModel *>(sourceModel());
    QString leftTag = model->data(left, ContactsModel::RoleTag).toString();
    QString rightTag = model->data(right, ContactsModel::RoleTag).toString();
    return leftTag < rightTag;
}

}
}
