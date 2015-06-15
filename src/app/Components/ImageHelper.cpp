#include "ImageHelper.h"
#include <QCoreApplication>
#include <QUrl>
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <QCryptographicHash>
#include <QDebug>
#include <QImage>
#include <QStringBuilder>
#include <QDir>
#include <QResource>
#include <QtConcurrent/QtConcurrentRun>
#include <QQmlNetworkAccessManagerFactory>
#include <QFuture>
#include <QtQuick>
#include "app/Utils/Path.h"

extern QQuickView *qQuickViewer;
namespace Dekko
{
namespace Components
{
QString ImageHelper::s_basePath;
QString ImageHelper::s_cachePath;

ImageHelper::ImageHelper(QObject *parent) :
    QObject(parent),m_size(0), m_verticalRatio(1.0), m_horizontalRatio(1.0),
    m_color(Qt::transparent), m_nam(0), m_ready(false), m_requestSent(false), m_alreadySeen(false)
{
    connect(this, SIGNAL(refresh()), this, SLOT(handleRefresh()));

    if (s_basePath.isEmpty()) {
        // Our default base path is loading from qrc files. This can be overidden to point
        // to any allowed path.
        ImageHelper::s_basePath = QStringLiteral(":/actions");
    }
    if (s_cachePath.isEmpty()) {
        ImageHelper::s_cachePath = (Dekko::cacheLocation() % "/.ImageCache");
    }

    m_nam = new QNetworkAccessManager(this);
    connect(this, &ImageHelper::gravatarEmailChanged, [this]() {setGravatarUrl(m_gravatarUrl);});
    connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    connect(&m_watcher, SIGNAL(finished()), this, SLOT(writeUrlToQmlProperty()));
}

ImageHelper::~ImageHelper()
{
    delete m_nam;
    m_nam = 0;
}

void ImageHelper::setBasePath(const QString &basePath)
{
    ImageHelper::s_basePath = basePath;
}

void ImageHelper::setCachePath(const QString &cachePath)
{
    ImageHelper::s_cachePath = cachePath;
}

void ImageHelper::classBegin()
{
    m_ready = false;
}

void ImageHelper::componentComplete()
{
    m_ready = true;
    emit refresh();
}

void ImageHelper::setTarget(const QQmlProperty &target)
{
    m_property = target;
    emit refresh();
}

QUrl ImageHelper::gravatarUrl() const
{
    return m_gravatarUrl;
}

void ImageHelper::setGravatarUrl(const QUrl &gravurl)
{
    if (gravurl.isEmpty() || !gravurl.isValid()) {
        return;
    }
    if (m_gravatarEmail.isEmpty()) {
        return;
    }
    if (m_size == 0 || m_alreadySeen) {
        return;
    }
    m_gravatarUrl = gravurl;
    m_alreadySeen = true;
    QString avatarCache(s_cachePath % "/" % m_gravatarEmail % QString::number(m_size) % ".png");
    if (QFile::exists(avatarCache)) {
        if (m_property.isValid() && m_property.isWritable()) {
            m_property.write(QUrl::fromLocalFile(avatarCache));
        }
    } else {
        if (!m_requestSent) {
            QNetworkRequest request(m_gravatarUrl);
            m_nam->get(request);
            m_requestSent = true;
        }
    }
}

void ImageHelper::handleRefresh()
{
    if (m_ready) {
        if (!m_icon.isEmpty () && m_size > 0 && m_horizontalRatio > 0.0 && m_verticalRatio > 0.0) {
            // So this is quite an intensive task so we move it to a seperate thread to not starve the gui
            // benchmarking shows this greatly increases our load times in the message delegate, as this is a completely
            // asynchronous job and we actually only do the converting to png the once and just return the cached
            // version on each subsequent request.
            SvgToPngWorker *worker = new SvgToPngWorker(this, ImageHelper::s_basePath, ImageHelper::s_cachePath);
            m_future = QtConcurrent::run(worker, &SvgToPngWorker::convertSvgToPng,
                                                     m_size, m_horizontalRatio, m_verticalRatio,
                                                     m_color, m_icon);
            m_watcher.setFuture(m_future);
        }
    }
}

void ImageHelper::finished(QNetworkReply *reply)
{
    QString avatarCache(s_cachePath % "/" % m_gravatarEmail % QString::number(m_size) % ".png");
    if (reply->error() == QNetworkReply::NoError) {
        GravatarWorker *worker = new GravatarWorker(this);
        m_future = QtConcurrent::run(worker, &GravatarWorker::saveGravatar, reply->readAll(), avatarCache);
        m_watcher.setFuture(m_future);
    }
    // let's just ignore anything other than a NoError as we don't need to handle it.
    reply->deleteLater();
    return;
}

void ImageHelper::writeUrlToQmlProperty()
{
    QUrl url = m_future.result();
    if (!url.isValid() || url.isEmpty()) {
        return;
    }
    if (m_property.isValid() && m_property.isWritable()) {
        m_property.write(url);
    }
}

SvgToPngWorker::SvgToPngWorker(QObject *parent, QString &basePath, QString &cachePath) :
    QObject(parent), m_basePath(basePath), m_cachePath(cachePath)
{

}

QUrl SvgToPngWorker::convertSvgToPng(const int &size, const qreal &h, const qreal &v, const QColor &color, const QString &icon)
{
    QUrl url;
    if (!icon.isEmpty () && size > 0 && h > 0.0 && v > 0.0) {
        QImage image (size * h, size * v, QImage::Format_ARGB32);
        QString uri (icon
                     % "?color="  % (color.isValid() ? color.name() : "none")
                     % "&width="  % QString::number (image.width())
                     % "&height=" % QString::number (image.height()));
        QString hash (QCryptographicHash::hash(uri.toLocal8Bit(), QCryptographicHash::Md5).toHex());
        QString sourcePath(m_basePath  % "/" % icon % ".svg");
        QString cachedPath(m_cachePath % "/" % hash % ".png");
        if(!QFile::exists(cachedPath)) {
            QPainter painter(&image);
            image.fill(Qt::transparent);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            QSvgRenderer renderer;
            if (QFile::exists(sourcePath)) {
                renderer.load(sourcePath);
                if (renderer.isValid()) {
                    renderer.render(&painter);
                    if (color.isValid() && color.alpha() > 0) {
                        QColor tmp (color);
                        for (int x = 0; x < image.width(); x++) {
                            for (int y = 0; y < image.height(); y++) {
                                tmp.setAlpha(qAlpha (image.pixel(x, y)));
                                image.setPixel(x, y, tmp.rgba());
                            }
                        }
                    }
                    QDir().mkpath(m_cachePath);
                    image.save(cachedPath, "PNG", 0);
                    url = QUrl::fromLocalFile(cachedPath);
                }
            }
            else {
                qWarning() << ">>> SvgToPngWorker: Can't render" << sourcePath << ", no such file!";
            }
        }
        else {
            url = QUrl::fromLocalFile(cachedPath);
        }
    }
    return url;
}

GravatarWorker::GravatarWorker(QObject *parent) : QObject(parent)
{
}

QUrl GravatarWorker::saveGravatar(const QByteArray &reply, const QString &path)
{
    QImage *img = new QImage();
    img->loadFromData(reply, 0);
    if (img->isNull()) {
        qDebug() << ">>> GravatarWorker: Failed loading gravatar from reply";
        return QUrl();
    }
    if (img->save(path, "PNG", 0)) {
        return QUrl::fromLocalFile(path);
    }
    return QUrl();
}

}
}

