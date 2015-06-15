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
#ifndef TABEVENTSIGNALER_H
#define TABEVENTSIGNALER_H

#include <QObject>
#include <QtQuick>
#include <QQmlEngine>
#include <QJSEngine>
#include <QtGui/QGuiApplication>

namespace Dekko
{
namespace Utils
{

class TabEventSignaler : public QObject
{
    Q_OBJECT
public:
    explicit TabEventSignaler(QObject *parent = 0);

    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

signals:

public slots:
    static void sendTabEvent();

};
}
}

#endif // TABEVENTSIGNALER_H
