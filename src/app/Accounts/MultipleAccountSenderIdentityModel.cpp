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

#include "MultipleAccountSenderIdentityModel.h"
#include "SenderIdentityModel.h"
#include <QDebug>

namespace Dekko {
namespace Accounts {

MultipleAccountSenderIdentityModel::MultipleAccountSenderIdentityModel(QObject *parent)
    : RowsJoinerProxy(parent)
{

}

QHash<int, QByteArray> MultipleAccountSenderIdentityModel::roleNames() const
{
    if (m_identityModels.size() > 0)
        return m_identityModels.at(0)->roleNames();
    else
        return QHash<int, QByteArray>();
}

QStringList MultipleAccountSenderIdentityModel::accountIds()
{
    Q_ASSERT(false);
    return QStringList();
}

void MultipleAccountSenderIdentityModel::setAccountIds(QStringList accountIds)
{
    if (accountIds.size() <= 0) {
        return;
    }
    if (m_identityModels.size() > 0) {
        for (int i = 0; i < m_identityModels.size(); i++) {
            removeSourceModel(m_identityModels[i]);
            m_identityModels[i]->deleteLater();
        }
        m_identityModels.clear();
    }
    for (int i = 0; i < accountIds.size(); i++) {
        SenderIdentityModel * newModel = new SenderIdentityModel(this);
        newModel->setAccountId(accountIds[i]);
        m_identityModels.append(newModel);
        insertSourceModel(newModel);
    }
    m_accountIds = accountIds;
    emit accountIdsChanged();
}

QVariant MultipleAccountSenderIdentityModel::get(int idx)
{
    for (int i = 0; i < m_identityModels.size(); i++) {
        if (idx >= m_identityModels[i]->rowCount()) {
            idx -= m_identityModels[i]->rowCount();
        } else {
            return ((SenderIdentityModel *)m_identityModels[i])->get(idx);
        }
    }
    return 0;
}

QString MultipleAccountSenderIdentityModel::getAccountIdForIdentityIdx(int idx)
{
    for (int i = 0; i < m_identityModels.size(); i++) {
        if (idx >= m_identityModels[i]->rowCount()) {
            idx -= m_identityModels[i]->rowCount();
        } else {
            qDebug() << "m_accountIds[i]" << m_accountIds[i];
            return m_accountIds[i];
        }
    }
    Q_ASSERT(false);
    return "";
}

int MultipleAccountSenderIdentityModel::getFirstIdentityIdxForAccountId(QString accountId)
{
    int idx = 0;
    for (int i = 0; i < m_accountIds.size(); i++) {
        if (accountId != m_accountIds[i]) {
            idx += m_identityModels[i]->rowCount();
        } else {
            return idx;
        }
    }
    return idx;
}

}
}
