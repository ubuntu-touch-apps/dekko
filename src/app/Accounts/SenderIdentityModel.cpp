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
#include "SenderIdentityModel.h"
#include "Common/SettingsNames.h"

namespace Dekko
{
namespace Accounts
{
SenderIdentityModel::SenderIdentityModel(QObject *parent) :
    QAbstractListModel(parent)
{
    connect(this, SIGNAL(accountIdChanged()), this, SLOT(reloadIdentities()));
}

QString SenderIdentityModel::accountId() const
{
    return m_accountId;
}

void SenderIdentityModel::setAccountId(const QString &accountId)
{
    if (accountId == m_accountId) {
        return;
    }
    m_accountId = accountId;
    emit accountIdChanged();
}

void SenderIdentityModel::reloadIdentities()
{
//    m_identities.clear();
    QSettings settings;
    settings.beginGroup(m_accountId);
    int size = settings.beginReadArray(Common::SettingsNames::identitiesKey);
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        SenderIdentity *identity = new SenderIdentity();
        identity->setName(settings.value(Common::SettingsNames::realNameKey).toString());
        identity->setEmail(settings.value(Common::SettingsNames::addressKey).toString());
        identity->setOrganization(settings.value(Common::SettingsNames::organisationKey).toString());
        identity->setUseDefaultSignature(settings.value("useDefaultSignature", true).toBool());
        identity->setSignature(settings.value(Common::SettingsNames::signatureKey).toString());
        appendRow(identity);
    }
    settings.endArray();
    settings.endGroup();
}

void SenderIdentityModel::saveIdentites()
{
    QSettings settings;
    settings.beginGroup(m_accountId);
    settings.beginWriteArray(Common::SettingsNames::identitiesKey);

    for (int i = 0; i < m_identities.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue(Common::SettingsNames::realNameKey, m_identities[i]->name());
        settings.setValue(Common::SettingsNames::addressKey, m_identities[i]->email());
        settings.setValue(Common::SettingsNames::organisationKey, m_identities[i]->organization());
        settings.setValue("useDefaultSignature", m_identities[i]->useDefaultSignature());
        settings.setValue(Common::SettingsNames::signatureKey, m_identities[i]->signature());
    }
    settings.endArray();
    settings.endGroup();
}

QHash<int, QByteArray> SenderIdentityModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdentityNameRole] = "name";
    roles[IdentityEmailRole] = "email";
    roles[IdentityOrganizationRole] = "organization";
    roles[IdentitySignatureRole] = "signature";
    roles[IdentityRecipientStringRole] = "formattedString";
    roles[IdentityUseDefaultSignature] = "useDefaultSignature";
    return roles;
}

QVariant SenderIdentityModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_identities.size()) {
        return QVariant();
    }
    switch (role) {
    case IdentityNameRole:
        return m_identities[index.row()]->name();
    case IdentityEmailRole:
        return m_identities[index.row()]->email();
    case IdentityOrganizationRole:
        return m_identities[index.row()]->organization();
    case IdentitySignatureRole:
        return m_identities[index.row()]->signature();
    case IdentityUseDefaultSignature:
        return m_identities[index.row()]->useDefaultSignature();
    case IdentityRecipientStringRole:
        return m_identities[index.row()]->toFormattedString();
    }
    return QVariant();
}

int SenderIdentityModel::rowCount(const QModelIndex &parent) const
{
    return m_identities.size();
}

QModelIndex SenderIdentityModel::indexFromItem(SenderIdentity *identity) const
{
    if (identity != NULL) {
        for (int i = 0; i < m_identities.size(); i++) {
            if (m_identities.at(i) == identity) {
                return index(i);
            }
        }
    }
    return QModelIndex();
}

bool SenderIdentityModel::removeRow(int row, const QModelIndex &index)
{
    if (row >= 0 || row < m_identities.size()) {
        beginRemoveRows(index, row, row);
        SenderIdentity *id = m_identities.takeAt(row);
        Q_ASSERT(id);
        id->deleteLater();
        id = 0;
        endRemoveRows();
        emit countChanged();
        return true;
    }
    return false;
}

bool SenderIdentityModel::removeRows(int row, int count, const QModelIndex &index)
{
    if (row >= 0 && count > 0 && (row + count) <= m_identities.size())
    {
        beginRemoveRows(index, row, row + count - 1);
        SenderIdentity *id = m_identities.takeAt(row);
        Q_ASSERT(id);
        id->deleteLater();
        id = 0;
        endRemoveRows();
        emit countChanged();
        return true;
    }
    return false;
}

QVariant SenderIdentityModel::get(int index)
{
    if (index < 0 || index >= m_identities.size()) {
        return QVariant();
    }
    SenderIdentity *identity = m_identities.at(index);
    QMap<QString, QVariant> identityData;
    identityData.insert(QString("name"), identity->name());
    identityData.insert(QString("email"), identity->email());
    identityData.insert(QString("organization"), identity->organization());
    identityData.insert(QString("signature"), identity->signature());
    identityData.insert(QString("formattedString"), identity->toFormattedString());
    identityData.insert(QString("useDefaultSignature"), identity->useDefaultSignature());
    return identityData;
}

QObject *SenderIdentityModel::getIdentity(int index)
{
    return m_identities.at(index);
}

bool SenderIdentityModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= m_identities.size()) {
        return false;
    }

    switch (role) {
    case IdentityEmailRole:
        m_identities[index.row()]->setEmail(value.toString());
        return true;
    case IdentityNameRole:
        m_identities[index.row()]->setName(value.toString());
        return true;
    case IdentityOrganizationRole:
        m_identities[index.row()]->setOrganization(value.toString());
        return true;
    case IdentitySignatureRole:
        m_identities[index.row()]->setSignature(value.toString());
        break;
    case IdentityUseDefaultSignature:
        m_identities[index.row()]->setUseDefaultSignature(value.toBool());
        break;
    }
    return false;
}

void SenderIdentityModel::appendRow(SenderIdentity *identity)
{
    if (identity != NULL) {
        appendRows(QList<SenderIdentity *>() << identity);
        emit countChanged();
    }
}

void SenderIdentityModel::appendRows(QList<SenderIdentity *> &identities)
{
    if (identities.size() == 0) {
        return;
    }
    beginInsertRows(QModelIndex() , rowCount(), rowCount() + identities.size() - 1);
    Q_FOREACH(SenderIdentity *id, identities) {
        id->setParent(this);
        connect(id, SIGNAL(dataChanged()), this, SLOT(updateIdentity()));
        m_identities.append(id);
    }
    endInsertRows();
    emit countChanged();
}

void SenderIdentityModel::insertRow(int row, SenderIdentity *identity)
{
    if (identity == NULL) {
        return;
    }
    beginInsertRows(QModelIndex(), row, row);
    connect(identity, SIGNAL(dataChanged()), this, SLOT(updateIdentity()));
    m_identities.insert(row, identity);
    endInsertRows();
    emit countChanged();
}

void SenderIdentityModel::clear()
{
    if (m_identities.size() == 0) {
        return;
    }
    beginRemoveRows(QModelIndex(), 0, m_identities.size());
    for (int i = 0; i < m_identities.count(); i++) {
        m_identities[i]->deleteLater();
    }
    m_identities.clear();
    endRemoveRows();
    emit countChanged();
}

void SenderIdentityModel::updateIdentity()
{
    SenderIdentity *identity = static_cast<SenderIdentity *>(sender());
    QModelIndex idx = indexFromItem(identity);
    if(idx.isValid()) {
        emit dataChanged(idx, idx);
    }
    saveIdentites();
}

}
}
