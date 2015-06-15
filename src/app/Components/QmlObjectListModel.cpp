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
#include "QmlObjectListModel.h"

namespace Dekko
{
namespace Components
{

QmlObjectListModel::QmlObjectListModel(QMetaObject metaObj, QObject *parent) :
    QAbstractListModel(parent), m_count(0), m_metaObject(metaObj)
{
    m_roles.insert(0, QByteArrayLiteral("qtObject"));
    for (int idx = 0; idx < m_metaObject.propertyCount(); idx++) {
        int role = m_roles.count();
        QMetaProperty mp = m_metaObject.property(idx);
        m_roles.insert(role, mp.name());
        if (mp.hasNotifySignal()) {
            m_signalToIndexMap.insert(mp.notifySignalIndex(), role);
        }
    }
    m_methodHandler = metaObject()->method(metaObject()->indexOfMethod("onObjectPropertyChanged()"));
}

void QmlObjectListModel::dereferenceObject(QObject *object)
{
    if (object != NULL) {
        object->disconnect();
        if (object->parent() == this) {
            object->deleteLater();
        }
        if (!m_uidName.isEmpty()) {
            QString k = m_indexByUid.key(object, QStringLiteral(""));
            if (!k.isEmpty()) {
                m_indexByUid.remove(k);
            }
        }
        updateCount();
    }
}

void QmlObjectListModel::referenceObject(QObject *object)
{
    if (object != NULL) {
        if (object->parent() == NULL) {
            object->setParent(this);
        }
        Q_FOREACH(int sigIndex, m_signalToIndexMap.keys()) {
            QMetaMethod metaMethod = object->metaObject()->method(sigIndex);
            QObject::connect(object, metaMethod, this, m_methodHandler, Qt::UniqueConnection);
        }
        if (!m_uidName.isEmpty()) {
            QString key = m_indexByUid.key(object, QStringLiteral(""));
            if (!key.isEmpty()) {
                m_indexByUid.remove(key);
            }
            QString value = object->property(m_uidName).toString();
            if (!value.isEmpty()) {
                m_indexByUid.insert(value, object);
            }
        }
        updateCount();
    }
}

void QmlObjectListModel::updateCount()
{
    if (m_count != m_objects.count()) {
        m_count = m_objects.count();
        emit countChanged(m_count);
    }
}

int QmlObjectListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_objects.count();
}

QVariant QmlObjectListModel::data(const QModelIndex &index, int role) const
{
    QVariant var;
    if (index.isValid()) {
        QObject *obj = get(index.row());
        QByteArray roleName = m_roles.value(role, QByteArrayLiteral(""));
        if (obj != NULL && !roleName.isEmpty()) {
            var.setValue(role > 0 ? obj->property(roleName) : QVariant::fromValue(obj));
        }
    }
    return var;
}

bool QmlObjectListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool hasSet = false;
    if (index.isValid()) {
        QObject *obj = get(index.row());
        QByteArray roleName = m_roles.value(role, QByteArray(""));
        if (obj != NULL && role > 0 && !roleName.isEmpty()) {
            hasSet = obj->setProperty(roleName, value);
        }
    }
    return hasSet;
}

QHash<int, QByteArray> QmlObjectListModel::roleNames() const
{
    return m_roles;
}

void QmlObjectListModel::append(QObject *object)
{
    if (object != NULL) {
        int c = m_objects.count();
        beginInsertRows(QModelIndex(), c, c);
        m_objects.append(object);
        referenceObject(object);
        endInsertRows();
    }
}

void QmlObjectListModel::append(QObjectList objectList)
{
    // strip any NULL objects
    objectList.removeAll(NULL);
    if (!objectList.isEmpty()) {
        int c = m_objects.count();
        beginInsertRows(QModelIndex(), c, c + objectList.count() - 1);
        m_objects.append(objectList);
        Q_FOREACH(QObject *obj, objectList) {
            referenceObject(obj);
        }
        endInsertRows();
    }
}

void QmlObjectListModel::clear()
{
    if (m_objects.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_objects.count() - 1);
        Q_FOREACH(QObject *obj, m_objects) {
            dereferenceObject(obj);
        }
        m_objects.clear();
        endRemoveRows();
    }
}

bool QmlObjectListModel::contains(QObject *object) const
{
    return m_objects.contains(object);
}

int QmlObjectListModel::count() const
{
    return m_count;
}

QObject *QmlObjectListModel::first() const
{
    return m_objects.first();
}

QObject *QmlObjectListModel::get(int idx) const
{
    QObject *obj = NULL;
    if (idx >= 0 && idx < m_objects.size()) {
        obj = m_objects.value(idx);
    }
    return obj;
}

QObject *QmlObjectListModel::getByUid(QString &uid) const
{
    return m_indexByUid.value(uid, NULL);
}

int QmlObjectListModel::indexOf(QObject *object) const
{
    return m_objects.indexOf(object);
}

void QmlObjectListModel::insert(int &idx, QObject *object)
{
    if (object != NULL) {
        beginInsertRows(QModelIndex(), idx, idx);
        m_objects.insert(idx, object);
        referenceObject(object);
        endInsertRows();
    }
}

void QmlObjectListModel::insert(int &idx, QObjectList objectList)
{
    objectList.removeAll(NULL);
    if (!objectList.isEmpty()) {
        beginInsertRows(QModelIndex(), idx, idx + objectList.count() - 1);
        int offset = 0;
        Q_FOREACH(QObject *obj, objectList) {
            m_objects.insert(idx + offset, obj);
            referenceObject(obj);
            offset++;
        }
        endInsertRows();
    }
}

bool QmlObjectListModel::isEmpty() const
{
    return m_objects.isEmpty();
}

QObject *QmlObjectListModel::last() const
{
    return m_objects.last();
}

QObjectList QmlObjectListModel::list() const
{
    return m_objects;
}

void QmlObjectListModel::move(int &start, int &finish)
{
    if (start != finish) {
        const int min = qMin(start, finish);
        const int max = qMax(start, finish);
        beginMoveRows(QModelIndex(), max, max, QModelIndex(), min);
        m_objects.move(max, min);
        endMoveRows();
    }
}

void QmlObjectListModel::prepend(QObject *object)
{
    if (object != NULL) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_objects.prepend(object);
        referenceObject(object);
        endInsertRows();
    }
}

void QmlObjectListModel::prepend(QObjectList objectList)
{
    objectList.removeAll(NULL);
    if (!objectList.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, objectList.count() -1);
        int offset = 0;
        Q_FOREACH(QObject *obj, objectList) {
            m_objects.insert(offset, obj);
            referenceObject(obj);
            offset++;
        }
        endInsertRows();
    }
}

void QmlObjectListModel::remove(int &idx)
{
    if (idx > 0 && idx < m_objects.size()) {
        beginRemoveRows(QModelIndex(), idx, idx);
        QObject *obj = m_objects.takeAt(idx);
        dereferenceObject(obj);
        endRemoveRows();
    }
}

void QmlObjectListModel::remove(QObject *object)
{
    if (object != NULL) {
        int idx = m_objects.indexOf(object);
        remove(idx);
    }
}

int QmlObjectListModel::roleForName(QByteArray &name) const
{
    return m_roles.key(name, -1);
}

void QmlObjectListModel::onObjectPropertyChanged()
{
    QObject *obj = sender();
    // first find the role and row
    int role = m_signalToIndexMap.value(senderSignalIndex(), -1);
    int row = m_objects.indexOf(obj);
    // ok so if role is > -1 and is an actual row then we should emit dataChanged here
    if (role >= 0 && row >= 0) {
        QModelIndex idx = index(row, 0);
        emit dataChanged(idx, idx, QVector<int>(1, role));
    }
    if (!m_uidName.isEmpty()) {
        QByteArray rolename = m_roles.value(role, QByteArrayLiteral(""));
        if (!rolename.isEmpty() && rolename == m_uidName) {
            QString k = m_indexByUid.key(obj, QStringLiteral(""));
            if (!k.isEmpty()) {
                m_indexByUid.remove(k);
            }
            QString val = obj->property(m_uidName).toString();
            if (!val.isEmpty()) {
                m_indexByUid.insert(val, obj);
            }
        }
    }

}

}
}
