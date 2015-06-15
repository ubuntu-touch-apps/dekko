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
#ifndef SETTINGSFILEBASE_H
#define SETTINGSFILEBASE_H

#include <QObject>
#include <QStringList>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>
#include "SettingsObjectBase.h"

namespace Dekko
{
namespace Settings
{
class SettingsObjectBase;
class SettingsFileBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SettingsObjectBase *object READ object CONSTANT)
    Q_PROPERTY(QString errorString READ errorString NOTIFY error)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit SettingsFileBase(QObject *parent = 0);
    ~SettingsFileBase();
    QString path() const;
    bool setPath(const QString &path);
    SettingsObjectBase *object();
    const SettingsObjectBase *object() const;

    QString errorString() const;
    QJsonValue read(const QJsonObject &obj, const QStringList &path);
    bool write(const QStringList &path, const QJsonValue &value);
    bool readFile();
    bool writeFile();

    static QStringList splitPath(const QString &input, bool &ok);

signals:
    void error();
    void pathChanged();
    void modified(const QStringList &path, const QJsonValue &value);

protected:
    void reset();
    void setErrorString(const QString &msg);
    bool checkDirPermissions(const QString &path);

private slots:
    void sync();

private:
    QString m_path;
    QString m_errorString;
    SettingsObjectBase *m_rootObject;
    // This is the root jsonobject inside the AbstractSettingsObject
    QJsonObject m_jsonObject;
    QTimer m_syncTimer;
    friend class SettingsObjectBase;
};
}
}
#endif
