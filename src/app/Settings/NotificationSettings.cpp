#include "NotificationSettings.h"
#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QVariantMap>
#include <QJsonArray>

namespace Dekko
{
namespace  Settings
{

NotificationSettings::NotificationSettings(QObject *parent) :
    SettingsObjectBase(parent)
{
    setFilePath(settingsFile());
    if (m_file) {
        setPath(QString("default"));
        createDefaultsIfNotExist();
    }
}

const QVariantMap NotificationSettings::keys()
{
    QVariantMap m_keys;
    m_keys["card"] = QLatin1String("card");
    m_keys["show_popup"] = QLatin1String("card.show_popup");
    m_keys["persist"] = QLatin1String("card.persist");
    m_keys["sound"] = QLatin1String("sound");
    m_keys["play_sound"] = QLatin1String("sound.play_sound");
    m_keys["sound_file"] = QLatin1String("sound.sound_file");
    m_keys["vibrate"] = QLatin1String("vibrate");
    m_keys["vibrate_pattern"] = QLatin1String("vibrate.pattern");
    m_keys["vibrate_duration"] = QLatin1String("vibrate.duration");
    m_keys["repeat_vibration"] = QLatin1String("vibrate.repeat");
    m_keys["push_token"] = QLatin1String("push_token");
    m_keys["defaults_created"] = QLatin1String("defaults_created");
    return m_keys;
}
static QPointer<SettingsFileBase> notificationsDefaultFile;
SettingsFileBase *NotificationSettings::settingsFile()
{
    return notificationsDefaultFile;
}

void NotificationSettings::setSettingsFile(SettingsFileBase *file)
{
    notificationsDefaultFile = file;
}

static inline QString soundFilePath(const QString &soundFile) {
    QString path;
    if (path.isEmpty()) {
        QString soundsPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QLatin1String("../../../sounds"));
        QDir soundRoot = QFileInfo(soundsPath).absoluteDir();
        QString myPath = soundRoot.absolutePath() + QLatin1String("/sounds/") + soundFile;
        if (QFile::exists(myPath)) {
            path = myPath;
        }
    }
    return path;
}

void NotificationSettings::createDefaultsIfNotExist()
{
    if (read(QLatin1String("defaults_created")).toBool()) {
        return;
    }
    write(QLatin1String("card.show_popup"), true);
    write(QLatin1String("card.persist"), true);
    write(QLatin1String("sound.play_sound"), true);
    write(QLatin1String("sound.sound_file"), soundFilePath(QStringLiteral("blop.mp3")));
    write(QLatin1String("vibrate.pattern"), QJsonArray() << 200 << 100);
    write(QLatin1String("vibrate.duration"), 300);
    write(QLatin1String("vibrate.repeat"), 3);
    write(QLatin1String("push_token"), QString());
    write(QLatin1String("defaults_created"), true);
}

QVariant NotificationSettings::value(const QString &value)
{
    return read(keys().value(value).toString()).toVariant();
}
}
}
