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
#include "Theme.h"
#include <QtCore/QLibraryInfo>
#include <QtCore/QStandardPaths>
#include <QtGui/QGuiApplication>
namespace Dekko
{
namespace Themes
{

QString findThemeFile() {
    QString configFile;
    // TODO: make this configurable
    const QString filePath = QLatin1String("configuration/styles.conf");
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    paths.prepend(QDir::currentPath());
    paths.prepend(QCoreApplication::applicationDirPath());
    Q_FOREACH (const QString &path, paths) {
        QString myPath = path + QLatin1Char('/') + filePath;
        if (QFile::exists(myPath)) {
            configFile = myPath;
            break;
        }
    }
    if (!configFile.isEmpty()) {
        qDebug() << "Theme file found";
        return configFile;
    }
    qDebug() <<" Theme file not found";
    return QString();
}

Theme::Theme(QObject *parent) :
    QObject(parent)
{
    m_theme = findThemeFile();
    if (m_theme.isEmpty()) {
        qFatal("No theme file found :-/");
    }
    m_common = new Themes::Common(this, m_theme);
    m_actions = new Themes::Actions(this, m_theme);
    m_avatar = new Themes::Avatar(this, m_theme);
    m_colors = new Themes::Colors(this, m_theme);
    m_buttons = new Themes::Buttons(this, m_theme);
    m_header = new Themes::Header(this, m_theme);
    emit themeChanged();
}

QObject *Theme::common() const
{
    return m_common;
}

QObject *Theme::header() const
{
    return m_header;
}

QObject *Theme::actions() const
{
    return m_actions;
}

QObject *Theme::avatar() const
{
    return m_avatar;
}

QObject *Theme::colors() const
{
    return m_colors;
}

QObject *Theme::buttons() const
{
    return m_buttons;
}

QString Theme::currentTheme() const
{
    return m_theme;
}

QObject *Theme::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    Theme *theme = new Theme(reinterpret_cast<QObject*>(engine));
    return theme;
}
}
}
