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
#include "PreSetProviderModel.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include "app/Accounts/AccountUtils.h"

namespace Dekko
{
namespace Models
{
PreSetProvider::PreSetProvider(QObject *parent, QString providerFilePath, QString serviceName) :
    QObject(parent), m_providerFile(providerFilePath), m_service(serviceName)
{
}

QString PreSetProvider::serviceName()
{
    return m_service;
}

QString PreSetProvider::description()
{
    QSettings s(m_providerFile, QSettings::NativeFormat);
    QString description;
    s.beginGroup(m_service);
    description =  s.value("description").toString();
    s.endGroup();
    return description;
}

QVariant PreSetProvider::serviceInfo()
{
    QSettings s(m_providerFile, QSettings::NativeFormat);
    s.beginGroup(m_service);
    QMap<QString, QVariant> serviceInfo;
    serviceInfo.insert(QString("description"), s.value("description").toString());
    serviceInfo.insert(QString("imapServer"), s.value("imap.server").toString());
    serviceInfo.insert(QString("imapPort"), s.value("imap.port", 0).toInt());
    serviceInfo.insert(QString("imapSSL"), s.value("imap.useSsl", false).toBool());
    serviceInfo.insert(QString("imapStartTls"), s.value("imap.startTls", false).toBool());
    serviceInfo.insert(QString("smtpServer"), s.value("smtp.server").toString());
    serviceInfo.insert(QString("smtpPort"), s.value("smtp.port", 0).toInt());
    serviceInfo.insert(QString("smtpSSL"), s.value("smtp.useSsl", false).toBool());
    serviceInfo.insert(QString("smtpStartTls"), s.value("smtp.startTls", false).toBool());
    s.endGroup();
    return serviceInfo;
}

QString PreSetProvider::icon()
{
    QSettings s(m_providerFile, QSettings::NativeFormat);
    s.beginGroup(m_service);
    QString fakeEmail = QString("fake@%1").arg(s.value("domain").toString());
    s.endGroup();
    return Accounts::AccountUtils::getDomainIcon(fakeEmail);
}

PreSetProviderModel::PreSetProviderModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_providerFile = findServiceProviderFile();
    reloadProviders();
}

QHash<int, QByteArray> PreSetProviderModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ServiceNameRole] = "serviceName";
    roles[DescriptionRole] = "description";
    roles[ServiceInfoRole] = "serviceInfo";
    roles[IconRole] = "icon";
    return roles;
}

QVariant PreSetProviderModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_providers.size()) {
        return QVariant();
    }
    switch (role) {
    case ServiceNameRole:
        return m_providers[index.row()]->serviceName();
    case DescriptionRole:
        return m_providers[index.row()]->description();
    case ServiceInfoRole:
        return m_providers[index.row()]->serviceInfo();
    case IconRole:
        return m_providers[index.row()]->icon();
    }
    return QVariant();
}

int PreSetProviderModel::rowCount(const QModelIndex &parent) const
{
    return m_providers.size();
}

void PreSetProviderModel::reloadProviders()
{
    QSettings s(m_providerFile, QSettings::NativeFormat);
    Q_FOREACH(const QString &group, s.childGroups()) {
        qDebug() << "Pre-set provider " << group << " found!";
        PreSetProvider *provider = new PreSetProvider(this, m_providerFile, group);
        appendProvider(provider);
    }
}

void PreSetProviderModel::appendProvider(PreSetProvider *provider)
{
    if (provider != NULL) {
        beginInsertRows(QModelIndex() ,rowCount(), rowCount() + 1);
        m_providers.append(provider);
        endInsertRows();
        emit countChanged();
    }
}

QString PreSetProviderModel::findServiceProviderFile()
{
    QString configFile;
    // TODO: make this configurable
    const QString filePath = QLatin1String("configuration/serviceProviders.conf");
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    paths.prepend(QDir::currentPath());
    paths.prepend(QCoreApplication::applicationDirPath());
    Q_FOREACH (const QString &path, paths) {
        QString myPath = path + QLatin1Char('/') + filePath;
        if (QFile::exists(myPath)) {
            configFile = myPath;
            break;
        }
    }
    if (!configFile.isEmpty()) {
        qDebug() << "Config file found";
        return configFile;
    }
    qDebug() <<" Config file not found";
    return QString();
}
}
}
