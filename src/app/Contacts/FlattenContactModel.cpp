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

#include "FlattenContactModel.h"
#include <QDebug>
#include "ContactFilterModel.h"
#include "ContactsModel.h"
#include <QVariantList>

namespace Dekko {
namespace Models {

FlattenContactModel::FlattenContactModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

FlattenContactModel::~FlattenContactModel() {
    for (int i = 0; i < m_contactsList.length(); i++) {
        delete m_contactsList[i];
    }
    m_contactsList.clear();
}

void FlattenContactModel::update()
{
    ContactFilterModel *contactsModel = qobject_cast<ContactFilterModel *>(m_model);
    emit beginRemoveRows(QModelIndex(), 0, m_contactsList.length() - 1);
    for (int i = 0; i < m_contactsList.length(); i++) {
        delete m_contactsList[i];
    }
    m_contactsList.clear();
    emit endRemoveRows();
    int totalEmailCount = 0;
    for (int i = 0; i < contactsModel->rowCount(); i++) {
        QString name = contactsModel->data(contactsModel->index(i, 0), ContactsModel::RoleFirstName).toString() + " " +
                contactsModel->data(contactsModel->index(i, 0), ContactsModel::RoleLastName).toString();
        QVariantList emails = contactsModel->data(contactsModel->index(i, 0), ContactsModel::RoleEmails).value<QVariantList>();
        for (int j = 0; j < emails.length(); j++) {
            QString email = emails[j].toString();
            ContactWithName *contact = new ContactWithName();
            contact->name = name;
            contact->email = email;
            emit beginInsertRows(QModelIndex(), totalEmailCount, totalEmailCount);
            m_contactsList.append(contact);
            emit endInsertRows();
            totalEmailCount ++;
        }
    }
}

QAbstractItemModel *FlattenContactModel::model()
{
    return m_model;
}

void FlattenContactModel::setModel(QAbstractItemModel *model)
{
    m_model = model;
    emit modelChanged();
}

int FlattenContactModel::rowCount(const QModelIndex & parent) const {
    return m_contactsList.length();
}

QVariant FlattenContactModel::data(const QModelIndex & index, int role) const {
    if (index.row() >= m_contactsList.length())
        return QVariant();		
    switch(role) {
    case RoleName:
        return m_contactsList[index.row()]->name;
    case RoleEmail:
        return m_contactsList[index.row()]->email;
    }
    return QVariant();
}

QHash<int, QByteArray> FlattenContactModel::roleNames() const {
    static QHash<int, QByteArray> roleNames;
    if (roleNames.isEmpty()) {
        roleNames[RoleName] = "name";
        roleNames[RoleEmail] = "email";
    }
    return roleNames;
}


ContactWithName *FlattenContactModel::get(int idx) {
    return m_contactsList[idx];
}

}
}
