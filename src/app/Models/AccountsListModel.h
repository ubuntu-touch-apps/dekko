/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the Dekko

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

#ifndef ACCOUNTSLISTMODEL_H
#define ACCOUNTSLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "app/Accounts/Account.h"
#include "app/Settings/Settings.h"

class QSettings;

namespace Dekko
{
namespace Models
{
/*! This model displays a lightweight list of accounts using only the AccountProfile from each account
 *
 * It has no functionlaity bar getting useful info i.e an accountId for a specific account
 * This makes it faster to load an accounts list than having to reload all settings for all accounts.
 *
*/
class AccountsListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    AccountsListModel(QObject *parent = 0);

    enum Roles {
        AccountIdRole = Qt::UserRole + 1,
        AccountNameRole,
        AccountDescriptionRole,
        AccountOrganizationRole,
        AccountDomainRole,
        AccountDomainIconRole
    };
    Q_INVOKABLE void reloadAccounts();

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex indexFromItem(Settings::AccountProfile *account) const;

    Q_INVOKABLE bool removeRow(int row, const QModelIndex &index = QModelIndex());
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &index = QModelIndex());
    Q_INVOKABLE QVariant get(int index);

signals:
    void countChanged();

public slots:
    void appendRow(Dekko::Settings::AccountProfile *account);
    void appendRows(QList<Settings::AccountProfile *> &account);
    void insertRow(int row, Dekko::Settings::AccountProfile *account);
    void clear();

private slots:
//    void updateAccount();

private:
    QList<Dekko::Settings::AccountProfile *> m_accounts;
};

}
}

#endif // ACCOUNTSLISTMODEL_H
