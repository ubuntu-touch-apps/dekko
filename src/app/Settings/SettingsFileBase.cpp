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
#include "SettingsFileBase.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QSaveFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDebug>
#include <functional>

namespace Dekko
{
namespace Settings
{
SettingsFileBase::SettingsFileBase(QObject *parent) :
    QObject(parent), m_rootObject(0)
{
    m_rootObject = new SettingsObjectBase(this);
    m_syncTimer.setInterval(0);
    m_syncTimer.setSingleShot(true);
    connect(&m_syncTimer, SIGNAL(timeout()), this, SLOT(sync()));
}
SettingsFileBase::~SettingsFileBase()
{
    if (m_syncTimer.isActive()) {
        sync();
    }
    m_rootObject->deleteLater();
}

QString SettingsFileBase::path() const
{
    return m_path;
}

bool SettingsFileBase::setPath(const QString &path)
{
    if (m_path == path) {
        return false;
    }
    reset();
    m_path = path;
    // Get the path and check it exists if not try and make it
    QFileInfo info(m_path);
    QDir dir(info.path());
    if (!dir.exists() && !dir.mkpath(QLatin1String("."))) {
        setErrorString(QString("Cannot create directory: %1").arg(dir.path()));
        return false;
    }
    checkDirPermissions(info.path());
    emit pathChanged();
    return readFile();
}

SettingsObjectBase *SettingsFileBase::object()
{
    return m_rootObject;
}

const SettingsObjectBase *SettingsFileBase::object() const
{
    return m_rootObject;
}

QString SettingsFileBase::errorString() const
{
    return m_errorString;
}

QJsonValue SettingsFileBase::read(const QJsonObject &obj, const QStringList &path)
{
    QJsonValue val = obj;
    Q_FOREACH(const QString &key, path) {
        QJsonObject object = val.toObject();
        if (object.isEmpty() || (val = object.value(key)).isUndefined()) {
            return QJsonValue::Undefined;
        }
    }
    return val;
}

typedef QList<QPair<QStringList, QJsonValue>> ChangedList;
static void findChangedRecursive(ChangedList &list, const QStringList &objPath, const QJsonValue &oldValue, const QJsonValue &newValue) {
    if (oldValue.isObject() || newValue.isObject()) {
        // If either is a non-object type, this returns an empty object
        QJsonObject oldObject = oldValue.toObject();
        QJsonObject newObject = newValue.toObject();

        // Iterate keys of the original object and compare to new
        for (QJsonObject::iterator it = oldObject.begin(); it != oldObject.end(); it++) {
            QJsonValue newSubValue = newObject.value(it.key());
            if (*it == newSubValue)
                continue;

            if ((*it).isObject() || newSubValue.isObject())
                findChangedRecursive(list, QStringList() << objPath << it.key(), *it, newSubValue);
            else
                list.append(qMakePair(QStringList() << objPath << it.key(), newSubValue));
        }

        // Iterate keys of the new object that may not be in original
        for (QJsonObject::iterator it = newObject.begin(); it != newObject.end(); it++) {
            if (oldObject.contains(it.key()))
                continue;

            if ((*it).isObject())
                findChangedRecursive(list, QStringList() << objPath << it.key(), QJsonValue::Undefined, it.value());
            else
                list.append(qMakePair(QStringList() << objPath << it.key(), it.value()));
        }
    } else
        list.append(qMakePair(objPath, newValue));
}

bool SettingsFileBase::write(const QStringList &path, const QJsonValue &value)
{

    typedef QVarLengthArray<QPair<QString,QJsonObject> > ObjectArray;
    ObjectArray stack;
    QJsonValue current = m_jsonObject;
    QJsonValue originalValue;
    QString currentKey;

    foreach (const QString &key, path) {
        const QJsonObject &parent = current.toObject();
        stack.append(qMakePair(currentKey, parent));
        current = parent.value(key);
        currentKey = key;
    }

    // ObjectArray now contains parent objects starting with the root, and current
    // is the old value. Write back changes in reverse.
    if (current == value)
        return false;
    originalValue = current;
    current = value;

    ObjectArray::const_iterator it = stack.end(), begin = stack.begin();
    while (it != begin) {
        --it;
        QJsonObject update = it->second;
        update.insert(currentKey, current);
        current = update;
        currentKey = it->first;
    }

    // current is now the updated jsonRoot
    m_jsonObject = current.toObject();
    m_syncTimer.start();

    ChangedList changed;
    findChangedRecursive(changed, path, originalValue, value);

    for (ChangedList::iterator it = changed.begin(); it != changed.end(); it++)
        emit this->modified(it->first, it->second);

    return true;
}

void SettingsFileBase::reset()
{
    m_errorString.clear();
    m_path.clear();
    m_jsonObject = QJsonObject();
    emit modified(QStringList(), m_jsonObject);
}

void SettingsFileBase::setErrorString(const QString &msg)
{
    m_errorString = msg;
    emit error();
}

bool SettingsFileBase::checkDirPermissions(const QString &path)
{
    static QFile::Permissions desired = QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ExeUser;
    static QFile::Permissions ignored = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner;

    QFile file(path);
    if ((file.permissions() & ~ignored) != desired) {
        qDebug() << "Correcting permissions on configuration directory";
        if (!file.setPermissions(desired)) {
            qWarning() << "Correcting permissions on configuration directory failed";
            return false;
        }
    }

    return true;
}

bool SettingsFileBase::readFile()
{
    QFile config(m_path);
    if (!config.open(QIODevice::ReadWrite)) {
        setErrorString(config.errorString());
        return false;
    }
    QByteArray contents = config.readAll();
    if (contents.isEmpty()) {
        // ok we havn't previously saved anything so let's just set
        // a default jsonobject to return
        m_jsonObject = QJsonObject();
        return true;
    }
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(contents);
    if (doc.isNull()) {
        setErrorString(err.errorString());
        return false;
    }

    // at the moment we only support a rootobject, in the future
    // would be good to expand to support an array here aswell.
    if (!doc.isObject()) {
        setErrorString(QLatin1String("Unsupported config file type. Expecting an object here."));
        return false;
    }
    m_jsonObject = doc.object();
    emit modified(QStringList(), m_jsonObject);
    return true;
}

bool SettingsFileBase::writeFile()
{
    QSaveFile config(m_path);
    if (!config.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        setErrorString(config.errorString());
        return false;
    }

    QJsonDocument document(m_jsonObject);
    QByteArray data = document.toJson();
    if (data.isEmpty() && !document.isEmpty()) {
        setErrorString(QLatin1String("Encoding failure"));
        return false;
    }

    if (config.write(data) < data.size() || !config.commit()) {
        setErrorString(config.errorString());
        return false;
    }
    return true;
}

QStringList SettingsFileBase::splitPath(const QString &input, bool &ok)
{
    QStringList objectList = input.split(QLatin1Char('.'));
    // There should be no empty obj keys in here
    Q_FOREACH(const QString &obj, objectList) {
        if (obj.isEmpty()) {
            ok = false;
            return QStringList();
        }
    }
    ok = true;
    return objectList;
}

void SettingsFileBase::sync()
{
    if (m_path.isEmpty())
        return;
    m_syncTimer.stop();
    writeFile();
}
}
}
