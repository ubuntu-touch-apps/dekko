/* Copyright (C) 2006 - 2014 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

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

#ifndef IMAP_PARSER_THREADINGNODE
#define IMAP_PARSER_THREADINGNODE

#include <QDataStream>
#include <QMetaType>
#include <QVector>

namespace Imap
{
namespace Responses
{

/** @short Structure for keeping track of the message hierarchy, or threading */
struct ThreadingNode {
    /** @short Message sequence number or UID number

    Which of the two allowed numbering schemes would be used depends on
    the command which triggered this reply, if it was plain THREAD, it
    will use message sequence numbers, if it was an UID THREAD command,
    it uses UIDs.

    Special value 0 means "parent is present and its existence can be
    proved, but it doesn't match the search criteria or it isn't in the
    mailbox.
    */
    uint num;
    /** @short Recursive data structure storing numbers of all messages which are children of the current one */
    QVector<ThreadingNode> children;
    ThreadingNode(const uint num=0, const QVector<ThreadingNode> &children=QVector<ThreadingNode>()):
        num(num), children(children)
    {
    }
};

bool operator==(const ThreadingNode &n1, const ThreadingNode &n2);
bool operator!=(const ThreadingNode &n1, const ThreadingNode &n2);

QDataStream &operator>>(QDataStream &s, ThreadingNode &n);
QDataStream &operator<<(QDataStream &s, const ThreadingNode &n);

}
}

Q_DECLARE_METATYPE(Imap::Responses::ThreadingNode);


#endif // IMAP_PARSER_THREADINGNODE
