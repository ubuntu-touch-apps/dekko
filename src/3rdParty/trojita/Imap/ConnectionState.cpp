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

#include <QObject>
#include "ConnectionState.h"

namespace Imap
{

QString connectionStateToString(const ConnectionState state)
{
    switch (state) {
    case CONN_STATE_NONE:
        return QString();
    case CONN_STATE_HOST_LOOKUP:
        return QString("Resolving hostname...");
    case CONN_STATE_CONNECTING:
        return QString("Connecting to the IMAP server...");
    case CONN_STATE_SSL_HANDSHAKE:
        return QString("Starting encryption (SSL)...");
    case CONN_STATE_SSL_VERIFYING:
        return QString("Checking certificates (SSL)...");
    case CONN_STATE_CONNECTED_PRETLS_PRECAPS:
        return QString("Checking capabilities...");
    case CONN_STATE_CONNECTED_PRETLS:
        return QString("Waiting for encryption...");
    case CONN_STATE_STARTTLS_ISSUED:
        return QString("Asking for encryption...");
    case CONN_STATE_STARTTLS_HANDSHAKE:
        return QString("Starting encryption (STARTTLS)...");
    case CONN_STATE_STARTTLS_VERIFYING:
        return QString("Checking certificates (STARTTLS)...");
    case CONN_STATE_ESTABLISHED_PRECAPS:
        return QString("Checking capabilities (after STARTTLS)...");
    case CONN_STATE_LOGIN:
        return QString("Logging in...");
    case CONN_STATE_POSTAUTH_PRECAPS:
        return QString("Checking capabilities (after login)...");
    case CONN_STATE_COMPRESS_DEFLATE:
        return QString("Activating compression...");
    case CONN_STATE_AUTHENTICATED:
        return QString("Logged in.");
    case CONN_STATE_SELECTING_WAIT_FOR_CLOSE:
        return QString("Waiting for another mailbox...");
    case CONN_STATE_SELECTING:
        return QString("Opening mailbox...");
    case CONN_STATE_SYNCING:
        return QString("Synchronizing mailbox...");
    case CONN_STATE_SELECTED:
        return QString("Mailbox opened.");
    case CONN_STATE_FETCHING_PART:
        return QString("Downloading message...");
    case CONN_STATE_FETCHING_MSG_METADATA:
        return QString("Downloading message structure...");
    case CONN_STATE_LOGOUT:
        return QString("Logged out.");
    }
    Q_ASSERT(false);
    return QString();
}

}
