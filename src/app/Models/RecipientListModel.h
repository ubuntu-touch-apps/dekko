/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko "launchpad.net/dekko"

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

#ifndef RECIPIENTLISTMODEL_H
#define RECIPIENTLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QModelIndex>
#include <QtQml/QQmlEngine>
#include "Composer/Recipients.h"
#include "Recipient.h"
#include "app/Composition/ReplyMode.h"

namespace Dekko
{
namespace Models
{

class RecipientListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit RecipientListModel(QObject *parent = 0);
    ~RecipientListModel();

    typedef QList<Recipient *> RecipientsList;

    enum Roles {
        RecipientAddressRole = Qt::UserRole + 1,
        RecipientKindRole,
        RecipientIndexRole // reture the index of recipient in m_recipients, used in RecipientListFilterModel
    };

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int count() const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QModelIndex indexFromItem(Recipient *recipient) const;

    Q_INVOKABLE bool removeRow(int row, const QModelIndex &index = QModelIndex());
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &index = QModelIndex());
    // return a QVariant similar to how the QML ListModel does
    Q_INVOKABLE QVariant get(int index);
    // returns a formatted recipient list that will be accepted by the MessageComposer
    Q_INVOKABLE Composer::RecipientList toRecipientList() const;

    Q_INVOKABLE bool buildRecipientList(const int mode, const QModelIndex &message);

signals:
    void countChanged();

public slots:
    void appendRow(Recipient *recipient);
    void appendRows(QList<Recipient *> &recipients);
    void insertRow(int row, Recipient *recipient);
    void clear();
    void setIncludes(const QStringList &includes, const bool isCc); // if not cc then it can only be Bcc

private slots:
    void updateRecipient();

private:
    // keep a list of recipients
    RecipientsList m_recipients;

    Composer::RecipientList getRecipientsFromMessage(const QModelIndex &message);
    Composer::RecipientList deduplicatedAndJustToCcBcc(Composer::RecipientList input);
    Composer::RecipientList mapRecipients(Composer::RecipientList input, const QMap<Composer::RecipientKind, Composer::RecipientKind>& mapping);
    bool buildRecipientList(const int mode, const Composer::RecipientList &originalRecipients,
                                            const QList<QUrl> &headerListPost, const bool headerListPostNo);
    bool prepareReplyAll(const Composer::RecipientList &recipients, Composer::RecipientList &output);
    bool prepareReply(const Composer::RecipientList &recipients, const QList<QUrl> headerListPost, Composer::RecipientList &output);
    bool prepareReplyList(const QList<QUrl> &headerListPost, const bool headerListPostNo, Composer::RecipientList &output);
    bool appendRecipientsFromList(const Composer::RecipientList &recipients);

    QStringList m_ccIncludes;
    QStringList m_bccIncludes;

};
}
}
#endif // RECIPIENTLISTMODEL_H
