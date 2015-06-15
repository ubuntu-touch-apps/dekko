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
#ifndef STYLES_H
#define STYLES_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QStringList>

namespace Dekko
{
namespace Themes
{
static QColor getColorForObject(QSettings *settings, const QString &object) {
    return QColor(settings->value(object).toString());
}
class AbstractStyle : public QObject
{
    Q_OBJECT
public:
    AbstractStyle(QObject *parent, const QString &themeFile) :
        QObject(parent), m_settings(new QSettings(themeFile, QSettings::NativeFormat))
    {
    }
protected:
    QSettings *m_settings;
};

class Common : public AbstractStyle
{
    Q_OBJECT
public:
    Common(QObject *parent, const QString &themeFile) :
        AbstractStyle(parent, themeFile)
    {
        m_settings->beginGroup(QLatin1String("common"));
    }
    Q_PROPERTY(QColor background READ background CONSTANT)
    QColor background() const {
        return Themes::getColorForObject(m_settings, QLatin1String("background"));
    }
    Q_PROPERTY(QColor divider READ divider CONSTANT)
    QColor divider() const {
        return Themes::getColorForObject(m_settings, QLatin1String("divider"));
    }
    Q_PROPERTY(QColor text READ text CONSTANT)
    QColor text() const {
        return Themes::getColorForObject(m_settings, QLatin1String("text"));
    }
    Q_PROPERTY(QColor textSelected READ textSelected CONSTANT)
    QColor textSelected() const {
        return Themes::getColorForObject(m_settings, QLatin1String("textSelected"));
    }
    Q_PROPERTY(QColor drawerBackground READ drawerBackground CONSTANT)
    QColor drawerBackground() const {
        return Themes::getColorForObject(m_settings, QLatin1String("drawerBackground"));
    }
    Q_PROPERTY(QColor section READ section CONSTANT)
    QColor section() const {
        return Themes::getColorForObject(m_settings, QLatin1String("section"));
    }
};

class Header : public AbstractStyle
{
    Q_OBJECT
public:
    Header(QObject *parent, const QString &themeFile) :
        AbstractStyle(parent, themeFile)
    {
        m_settings->beginGroup(QLatin1String("header"));
    }
    Q_PROPERTY(QColor background READ background CONSTANT)
    QColor background() const {
        return Themes::getColorForObject(m_settings, QLatin1String("background"));
    }
    Q_PROPERTY(QColor title READ title CONSTANT)
    QColor title() const {
        return Themes::getColorForObject(m_settings, QLatin1String("title"));
    }
    Q_PROPERTY(QColor textSelected READ textSelected CONSTANT)
    QColor textSelected() const {
        return Themes::getColorForObject(m_settings, QLatin1String("textSelected"));
    }
    Q_PROPERTY(QColor text READ text CONSTANT)
    QColor text() const {
        return Themes::getColorForObject(m_settings, QLatin1String("text"));
    }
    Q_PROPERTY(QColor icons READ icons CONSTANT)
    QColor icons() const {
        return Themes::getColorForObject(m_settings, QLatin1String("icons"));
    }
};

class Actions : public AbstractStyle
{
    Q_OBJECT
public:
    Actions(QObject *parent, const QString &themeFile) :
        AbstractStyle(parent, themeFile)
    {
        m_settings->beginGroup(QLatin1String("actions"));
    }
    Q_PROPERTY(QColor important READ important CONSTANT)
    QColor important() const {
        return Themes::getColorForObject(m_settings, QLatin1String("important"));
    }
    Q_PROPERTY(QColor starred READ starred CONSTANT)
    QColor starred() const {
        return Themes::getColorForObject(m_settings, QLatin1String("starred"));
    }

};
class Avatar : public AbstractStyle
{
    Q_OBJECT
public:
    Avatar(QObject *parent, const QString &themeFile) :
        AbstractStyle(parent, themeFile)
    {
        m_settings->beginGroup(QLatin1String("avatar"));
    }
    Q_PROPERTY(QColor yellow READ yellow CONSTANT)
    QColor yellow() const {
        return Themes::getColorForObject(m_settings, QLatin1String("yellow"));
    }
    Q_PROPERTY(QColor green READ green CONSTANT)
    QColor green() const {
        return Themes::getColorForObject(m_settings, QLatin1String("green"));
    }
    Q_PROPERTY(QColor blue READ blue CONSTANT)
    QColor blue() const {
        return Themes::getColorForObject(m_settings, QLatin1String("blue"));
    }
    Q_PROPERTY(QColor purple READ purple CONSTANT)
    QColor purple() const {
        return Themes::getColorForObject(m_settings, QLatin1String("purple"));
    }
    Q_PROPERTY(QColor turqoise READ turqoise CONSTANT)
    QColor turqoise() const {
        return Themes::getColorForObject(m_settings, QLatin1String("turqoise"));
    }
    Q_PROPERTY(QColor text READ text CONSTANT)
    QColor text() const {
        return Themes::getColorForObject(m_settings, QLatin1String("text"));
    }
    Q_PROPERTY(QStringList avatarColorList READ avatarColorList CONSTANT)
    QStringList avatarColorList() const {
        QStringList keys = m_settings->allKeys();
        QStringList values;
        keys.removeAll(QLatin1String("text"));
        Q_FOREACH(const QString &key, keys) {
            values.append(m_settings->value(key).toString());
        }
        return values;
    }
    Q_INVOKABLE QColor randomColor() const {
        QStringList keys = m_settings->allKeys();
        keys.removeAll(QLatin1String("text"));
        srand(time(0));
        return Themes::getColorForObject(m_settings, keys[rand()%keys.size()]);
    }
    Q_INVOKABLE QString getInitialsForName(const QString &name) {
        if (name.isEmpty() || !name.at(0).isLetter()) {
            return QString();
        }
        // Intitials string
        QString initials;
        // Now break up the name, we have to set the encoding here as QT_NO_CAST_FROM/TO_ASCII is set
        QStringList parts = name.split(QString::fromLatin1(" "));
        if (parts.first().at(0).isLetter()) {
            initials += parts.first().at(0).toUpper();
        }
        if (parts.size() > 1) {
            if (parts.last().at(0).isLetter()) {
                initials += parts.last().at(0).toUpper();
            }
        }
        return initials;
    }
};
class Colors : public AbstractStyle
{
    Q_OBJECT
public:
    Colors(QObject *parent, const QString &themeFile) :
        AbstractStyle(parent, themeFile)
    {
        m_settings->beginGroup(QLatin1String("colors"));
    }
    Q_PROPERTY(QColor blue READ blue CONSTANT)
    QColor blue() const {
        return  Themes::getColorForObject(m_settings, QLatin1String("blue"));
    }
    Q_PROPERTY(QColor gray READ gray CONSTANT)
    QColor gray() const {
        return Themes::getColorForObject(m_settings, QLatin1String("gray"));
    }
    Q_PROPERTY(QColor green READ green CONSTANT)
    QColor green() const {
        return Themes::getColorForObject(m_settings, QLatin1String("green"));
    }
    Q_PROPERTY(QColor orange READ orange CONSTANT)
    QColor orange() const {
        return Themes::getColorForObject(m_settings, QLatin1String("orange"));
    }
    Q_PROPERTY(QColor red READ red CONSTANT)
    QColor red() const {
        return Themes::getColorForObject(m_settings, QLatin1String("red"));
    }
};
class Buttons : public AbstractStyle
{
    Q_OBJECT
public:
    Buttons(QObject *parent, const QString &themeFile) :
        AbstractStyle(parent, themeFile)
    {
        m_settings->beginGroup(QLatin1String("button"));
    }
    Q_PROPERTY(QColor stroke READ stroke CONSTANT)
    QColor stroke() const {
        return Themes::getColorForObject(m_settings, QLatin1String("strokeColor"));
    }
};
}
}

#endif // STYLES_H
