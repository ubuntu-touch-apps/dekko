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
#ifndef PRESETPROVIDERMODEL_H
#define PRESETPROVIDERMODEL_H

#include <QObject>
#include <QSettings>
#include <QAbstractListModel>

namespace Dekko
{
namespace Models
{

class PreSetProvider : public QObject
{
    Q_OBJECT

public:
    PreSetProvider(QObject *parent, QString providerFilePath, QString serviceName);
    QString serviceName(); /*< this should be the group name from the conf file */
    QString description();
    QVariant serviceInfo();
    QString icon();
private:
    QString m_providerFile;
    QString m_service;
};

class PreSetProviderModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit PreSetProviderModel(QObject *parent = 0);

    enum Roles {
        ServiceNameRole = Qt::UserRole + 1,
        DescriptionRole,
        ServiceInfoRole,
        IconRole
    };
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

signals:
    void countChanged();

public slots:
    void reloadProviders();

private slots:
    void appendProvider(PreSetProvider *provider);

private:
    QList<PreSetProvider *> m_providers;
    QString m_providerFile;
    QString findServiceProviderFile();

};
}
}
#endif // PRESETPROVIDERMODEL_H
