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

#include "ReplyMode.h"
namespace Dekko
{
ReplyMode::ReplyMode(QObject *parent) :
    QObject(parent), m_replyMode(ReplyMode::REPLY_NONE)
{
}

ReplyMode::Mode ReplyMode::replyMode() const
{
    return m_replyMode;
}

void ReplyMode::setReplyMode(const ReplyMode::Mode &replyMode)
{
    if (replyMode == m_replyMode) {
        return;
    }
    m_replyMode = replyMode;
    replyModeChanged();
}
}
