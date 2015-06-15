#ifndef NOTIFICATIONSETTINGS_H
#define NOTIFICATIONSETTINGS_H

#include <QObject>
#include <QPointer>
#include "SettingsObjectBase.h"
#include "SettingsFileBase.h"

namespace Dekko
{
namespace  Settings
{
class NotificationSettings : public SettingsObjectBase
{
    Q_OBJECT
public:
    explicit NotificationSettings(QObject *parent = 0);

    static const QVariantMap keys();
    static SettingsFileBase *settingsFile();
    static void setSettingsFile(SettingsFileBase *file);
    Q_INVOKABLE void createDefaultsIfNotExist();
    Q_INVOKABLE QVariant value(const QString &value);
signals:

public slots:

};
}
}
#endif // NOTIFICATIONSETTINGS_H
