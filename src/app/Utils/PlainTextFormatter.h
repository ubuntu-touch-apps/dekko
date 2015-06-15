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
#ifndef PLAINTEXTFORMATTER_H
#define PLAINTEXTFORMATTER_H

#include <QObject>
#include <QModelIndex>
#include <QNetworkReply>
#include "Imap/Model/ItemRoles.h"
#include "Imap/Network/MsgPartNetAccessManager.h"

namespace Dekko {

namespace Utils {

class PlainTextFormatter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QModelIndex msgPartIndex READ msgPartIndex WRITE setMsgPartIndex NOTIFY indexChanged)
    Q_PROPERTY(QModelIndex msgIndex READ msgIndex WRITE setMsgIndex NOTIFY msgIndexChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QString rawMessage READ rawMessage NOTIFY indexChanged)
    Q_PROPERTY(LoadStatus loadStatus READ loadStatus NOTIFY loadingStatusChanged)
    Q_PROPERTY(FormatType format READ format WRITE setFormat NOTIFY formatChanged)
    Q_ENUMS(LoadStatus)
    Q_ENUMS(FormatType)

public:
    explicit PlainTextFormatter(QObject *parent = 0);

    enum LoadStatus {
        NONE,
        LOADING,
        LOADING_FROM_NETWORK,
        DONE,
        ERROR
    };

    enum FormatType {
        PLAIN, // untouched raw text
        LINKIFIED, // Just hyper & mailto; links
        PRETTIFIED, // Rich text formatting i.e bold, italics & underlines (includes linkify)
        HTML // full blown collapsible web view style
    };
    QModelIndex msgIndex() const;
    void setMsgIndex(const QModelIndex &index);
    QModelIndex msgPartIndex() const;
    void setMsgPartIndex(const QModelIndex &index);

    QString message() const;
    QString rawMessage() const;

    LoadStatus loadStatus() const;
    FormatType format() const;
    void setFormat(const FormatType &format);

signals:
    void indexChanged();
    void error(const QString &msg);
    void messageChanged();
    void qnamChanged();
    void msgIndexChanged();
    void loadingStatusChanged(LoadStatus &status);
    void formatChanged();


public slots:
    void qnamFinished();
    void handleRequestError();
    void formatMessage();

protected:
    void setLoadStatus(const LoadStatus &status);

private:
    Imap::Network::MsgPartNetAccessManager *m_qnam;
    QModelIndex m_partIndex;
    QModelIndex m_msgIndex;
    QString m_rawTextPart;
    QString m_message;
    LoadStatus m_loadStatus;
    FormatType m_format;
    QNetworkReply *m_reply;
    QString formatRawText(const QString &rawMsg);

};
}
}
#endif // PLAINTEXTFORMATTER_H
