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

#include "AccountsListModel.h"
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QUuid>
#include "app/Accounts/AccountUtils.h"

namespace Dekko
{
namespace Models
{

AccountsListModel::AccountsListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    reloadAccounts();
}

void AccountsListModel::reloadAccounts()
{
    if (rowCount() > 0) {
        clear();
    }
    QSettings m_settings;
    qDebug() << m_settings.fileName();
    if (QFile::exists(m_settings.fileName())) {
        Dekko::Accounts::AccountUtils::checkSettingsFilePermissions(&m_settings);
    } else {
        qDebug("Account settings don't exist yet no need to check the permissions");
    }
    qDebug() << "AccountListModel: reloading accounts";
    QStringList groups;
    qDebug() << "AccountListModel: Current settings group: " << m_settings.group();
    m_settings.beginGroup("Accounts");
    qDebug() << "AccountListModel: Current settings group should now be Accounts: " << m_settings.group();
    groups = m_settings.childGroups();
    qDebug() << "AccountListModel: Child groups are: " << groups;
    m_settings.endGroup();
    Q_FOREACH(const QString &group, groups) {
        qDebug() << "AccountListModel: Creating account object for account: " << group;
        Dekko::Settings::AccountProfile *account = new Dekko::Settings::AccountProfile(this);
        qDebug() << "AccountListModel: Setting accountId";
        account->setAccountId(group);
        qDebug() << "AccountListModel: Appending account to model";
        appendRow(account);
        qDebug() << "AccountListModel: Append successful";
    }

}

QHash<int, QByteArray> AccountsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[AccountNameRole] = "name";
    roles[AccountDescriptionRole] = "description";
    roles[AccountOrganizationRole] = "organization";
    roles[AccountDomainRole] = "domain";
    roles[AccountDomainIconRole] = "domainIcon";
    return roles;
}

QVariant AccountsListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_accounts.size()) {
        return QVariant();
    }
    switch (role) {
    case AccountIdRole:
        return m_accounts[index.row()]->accountId();
    case AccountNameRole:
        return m_accounts[index.row()]->name();
    case AccountDescriptionRole:
        return m_accounts[index.row()]->description();
    case AccountOrganizationRole:
        return m_accounts[index.row()]->organization();
    case AccountDomainRole:
        return m_accounts[index.row()]->domain();
    case AccountDomainIconRole:
        return m_accounts[index.row()]->domainIcon();
    }
    return QVariant();
}

int AccountsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_accounts.size();
}

QModelIndex AccountsListModel::indexFromItem(Dekko::Settings::AccountProfile *account) const
{
    if (account != NULL) {
        for (int i = 0; i < m_accounts.size(); i++) {
            if (m_accounts.at(i) == account) {
                return index(i);
            }
        }
    }
    return QModelIndex();
}

bool AccountsListModel::removeRow(int row, const QModelIndex &index)
{
    if (row >= 0 || row < m_accounts.size()) {
        beginRemoveRows(index, row, row);
        Dekko::Settings::AccountProfile *account = m_accounts.takeAt(row);
        QString id = account->accountId();
        delete account;
        account = 0;
        QSettings m_settings;
        Dekko::Accounts::AccountUtils::nukeCache(id);
        m_settings.beginGroup("Accounts");
        m_settings.remove(id);
        m_settings.endGroup();
        m_settings.remove(id);
        endRemoveRows();
        emit countChanged();
        return true;
    }
    return false;
}

bool AccountsListModel::removeRows(int row, int count, const QModelIndex &index)
{
    if (row >= 0 && count > 0 && (row + count) <= m_accounts.size())
    {
        beginRemoveRows(index, row, row + count - 1);
        Dekko::Settings::AccountProfile *account = m_accounts.takeAt(row);
        QString id = account->accountId();
        delete account;
        account = 0;
        QSettings m_settings;
        Dekko::Accounts::AccountUtils::nukeCache(id);
        m_settings.beginGroup("Accounts");
        m_settings.remove(id);
        m_settings.endGroup();
        m_settings.remove(id);
        endRemoveRows();
        emit countChanged();
        return true;
    }
    return false;
}

QVariant AccountsListModel::get(int index)
{
    if (index < 0 || index > m_accounts.size()) {
        return QVariant();
    }
    Dekko::Settings::AccountProfile *account = m_accounts.at(index);
    QMap<QString, QVariant> accountData;
    accountData.insert(QString("accountId"), account->accountId());
    accountData.insert(QString("name"), account->name());
    accountData.insert(QString("description"), account->description());
    accountData.insert(QString("organization"), account->organization());
    accountData.insert(QString("domain"), account->domain());
    accountData.insert(QString("domainIcon"), account->domainIcon());
    return QVariant(accountData);
}

void AccountsListModel::appendRow(Settings::AccountProfile *account)
{
    appendRows(QList<Dekko::Settings::AccountProfile *>() << account);
}

void AccountsListModel::appendRows(QList<Dekko::Settings::AccountProfile *> &account)
{
    if (account.size() == 0) {
        return;
    }
    qDebug() << "Starting to insert rows";
    beginInsertRows(QModelIndex() , rowCount(), rowCount() + account.size() - 1);
    Q_FOREACH(Dekko::Settings::AccountProfile *acc, account) {
//        qDebug() << "Appending " << acc->accountName() << "to model";
//        connect(acc, SIGNAL(dataChanged()), this, SLOT(updateAccount()));
        m_accounts.append(acc);
    }
    endInsertRows();
    emit countChanged();
}

void AccountsListModel::insertRow(int row, Settings::AccountProfile *account)
{
    if (account == NULL) {
        return;
    }
    beginInsertRows(QModelIndex(), row, row);
    m_accounts.insert(row, account);
    endInsertRows();
    emit countChanged();
}

void AccountsListModel::clear()
{
    if (m_accounts.size() == 0) {
        return;
    }
    beginRemoveRows(QModelIndex(), 0, m_accounts.size());
    m_accounts.clear();
    endRemoveRows();
    emit countChanged();
}

}
}
