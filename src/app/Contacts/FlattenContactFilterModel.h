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

#ifndef FLATTENCONTACTFILTERMODEL_H
#define FLATTENCONTACTFILTERMODEL_H

#include <QSortFilterProxyModel>

namespace Dekko {
namespace Models {

class FlattenContactFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel * source WRITE setSourceModel) // source should be of class Dekko::Models::FlattenContactModel
public:
    explicit FlattenContactFilterModel(QObject *parent = 0);

    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
};
	
}
}

#endif