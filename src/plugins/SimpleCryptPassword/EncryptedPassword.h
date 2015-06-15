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

#ifndef ENCRYPTEDPASSWORD_H
#define ENCRYPTEDPASSWORD_H

#include <QString>

#include "Plugins/PasswordPlugin.h"
#include "Plugins/PluginInterface.h"
#include "SimpleCrypt.h"

class QSettings;

using namespace Plugins;

class EncryptedPasswordJob : public PasswordJob
{
    Q_OBJECT

public:
    enum Type {
        Request,
        Store,
        Delete
    };

    EncryptedPasswordJob(const QString &accountId, const QString &accountType,
                      const QString &password, enum Type type, QObject *parent,
                      QSettings *settings);



    QString encryptPassword(const QString &plainText);
    QString decryptPassword(const QString &encryptedText);
public slots:
    virtual void doStart();
    virtual void doStop();

private:
    QString m_accountId, m_accountType, m_password;
    enum Type m_type;
    QSettings *m_settings;
    SimpleCrypt *m_crypto;
};

class EncryptedPassword : public PasswordPlugin
{
    Q_OBJECT
public:
    EncryptedPassword(QObject *parent, QSettings *settings);
    virtual Features features() const;

public slots:
    virtual PasswordJob *requestPassword(const QString &accountId, const QString &accountType);
    virtual PasswordJob *storePassword(const QString &accountId, const QString &accountType, const QString &password);
    virtual PasswordJob *deletePassword(const QString &accountId, const QString &accountType);

private:
    QSettings *m_settings;
};

class dekko_plugin_SimpleCryptPasswordPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(Plugins::PluginInterface)
    Q_PLUGIN_METADATA(IID "com.ubuntu.developer.dpniel.dekko.password.simplecrypt")

public:
    virtual QString name() const;
    virtual QString description() const;
    virtual QObject *create(QObject *parent, QSettings *settings);
};

#endif // ENCRYPTEDPASSWORD_H
