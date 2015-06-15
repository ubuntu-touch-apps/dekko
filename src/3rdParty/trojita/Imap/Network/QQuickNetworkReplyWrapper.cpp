/* Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

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
#include "QQuickNetworkReplyWrapper.h"
#include <QDebug>

namespace Imap {

namespace Network {

QQuickNetworkReplyWrapper::QQuickNetworkReplyWrapper(QObject *qQuickReply, QNetworkReply *qNetworkReply)
    : m_quickReply(qQuickReply), m_qNetworkReply(qNetworkReply)
{
    Q_ASSERT(qQuickReply);
    Q_ASSERT(qNetworkReply);
    Q_ASSERT(qQuickReply->metaObject()->className() == QByteArray("QQuickNetworkReply"));
    connect(qNetworkReply, SIGNAL(finished()), this, SLOT(dataRecieved()));
}

void QQuickNetworkReplyWrapper::dataRecieved()
{
    QByteArray data = m_qNetworkReply->readAll();
    m_quickReply->setProperty("data", QVariant(data));
    m_quickReply->setProperty("contentType", m_qNetworkReply->header(QNetworkRequest::ContentTypeHeader));
    bool ok = QMetaObject::invokeMethod(m_quickReply, "send");
    Q_ASSERT(ok); Q_UNUSED(ok);
    m_qNetworkReply->deleteLater();
    this->deleteLater();
}

}
}
