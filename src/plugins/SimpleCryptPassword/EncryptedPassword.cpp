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

#include "EncryptedPassword.h"

#include <QString>
#include <QSettings>
#include "SimpleCrypt.h"

struct Settings {
    static QString imapPasskey, smtpPasskey;
};
QString Settings::imapPasskey = QLatin1String("imap.auth.pass");
QString Settings::smtpPasskey = QLatin1String("msa.smtp.auth.pass");

EncryptedPasswordJob::EncryptedPasswordJob(
        const QString &accountId, const QString &accountType,
        const QString &password, enum Type type,
        QObject *parent, QSettings *settings) : PasswordJob(parent),
    m_accountId(accountId), m_accountType(accountType), m_password(password),
    m_type(type), m_settings(settings), m_crypto(0)
{
    m_crypto = new SimpleCrypt(Q_UINT64_C(0x0c2ad4a4acb9f023));
}

QString EncryptedPasswordJob::encryptPassword(const QString &plainText)
{
    if (!plainText.isEmpty()) {
        return m_crypto->encryptToString(plainText);
    }
    return QString();
}

QString EncryptedPasswordJob::decryptPassword(const QString &encryptedText)
{
    if (!encryptedText.isEmpty()) {
        return m_crypto->decryptToString(encryptedText);
    }
    return QString();
}


void EncryptedPasswordJob::doStart()
{
    QString password;
    QSettings settings;
    settings.beginGroup(m_accountId);
    switch (m_type) {
    case Request:
        if (m_accountType == QLatin1String("imap")) {
            password = decryptPassword(settings.value(Settings::imapPasskey).toString());
        } else if (m_accountType == QLatin1String("smtp")) {
            password = decryptPassword(settings.value(Settings::smtpPasskey).toString());
        } else {
            emit error(PasswordJob::NoSuchPassword, tr("This plugin only supports storing imap and smtp passwords"));
            break;
        }
        emit passwordAvailable(m_accountId, m_accountType, password);
        break;
    case Store:
        if (m_accountType == QLatin1String("imap")) {
            settings.setValue(Settings::imapPasskey, encryptPassword(m_password));
        } else if (m_accountType == QLatin1String("smtp")) {
            settings.setValue(Settings::smtpPasskey, encryptPassword(m_password));
        } else {
            emit error(PasswordJob::UnknownError, tr("This plugin only supports storing of IMAP and SMTP passwords"));
            break;
        }
        emit passwordStored(m_accountId);
        break;

    case Delete:
        if (m_accountType == QLatin1String("imap")) {
            settings.remove(Settings::imapPasskey);
        } else if (m_accountType == QLatin1String("smtp")) {
            settings.remove(Settings::smtpPasskey);
        } else {
            emit error(PasswordJob::UnknownError, tr("This plugin only supports storing of IMAP and SMTP passwords"));
            break;
        }
        emit passwordDeleted(m_accountId);
        break;
    }
    settings.endGroup();
}

void EncryptedPasswordJob::doStop()
{
    emit error(PasswordJob::Stopped, QString());
}

EncryptedPassword::EncryptedPassword(QObject *parent, QSettings *settings) : PasswordPlugin(parent), m_settings(settings)
{
}

PasswordPlugin::Features EncryptedPassword::features() const
{
    return PasswordPlugin::FeatureEncryptedStorage;
}

PasswordJob *EncryptedPassword::requestPassword(const QString &accountId, const QString &accountType)
{
    return new EncryptedPasswordJob(accountId, accountType, QString(), EncryptedPasswordJob::Request, this, m_settings);
}

PasswordJob *EncryptedPassword::storePassword(const QString &accountId, const QString &accountType, const QString &password)
{
    return new EncryptedPasswordJob(accountId, accountType, password, EncryptedPasswordJob::Store, this, m_settings);
}

PasswordJob *EncryptedPassword::deletePassword(const QString &accountId, const QString &accountType)
{
    return new EncryptedPasswordJob(accountId, accountType, QString(), EncryptedPasswordJob::Delete, this, m_settings);
}

QString dekko_plugin_SimpleCryptPasswordPlugin::name() const
{
    return QLatin1String("simplecryptpassword");
}

QString dekko_plugin_SimpleCryptPasswordPlugin::description() const
{
    return trUtf8("Dekko's simple encrypted storage");
}

QObject *dekko_plugin_SimpleCryptPasswordPlugin::create(QObject *parent, QSettings *settings)
{
    Q_ASSERT(settings);
    return new EncryptedPassword(parent, settings);
}
