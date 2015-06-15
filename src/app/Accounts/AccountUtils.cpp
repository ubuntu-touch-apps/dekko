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

#include "AccountUtils.h"
#include <QStringList>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include "Common/Paths.h"

namespace Dekko {
namespace Accounts {


AccountUtils::AccountUtils(QObject *parent) :
    QObject(parent)
{
}

QString AccountUtils::getDomainIcon(const QString &emailAddress)
{
    QString domainProvider;
    QString domainConfFile = AccountUtils::findDomainConfFile();
    if (domainConfFile.isEmpty()) {
        return QString("../../icons/actions/email.svg");
    }
    QSettings domainServices(domainConfFile, QSettings::NativeFormat);
    QString domain;
    if (emailAddress.contains("@", Qt::CaseInsensitive)) {
        domain = emailAddress.split("@")[1];
    } else {
        domain = QString();
    }
    Q_FOREACH(const QString &group, domainServices.childGroups()) {
        if (group == domain) {
            domainServices.beginGroup(group);
            domainProvider = domainServices.value("serviceProvider").toString();
            domainServices.endGroup();
            break;
        }
    }
    if (domainProvider.isEmpty()) {
        return QString("qrc:///actions/mail-unread.svg");
    }
    if (domainProvider == QString("apple")) {
        return QString("qrc:///actions/mail-unread.svg");
    } else if (domainProvider == QString("aol")) {
        return QString("qrc:///actions/mail-unread.svg");
    } else if (domainProvider == QString("canonical")) {
        return QString("qrc:///provider/canonical.svg");
    } else if (domainProvider == QString("google")) {
        return QString("qrc:///provider/online-account-google.svg");
    } else if (domainProvider == QString("mail.ru")) {
        return QString("qrc:///actions/mail-unread.svg");
    } else if (domainProvider == QString("mozilla")) {
        return QString("qrc:///actions/mail-unread.svg");
    } else if (domainProvider == QString("orange.fr")) {
        return QString("qrc:///actions/mail-unread.svg");
    } else if (domainProvider == QString("outlook")) {
        return QString("qrc:///provider/online-account-outlook.svg");
    } else if (domainProvider == QString("ubuntu")) {
        return QString("qrc:///provider/ubuntu.svg");
    } else if (domainProvider == QString("yahoo") || domainProvider == QString("yahoo.jp")){
        return QString("qrc:///provider/online-account-yahoo.svg");
    } else {
        return QString("qrc:///actions/mail-unread.svg");
    }
}

QString AccountUtils::findDomainConfFile()
{
    QString configFile;
    const QString filePath = QLatin1String("configuration/domainService.conf");
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
        return configFile;
    }
    return QString();
}

void AccountUtils::nukeCache(QString &accountName)
{
    QString cacheDir = Common::writablePath(Common::LOCATION_CACHE) + accountName + QLatin1Char('/');
    Imap::removeRecursively(cacheDir);
}

void AccountUtils::checkSettingsFilePermissions(QSettings *s)
{
    QFile::Permissions expectedPerms = QFile::ReadOwner | QFile::WriteOwner;
    if (QFileInfo(s->fileName()).permissions() != expectedPerms) {
        qDebug() << "Changing settings file permissions";
        if (!QFile::setPermissions(s->fileName(), expectedPerms)) {
//            qFatal("Failed to set sufficient permissions on settings file");
            qDebug() << "Failed to set sufficient permissions on settings file";
        }
    }

    QFile::Permissions expectedDirPerms = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner;
    QString sDir = QFileInfo(s->fileName()).absolutePath();
    if (QFileInfo(sDir).permissions() != expectedDirPerms) {
        qDebug() << "Changing settings directory permissions";
        if (!QFile::setPermissions(sDir, expectedDirPerms)) {
//            qFatal("Failed to set sufficient permissions on settings Directory");
            qDebug() << "Failed to set sufficient permissions on settings directory";
        }
    }
}
}
}
