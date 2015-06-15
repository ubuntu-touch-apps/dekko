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
#include <QCoreApplication>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QFile>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        qFatal("Usage: %s infile outfile", argv[0]);
    }

    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();

    QFile infile(QString(args.at(1)));
    infile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString inVal = infile.readAll();
    infile.close();
    QFile outfile;
    outfile.setFileName(QString(args.at(2)));
    outfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    QTextStream(&outfile) << inVal;
    outfile.close();

    QTimer::singleShot(500, &app, SLOT(quit()));

    return app.exec();
}
