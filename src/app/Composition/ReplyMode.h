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

#ifndef REPLYMODE_H
#define REPLYMODE_H
#include <QObject>


namespace Dekko
{

class ReplyMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Mode replyMode READ replyMode WRITE setReplyMode NOTIFY replyModeChanged)
    Q_ENUMS(Mode)

public:
    explicit ReplyMode(QObject *parent = 0);
    enum Mode{
        REPLY_PRIVATE, /**< @short Reply to "sender(s)" only */
        REPLY_ALL, /**< @short Reply to all recipients */
        REPLY_ALL_BUT_ME, /**< @short Reply to all recipients excluding any of my own identities */
        REPLY_LIST, /**< @short Reply to the mailing list */
        REPLY_NONE, /**< @short This is not a reply but a simple compose */
        REPLY_FORWARD
    };

    Mode replyMode() const;
    void setReplyMode(const Mode &replyMode);
signals:
    void replyModeChanged();
private:
    Mode m_replyMode;
};
}
#endif // REPLYMODE_H
