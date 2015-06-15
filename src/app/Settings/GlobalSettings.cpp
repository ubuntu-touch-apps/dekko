#include "GlobalSettings.h"
namespace Dekko
{
namespace Settings
{
GlobalSettings::GlobalSettings(QObject *parent) :
    QObject(parent), m_offline(new OfflineSettings), m_preferences(new Preferences),
    m_developer(new DeveloperSettings)
{
}

QObject *GlobalSettings::offline() const
{
    return m_offline;
}

QObject *GlobalSettings::preferences() const
{
    return m_preferences;
}

QObject *GlobalSettings::developer() const
{
    return m_developer;
}

QObject *GlobalSettings::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    GlobalSettings *gs = new GlobalSettings(reinterpret_cast<QObject*>(engine));
    return gs;
}
}
}
