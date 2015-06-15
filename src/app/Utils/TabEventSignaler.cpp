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
#include "TabEventSignaler.h"

extern QQuickView *qQuickViewer;

namespace Dekko
{
namespace Utils
{
TabEventSignaler::TabEventSignaler(QObject *parent) :
    QObject(parent)
{
}

QObject *TabEventSignaler::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    TabEventSignaler *f = new TabEventSignaler(reinterpret_cast<QObject*>(engine));
    return f;
}

void TabEventSignaler::sendTabEvent()
{
    QKeyEvent tabPressEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    QKeyEvent tabReleaseEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier);

    QGuiApplication::sendEvent(qQuickViewer, &tabPressEvent);
    QGuiApplication::sendEvent(qQuickViewer, &tabReleaseEvent);
}
}
}
