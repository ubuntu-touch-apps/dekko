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

#include "MsgPartNetAccessManagerFactory.h"

namespace Dekko {

void MsgPartNetAccessManagerFactory::setMessageIndex(const QModelIndex &messageIndex)
{
    m_messageIndex = messageIndex;
    for (int i = 0; i < m_mpnamList.length(); i++) {
        m_mpnamList[i]->setModelMessage(messageIndex);
    }
}

QNetworkAccessManager *MsgPartNetAccessManagerFactory::create(QObject *parent)
{
    Imap::Network::MsgPartNetAccessManager * mpnam = new Imap::Network::MsgPartNetAccessManager(parent);
    mpnam->setModelMessage(m_messageIndex);
    connect(mpnam, SIGNAL(destroyed(QObject*)), this, SLOT(handleManagerDestroyed(QObject *)));
    m_mpnamList.append(mpnam);
    return mpnam;
}

void MsgPartNetAccessManagerFactory::handleManagerDestroyed(QObject *destroyedObject)
{
    m_mpnamList.removeOne(qobject_cast<Imap::Network::MsgPartNetAccessManager *>(destroyedObject));
}

}
