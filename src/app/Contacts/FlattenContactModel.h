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

#ifndef FLATTENCONTACTMODEL_H
#define FLATTENCONTACTMODEL_H

#include <QAbstractListModel>

namespace Dekko {
namespace Models {

struct ContactWithName
{
    QString name;
    QString email;
};
/*
 * FlattenContactModel display one contact with multiple email
 * by multiple contact of same name with one email
 *
 * Call update to update manually
 * Source model should be of type Dekko::Models::ContactsModel
 */
class FlattenContactModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel * model READ model WRITE setModel NOTIFY modelChanged)
    enum {
    	RoleName = Qt::UserRole + 1,
    	RoleEmail
    };
public:

    explicit FlattenContactModel(QObject *parent = 0);
    ~FlattenContactModel();

    Q_INVOKABLE void update();

    QAbstractItemModel* model();
    void setModel(QAbstractItemModel* model);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    ContactWithName *get(int idx);
signals:

    void modelChanged();

public slots:

private:
    QList<ContactWithName *> m_contactsList;
    QAbstractItemModel *m_model;
};
}
}

#endif // FLATTENCONTACTMODEL_H
