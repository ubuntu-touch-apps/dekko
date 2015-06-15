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
#include "SettingsObjectBase.h"
#include <QDebug>

namespace Dekko
{
namespace Settings
{
SettingsObjectBase::SettingsObjectBase(QObject *parent) :
    QObject(parent), m_file(0), m_invalid(true)
{
}

QJsonObject SettingsObjectBase::data() const
{
    return m_object;
}

void SettingsObjectBase::setData(const QJsonObject &data)
{
    if (m_invalid || m_object == data) {
        return;
    }
    m_object = data;
    m_file->write(m_path, m_object);
}

QString SettingsObjectBase::path() const
{
    return m_path.join(QLatin1Char('.'));
}

void SettingsObjectBase::setPath(const QString &path)
{
    bool ok = false;
    // Split this new path
    QStringList p = SettingsFileBase::splitPath(path, ok);
    if (!ok) {
        // set invalid and return and empty object
        m_invalid = true;
        m_path.clear();
        m_object = QJsonObject();
        emit dataChanged();
        emit pathChanged();
        return;
    }
    if (m_path == p && m_invalid) {
        return;
    }
    m_path = p;
    if (m_file) {
        m_invalid = false;
        m_object = m_file->read(m_file->m_jsonObject, m_path).toObject();
        emit dataChanged();
    }
    emit pathChanged();
}

bool SettingsObjectBase::isValid() const
{
    return !m_invalid;
}

QJsonValue SettingsObjectBase::read(const QString &key, const QJsonValue &defaultValue) const
{
    bool ok = false;
    QStringList splitKey = SettingsFileBase::splitPath(key, ok);
    if (m_invalid || !ok || splitKey.isEmpty()) {
        qDebug() << "Invalid settings for path: " << key;
        return defaultValue;
    }

    QJsonValue ret = m_file->read(m_object, splitKey);
    if (ret.isUndefined())
        ret = defaultValue;
    return ret;
}

void SettingsObjectBase::write(const QString &key, const QJsonValue &value)
{
    bool ok = false;
    QStringList splitKey = SettingsFileBase::splitPath(key, ok);
    if (m_invalid || !ok || splitKey.isEmpty()) {
        qDebug() << "Invalid settings, can't write to path: " << key;
        return;
    }

    splitKey = m_path + splitKey;
    m_file->write(splitKey, value);
}

void SettingsObjectBase::undefine()
{
    if (m_invalid)
        return;

    m_object = QJsonObject();
    m_file->write(m_path, QJsonValue::Undefined);
}

void SettingsObjectBase::modified(const QStringList &path, const QJsonValue &value)
{
    if (path.size() < m_path.size()) {
        return;
    }
    // Check the paths actually match
    for (int var = 0; var < m_path.size(); ++var) {
        if (m_path[var] != path[var]) {
            return;
        }
    }
    m_object = m_file->read(m_file->m_jsonObject, m_path).toObject();
    emit modified(QStringList(path.mid(m_path.size())).join(QLatin1Char('.')), value);
    emit dataChanged();
}

void SettingsObjectBase::setFilePath(SettingsFileBase *file)
{
    if (m_file == file) {
        return;
    }
    if (m_file) {
        disconnect(m_file, 0, this, 0);
    }
    m_file = file;
    if (m_file) {
        connect(m_file, SIGNAL(modified(QStringList,QJsonValue)), this, SLOT(modified(QStringList,QJsonValue)));
    }
}

}
}
