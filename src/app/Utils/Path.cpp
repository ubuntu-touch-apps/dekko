#include "Path.h"
#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <Common/Application.h>

namespace Dekko {

QString findQmlFile(QString filePath) {
    QString qmlFile;
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    paths.prepend(QDir::currentPath());
    paths.prepend(QCoreApplication::applicationDirPath());
    Q_FOREACH (const QString &path, paths) {
        QString myPath = path + QLatin1Char('/') + filePath;
        if (QFile::exists(myPath)) {
            qmlFile = myPath;
            break;
        }
    }
    if (qmlFile.isEmpty()) {
        // ok not a standard location, lets see if we can find trojita.json, to see if we are a click package
        QString manifestPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QLatin1String("../../dekko.json"));
        if (QFile::exists(manifestPath)) {
            // we are a click so lets tidy up our manifest path and find qmlfile
            QDir clickRoot = QFileInfo(manifestPath).absoluteDir();
            QString myPath = clickRoot.absolutePath() + QLatin1Char('/') + filePath;
            if (QFile::exists(myPath)) {
                qmlFile = myPath;
            }
        }
    }
    // sanity check
    if (qmlFile.isEmpty()) {
        qFatal("File: %s does not exist at any of the standard paths!", qPrintable(filePath));
    }
    return qmlFile;
}

QString configLocation(QString fileName)
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation).append(QString("/%1/%2").arg(Common::Application::organization).arg(fileName));
}

QString cacheLocation()
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation).append(QString("/%1").arg(Common::Application::organization));
}

QString soundFilePath(const QString &soundFile)
{
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

}
