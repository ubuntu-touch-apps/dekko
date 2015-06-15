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
#ifndef SENDERIDENTITYMODEL_H
#define SENDERIDENTITYMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QModelIndex>
#include <QSettings>
#include "SenderIdentity.h"

namespace Dekko
{
namespace Accounts
{
/*!
 \brief

 \class SenderIdentityModel SenderIdentityModel.h "src/app/Accounts/SenderIdentityModel.h"
*/
class SenderIdentityModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
public:
    /*!
     \brief

     \fn SenderIdentityModel
     \param parent
    */
    explicit SenderIdentityModel(QObject *parent = 0);

    /*!
     \brief

     \typedef IdentitiesList    */
    typedef QList<SenderIdentity *> IdentitiesList;

    /*!
     \brief

     \enum Roles
    */
    enum Roles {
        IdentityNameRole = Qt::UserRole + 1,
        IdentityEmailRole,
        IdentityOrganizationRole,
        IdentitySignatureRole,
        IdentityUseDefaultSignature,
        IdentityRecipientStringRole
    };
    QString accountId() const;
    void setAccountId(const QString &accountId);

    /*!
     \brief

     \fn roleNames
     \return QHash<int, QByteArray>
    */
    QHash<int, QByteArray> roleNames() const override;
    /*!
     \brief

     \fn data
     \param index
     \param role
     \return QVariant
    */
    QVariant data(const QModelIndex &index, int role) const;
    /*!
     \brief

     \fn rowCount
     \param parent
     \return int
    */
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    /*!
     \brief

     \fn indexFromItem
     \param identity
     \return QModelIndex
    */
    QModelIndex indexFromItem(SenderIdentity *identity) const;
    /*!
     \brief

     \fn removeRow
     \param row
     \param index
     \return bool
    */
    Q_INVOKABLE bool removeRow(int row, const QModelIndex &index = QModelIndex());
    /*!
     \brief

     \fn removeRows
     \param row
     \param count
     \param index
     \return bool
    */
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &index = QModelIndex());
    /*!
     \brief

     \fn get
     \param index
     \return QVariant
    */
    Q_INVOKABLE QVariant get(int index);

    Q_INVOKABLE QObject *getIdentity(int index);
    /*!
     \brief

     \fn setData
     \param index
     \param value
     \param role
     \return bool
    */
    bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:
    /*!
     \brief

     \fn countChanged
    */
    void countChanged();

    void accountIdChanged();

public slots:
    /*!
     \brief

     \fn appendRow
     \param identity
    */
    void appendRow(SenderIdentity *identity);
    /*!
     \brief

     \fn appendRows
     \param identities
    */
    void appendRows(QList<SenderIdentity *> &identities);
    /*!
     \brief

     \fn insertRow
     \param row
     \param identity
    */
    void insertRow(int row, SenderIdentity *identity);
    /*!
     \brief

     \fn clear
    */
    void clear();

    void reloadIdentities();

    void saveIdentites();

private slots:
    /*!
     \brief

     \fn updateIdentity
    */
    void updateIdentity();

private:
    IdentitiesList m_identities; /*!< TODO */
    QString m_accountId;


};
}
}
#endif // SENDERIDENTITYMODEL_H
