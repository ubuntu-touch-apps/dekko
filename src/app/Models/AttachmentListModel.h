/* Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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

#ifndef DEKKO_MODEL_ATTACHMENTLISTMODEL
#define DEKKO_MODEL_ATTACHMENTLISTMODEL

#include <QAbstractListModel>

namespace Composer {
class AttachmentItem;
}

namespace Dekko {

namespace Models {

class AttachmentListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles {
        AttachmentCaptionRole = Qt::UserRole + 1,
        HumanReadableSizeRole,
        MimeTypeRole,
        MimeTypeIconName,
    };

    AttachmentListModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE void addFileAttachment(const QString &path);
    Q_INVOKABLE void removeAttachmentAtIndex(int idx);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void addImapMessageAttachment(QModelIndex messageIndex, QAbstractItemModel *model);
    Q_INVOKABLE void addImapPartAttachment(QModelIndex messageIndex, QAbstractItemModel *model);
    Composer::AttachmentItem *getAttachmentAtIndex(int index);
signals:
    void countChanged();
private:
    QList<Composer::AttachmentItem *> m_attachmentList;
};

}
}

#endif // DEKKO_MODEL_ATTACHMENTLISTMODEL
