/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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

#include <QObject>

class QAbstractItemModel;
namespace Dekko {
namespace Utils {

class UidIterator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel * messageListModel READ messageListModel WRITE setMessageListModel)
    Q_PROPERTY(int uid READ uid WRITE setUid NOTIFY uidsChanged)
    Q_PROPERTY(int nextUid MEMBER m_nextUid NOTIFY uidsChanged)
    Q_PROPERTY(int prevUid MEMBER m_prevUid NOTIFY uidsChanged)
public:
    UidIterator(QObject * parent = 0);

    int uid();
    void setUid(int uid);
    QAbstractItemModel *messageListModel();
    void setMessageListModel(QAbstractItemModel * model);

    Q_INVOKABLE int goNext();
    Q_INVOKABLE int goPrev();

signals:
    void uidsChanged();

private:

    void findNextPrev();

    QAbstractItemModel *m_messageListModel;
    int m_uid;
    int m_nextUid;
    int m_prevUid;
    int m_currentPosition;
};

}
}
