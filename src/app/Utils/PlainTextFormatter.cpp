/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
#include "PlainTextFormatter.h"
#include <QDebug>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QRegExp>
#include "Composer/PlainTextFormatter.h"

namespace Dekko {

namespace Utils {

PlainTextFormatter::PlainTextFormatter(QObject *parent) :
    QObject(parent), m_message(QString())
{
    setLoadStatus(LoadStatus::NONE);
    connect(this, SIGNAL(indexChanged()), this, SLOT(formatMessage()));
}

QModelIndex PlainTextFormatter::msgIndex() const
{
    return m_msgIndex;
}

void PlainTextFormatter::setMsgIndex(const QModelIndex &index)
{
    m_msgIndex = index;
    emit msgIndexChanged();
}

QModelIndex PlainTextFormatter::msgPartIndex() const
{
    return m_partIndex;
}

void PlainTextFormatter::setMsgPartIndex(const QModelIndex &index)
{
    if (index == m_partIndex || !index.isValid()) {
        return;
    }
    setLoadStatus(LoadStatus::LOADING);
    m_partIndex = index;
    m_rawTextPart = m_partIndex.data(Imap::Mailbox::RolePartUnicodeText).toString();
    if (m_rawTextPart.isEmpty()) {
        setLoadStatus(LoadStatus::LOADING_FROM_NETWORK);
        QNetworkRequest request;
        QUrl url;
        url.setScheme(QLatin1String("trojita-imap"));
        url.setHost(QLatin1String("msg"));
        url.setPath(m_partIndex.data(Imap::Mailbox::RolePartPathToPart).toString());
        request.setUrl(url);
        m_qnam = new Imap::Network::MsgPartNetAccessManager(this);
        m_qnam->setModelMessage(m_msgIndex);
        m_reply = m_qnam->get(request);
        connect(m_reply, SIGNAL(finished()), this, SLOT(qnamFinished()));
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleRequestError()));
    } else {
        setLoadStatus(LoadStatus::DONE);
        emit indexChanged();
    }

}

QString PlainTextFormatter::message() const
{
    return m_message;
}

QString PlainTextFormatter::rawMessage() const
{
    return m_rawTextPart;
}

PlainTextFormatter::LoadStatus PlainTextFormatter::loadStatus() const
{
    return m_loadStatus;
}

PlainTextFormatter::FormatType PlainTextFormatter::format() const
{
    return m_format;
}

void PlainTextFormatter::setFormat(const PlainTextFormatter::FormatType &format)
{
    m_format = format;
    emit formatChanged();
}

void PlainTextFormatter::qnamFinished()
{
    m_rawTextPart = m_partIndex.data(Imap::Mailbox::RolePartUnicodeText).toString();
    setLoadStatus(LoadStatus::DONE);
    emit indexChanged();
}

void PlainTextFormatter::handleRequestError()
{
    if (m_reply->error() != QNetworkReply::NoError) {
        setLoadStatus(LoadStatus::ERROR);
        qDebug() << "[PlaintextFormatter] Failed requesting message part: " << m_reply->error();
        emit error(QString("Something went wrong while requesting this message part"));
    }
}

void PlainTextFormatter::formatMessage()
{
    switch(m_format) {
    case FormatType::PLAIN:
        m_message = m_rawTextPart;
        break;
    // At the moment this is rather dumb and will only return linkified text
    // for all cases that aren't PLAIN.
    case FormatType::LINKIFIED:
    case FormatType::PRETTIFIED:
    case FormatType::HTML:
        m_message = formatRawText(m_rawTextPart);
        break;
    }

    emit messageChanged();
}

void PlainTextFormatter::setLoadStatus(const PlainTextFormatter::LoadStatus &status)
{
    m_loadStatus = status;
    emit loadingStatusChanged(m_loadStatus);
}

QString PlainTextFormatter::formatRawText(const QString &rawMsg)
{
    QString m = Composer::Util::plainTextToHtml(rawMsg, Composer::Util::FORMAT_PLAIN);
    return m.replace(QLatin1Char('\n'), QLatin1String("<br />"));
}

}
}
