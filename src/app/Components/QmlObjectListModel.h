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
#ifndef QMLOBJECTLISTMODEL_H
#define QMLOBJECTLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QByteArray>
#include <QChar>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaProperty>
#include <QString>
#include <QVariant>
#include <QVector>


namespace Dekko
{
namespace Components
{
class QmlObjectListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    virtual ~QmlObjectListModel() {
        clear();
    }
    template <class T> static QmlObjectListModel *create(QObject *parent = 0) {
        return new QmlObjectListModel(T::staticMetaObject, parent);
    }
    template <class T> T *getAs(int idx) const {
        return qobject_cast<T *>(get(idx));
    }
    template <class T> QList<T *> listAs () const {
        QList<T *> returnList;
        for (int idx = 0; idx < count(); idx++) {
            returnList.append(qobject_cast<T *>(get(idx)));
        }
        return returnList;
    }
    // Re-Implemented from QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual QHash<int, QByteArray> roleNames() const;

signals:
    void countChanged(int count);

public slots:
    // Here we define the slots that expose a nice QList type API
    void append(QObject *object);
    void append(QObjectList objectList);
    void clear();
    bool contains(QObject *object) const;
    int count() const;
    QObject *first() const;
    QObject *get(int idx) const;
    QObject *getByUid(QString &uid) const;
    int indexOf(QObject *object) const;
    void insert(int &idx, QObject *object);
    void insert(int &idx, QObjectList objectList);
    bool isEmpty() const;
    QObject *last() const;
    QObjectList list() const;
    void move(int &start, int &finish);
    void prepend(QObject *object);
    void prepend(QObjectList objectList);
    void remove(int &idx);
    void remove(QObject *object);
    int roleForName(QByteArray &name) const;

private slots:
    void onObjectPropertyChanged();

protected:
    explicit QmlObjectListModel(QMetaObject metaObj, QObject * parent);

    void dereferenceObject(QObject *object);
    void referenceObject(QObject *object);
    void updateCount();

private:
    int m_count;
    QMetaObject m_metaObject;
    QMetaMethod m_methodHandler;
    QObjectList m_objects; // Items in our list
    QByteArray m_uidName;
    QHash<int, int> m_signalToIndexMap;
    QHash<int, QByteArray> m_roles;
    QHash<QString, QObject *> m_indexByUid;

};
}
}

#endif // QMLOBJECTLISTMODEL_H
