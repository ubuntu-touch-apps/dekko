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
#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QSettings>
#include <QtQuick>
#include <QQmlEngine>
#include <QJSEngine>
#include <QtGui/QGuiApplication>
#include "Styles.h"

namespace Dekko
{
namespace Themes
{
class Styles;

class Theme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *common READ common NOTIFY themeChanged)
    Q_PROPERTY(QObject *header READ header NOTIFY themeChanged)
    Q_PROPERTY(QObject *actions READ actions NOTIFY themeChanged)
    Q_PROPERTY(QObject *avatar READ avatar NOTIFY themeChanged)
    Q_PROPERTY(QObject *colors READ colors NOTIFY themeChanged)
    Q_PROPERTY(QObject *buttons READ buttons NOTIFY themeChanged)
    Q_PROPERTY(QString currentTheme READ currentTheme NOTIFY themeChanged)
//    Q_PROPERTY(QStringList installedThemes READ installedThemes NOTIFY installedThemesChanged)

public:
    explicit Theme(QObject *parent = 0);

    QObject *common() const;
    QObject *header() const;
    QObject *actions() const;
    QObject *avatar() const;
    QObject *colors() const;
    QObject *buttons() const;

    QString currentTheme() const;
//    void setCurrentTheme(const QString &themeName);

//    QStringList installedThemes() const; // List of installed themes

    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

signals:
    void themeChanged();
//    void installedThemesChanged();

private:
    Themes::Common *m_common;
    Themes::Actions * m_actions;
    Themes::Avatar *m_avatar;
    Themes::Colors *m_colors;
    Themes::Buttons *m_buttons;
    Themes::Header *m_header;
    QString m_theme;

};
}
}
#endif // THEME_H
