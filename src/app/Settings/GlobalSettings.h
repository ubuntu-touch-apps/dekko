#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QtQuick>
#include <QQmlEngine>
#include <QJSEngine>
#include <QtGui/QGuiApplication>
#include "Settings.h"

namespace Dekko
{
namespace Settings
{
class OfflineSettings;
class Preferences;
class DeveloperSettings;

class GlobalSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *preferences READ preferences CONSTANT)
    Q_PROPERTY(QObject *offline READ offline CONSTANT)
    Q_PROPERTY(QObject *developer READ developer CONSTANT)

public:
    explicit GlobalSettings(QObject *parent = 0);

    QObject *offline() const;
    QObject *preferences() const;
    QObject *developer() const;

    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    QPointer<OfflineSettings> m_offline;
    QPointer<Preferences> m_preferences;
    QPointer<DeveloperSettings> m_developer;
};
}
}

#endif // GLOBALSETTINGS_H
