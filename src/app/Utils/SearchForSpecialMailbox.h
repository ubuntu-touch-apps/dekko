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

#ifndef SEARCHFORSPECIALMAILBOX_H
#define SEARCHFORSPECIALMAILBOX_H

#include <QObject>
#include <QAbstractItemModel>

#define TOP_MAILBOX_NAME_ALIAS "_topLevelMailbox_"

namespace Imap {
namespace Mailbox {
class Model;
}
}

namespace Dekko {
namespace Utils {

class SearchForSpecialMailbox : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *model READ model WRITE setModel)
    Q_PROPERTY(bool isRunning MEMBER m_isRunning NOTIFY isRunningChanged)
    Q_ENUMS(MailboxSpecialFlag)

public:

    typedef enum {
        MAILBOX_IS_SENT,
        MAILBOX_IS_TRASH,
        MAILBOX_IS_INBOX
    } MailboxSpecialFlag;

    explicit SearchForSpecialMailbox(QObject *parent = 0);

    Q_INVOKABLE void start();

    QAbstractItemModel *model();
    void setModel(QAbstractItemModel * model);

signals:
    void isRunningChanged();
    void complete();
    void specialMailboxFound(QString mailboxName, int mailboxSpecialRole);
    void inboxFound(QString mboxName, QModelIndex mboxIndex);

public slots:
    void handleRowsInserted(QModelIndex parent, int first, int last);

private:
    Imap::Mailbox::Model *m_model;
    bool m_isRunning;
    QStringList m_mailboxListWaitForLoad;

    void findSpecialMailboxHelper(QModelIndex mailbox);
    void startFromTop();
    void checkComplete();
};
}
}
#endif // SEARCHFORSPECIALMAILBOX_H
