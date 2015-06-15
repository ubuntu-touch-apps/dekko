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
#include "MailboxSettings.h"
#include <QModelIndex>
#include "Imap/Model/ItemRoles.h"

namespace Dekko
{
namespace Settings
{

MailboxSettings::MailboxSettings(QObject *parent) :
    SettingsObjectBase(parent), m_hasOverride(false)
{
    setFilePath(defaultFile());
    if (m_file) {
        // initially set the path to the root object
        setPath(QString());
    }
    connect(this, SIGNAL(dataChanged()), this, SIGNAL(objectDataChanged()));
}


const QVariantMap MailboxSettings::keys()
{
    QVariantMap m_keys;
    m_keys["global"] = QLatin1String("global");
    m_keys["override"] = QLatin1String("override");
    m_keys["sentMailbox"] = QLatin1String("sentMailbox");
    m_keys["trashMailbox"] = QLatin1String("trashMailbox");
    m_keys["draftsMailbox"] = QLatin1String("draftsMailbox");
    m_keys["defaultIdentity"] = QLatin1String("defaultIdentity");
    m_keys["deleted"] = QLatin1String("deleted");
    m_keys["autoExpunge"] = QLatin1String("deleted.autoExpunge");
    m_keys["markRead"] = QLatin1String("deleted.markRead");
    m_keys["moveToTrash"] = QLatin1String("deleted.moveToTrash");
    m_keys["preserveFlagged"] = QLatin1String("deleted.preserveFlagged");
    m_keys["submission"] = QLatin1String("submission");
    m_keys["quoteOriginalMessage"] = QLatin1String("submission.quoteOriginalMessage");
    m_keys["replyQuotePosition"] = QLatin1String("submission.replyQuotePosition");
    m_keys["includeSignature"] = QLatin1String("submission.includeSignature");
    m_keys["saveToImap"] = QLatin1String("submission.saveToImap");
    m_keys["allowRetries"] = QLatin1String("submission.allowRetries");
    m_keys["includeCC"] = QLatin1String("submission.includeCC");
    m_keys["cc"] = QLatin1String("submission.cc");
    m_keys["includeBCC"] = QLatin1String("submission.includeBCC");
    m_keys["bcc"] = QLatin1String("submission.bcc");
    m_keys["drafts"] = QLatin1String("drafts");
    m_keys["saveToDrafts"] = QLatin1String("drafts.saveToDrafts");
    m_keys["saveLocal"] = QLatin1String("drafts.saveLocal");
    m_keys["saveDraftInterval"] = QLatin1String("drafts.saveDraftInterval");
    m_keys["archive"] = QLatin1String("archive");
    m_keys["archiveEnabled"] = QLatin1String("archive.enabled");
    m_keys["archiveFolder"] = QLatin1String("archive.folder");
    m_keys["archiveFormat"] = QLatin1String("archive.format");
    m_keys["keepMboxStructure"] = QLatin1String("archive.keepMboxStructure");
    return m_keys;
}

QObject *MailboxSettings::account()
{
    return m_account;
}

void MailboxSettings::setAccount(QObject *account)
{
    m_account = qobject_cast<Accounts::Account *>(account);
    m_hasOverride = false;
    determineGlobalOrOverride();
    emit accountChanged();
}

QString MailboxSettings::mailboxName() const
{
    return m_mboxName;
}

void MailboxSettings::setMailboxName(const QString &mboxName)
{
    if (mboxName.isEmpty())
        return;
    m_hasOverride = false;
    m_mboxName = mboxName;
    determineGlobalOrOverride();
    emit mailboxNameChanged();
}

QJsonObject MailboxSettings::submission() const
{
    return data().value(QLatin1String("submission")).toObject();
}

QJsonObject MailboxSettings::deleted() const
{
    return data().value(QLatin1String("deleted")).toObject();
}

QJsonObject MailboxSettings::drafts() const
{
    return data().value(QLatin1String("drafts")).toObject();
}

static QPointer<SettingsFileBase> mboxDefaultFile;
SettingsFileBase *MailboxSettings::defaultFile()
{
    return mboxDefaultFile;
}

void MailboxSettings::setDefaultFile(SettingsFileBase *file)
{
    mboxDefaultFile = file;
}

void MailboxSettings::createOverride()
{
    if (m_hasOverride) {
        qDebug() << "Mailbox already has an override";
        return;
    }
    if (m_mboxName.isEmpty()) {
        return;
    }
    if (m_account.isNull()) {
        return;
    }
    QJsonObject newObject = m_object;
    QString newPath = QString("%1.%2").arg(keys().value(QString("override")).toString()).arg(m_mboxPath);
    setPath(m_account.data()->accountId());
    write(newPath, newObject);
    setPath(QString("%1.%2").arg(path()).arg(newPath));
    qDebug() << "Current Path Is :" << path();
    m_hasOverride = true;
}

void MailboxSettings::removeOverride()
{
    if (m_hasOverride) {
        undefine();
        m_hasOverride = false;
        setPath(QString("%1.%2").arg(m_account.data()->accountId()).arg(keys().value(QString("global")).toString()));
        qDebug() << "Undefined override, path switched to: " << path();
    } else {
        qDebug() << "No override to remove";
    }
}

void MailboxSettings::createDefaults()
{
    if (m_account.isNull()) {
        return;
    }
    QVariantMap map = keys();
    setPath(QString("%1.global").arg(m_account.data()->accountId()));
    write(map.value("sentMailbox").toString(), QString(""));
    write(map.value("trashMailbox").toString(), QString(""));
    write(map.value("draftsMailbox").toString(), QString(""));
    write(map.value("autoExpunge").toString(), false);
    write(map.value("markRead").toString(), true);
    write(map.value("moveToTrash").toString(), false);
    write(map.value("preserveFlagged").toString(), false);
    write(map.value("saveToImap").toString(), false);
    write(map.value("quoteOriginalMessage").toString(), true);
    write(map.value("replyQuotePostion").toString(), QString("before"));
    write(map.value("includeSignature").toString(), QString("always"));
    write(map.value("allowRetries").toString(), false);
    write(map.value("includeCC").toString(), false);
    write(map.value("cc").toString(), QString(""));
    write(map.value("includeBCC").toString(), false);
    write(map.value("bcc").toString(), QString(""));;
    write(map.value("saveToDrafts").toString(), false);
    write(map.value("saveLocal").toString(), true);
    write(map.value("saveDraftInterval").toString(), 1000);
    write(map.value("archiveEnabled").toString(), false);
    write(map.value("archiveFolder").toString(),QString(""));
    write(map.value("archiveFormat").toString(), QString("monthly"));
    write(map.value("keepMboxStructure").toString(), false);
}

void MailboxSettings::removeAccount()
{
    if (m_account.isNull()) {
        return;
    }
    setPath(m_account.data()->accountId());
    undefine();
}

QString MailboxSettings::seperator() const
{
    if (!m_account || m_account->accountId().isEmpty()) {
        return QString();
    }
    // The mailbox seperator is included with the LIST response for every mailbox so we just grab the first
    // index in the list.
    QModelIndex index = static_cast<Imap::Mailbox::MailboxModel *>(m_account->mailboxModel())->index(0, 0);
    if (index.isValid())
      return index.data(Imap::Mailbox::RoleMailboxSeparator).toString();

    return QString();
}

void MailboxSettings::determineGlobalOrOverride()
{
    if (m_account.isNull()) {
        return;
    }
    const QString globalPath = QString("%1.global").arg(m_account.data()->accountId());
    QString overridePath = QString("override.");
    if (!m_mboxName.isEmpty()) {
        QString s = seperator();
        if (s != QString(".")) {
            QStringList p = m_mboxName.split(s);
            m_mboxPath = p.join(QLatin1String("."));
        } else {
            m_mboxPath = m_mboxName;
        }
        auto hasKey = [=](const QString &path)->bool {
            QJsonValue val = data();
            Q_FOREACH(const QString &key, path.split(QLatin1Char('.'))) {
                QJsonObject object = val.toObject();
                if (object.isEmpty() || (val = object.value(key)).isUndefined()) {
                    return false;
                }
            }
            return true;
        };
        if (hasKey(overridePath.append(m_mboxPath))) {
            m_hasOverride = true;
            setPath(QString("%1.%2").arg(m_account.data()->accountId()).arg(overridePath));
        } else {
            m_hasOverride = false;
            setPath(globalPath);
        }
    } else {
        m_hasOverride = false;
        setPath(m_account.data()->accountId());
    }
}
}
}
