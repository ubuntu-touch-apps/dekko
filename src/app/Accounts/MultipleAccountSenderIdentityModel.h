/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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

#ifndef MULTIPLEACCOUNTSENDERIDENTITYMODEL_H
#define MULTIPLEACCOUNTSENDERIDENTITYMODEL_H

#include "3rdParty/RowsJoinerProxy.h"

namespace Dekko {

namespace Accounts {

class MultipleAccountSenderIdentityModel: public RowsJoinerProxy
{
    Q_OBJECT
    Q_PROPERTY(QStringList accountIds READ accountIds WRITE setAccountIds NOTIFY accountIdsChanged)

public:
    explicit MultipleAccountSenderIdentityModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    QStringList accountIds();
    void setAccountIds(QStringList accountIds);

    Q_INVOKABLE QVariant get(int idx);
    Q_INVOKABLE QString getAccountIdForIdentityIdx(int idx);
    Q_INVOKABLE int getFirstIdentityIdxForAccountId(QString accountId);

signals:
    void accountIdsChanged();
private:
    QList<QAbstractItemModel *> m_identityModels;
    QStringList m_accountIds;
};

}

}

#endif
