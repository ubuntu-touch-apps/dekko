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

#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "Imap/Model/OneMessageModel.h"
#include "Imap/Model/Model.h"

namespace Dekko {
namespace Models {

class MessageModel : public Imap::Mailbox::OneMessageModel {
    Q_OBJECT
    Q_PROPERTY(QObject * imapModel READ imapModel WRITE setImapModel)
    Q_PROPERTY(int uid READ uid WRITE setUid)
    Q_PROPERTY(QString mailbox READ mailbox WRITE setMailbox)
    Q_PROPERTY(QModelIndex mainTextPartIndex READ mainTextPartIndex)
public:
    explicit MessageModel(QObject * parent = 0);

    Q_INVOKABLE void openMessage();
    Q_INVOKABLE void markMessageDeleted(QModelIndex messageIndex, bool marked);
    Q_INVOKABLE void setMessageModelIndex(QModelIndex index);
    Q_INVOKABLE QModelIndex getModelIndexFromPartId(QString partId);

    QObject * imapModel();
    void setImapModel(QObject * imapModel);
    int uid();
    void setUid(int uid);
    QString mailbox();
    void setMailbox(QString mailbox);
    QModelIndex mainTextPartIndex();
private:
    Imap::Mailbox::Model *m_imapModel;
    int m_uid;
    QString m_mailbox;
};

}
}

#endif
