/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

   This file is part of Dekko mail client

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "AttachmentDownloader.h"
#include <QDir>
#include "Imap/Network/FileDownloadManager.h"

namespace Dekko {

namespace Utils {

AttachmentDownloader::AttachmentDownloader(QObject *parent)
    : QObject(parent), m_tmpFile(0)
{

}

void AttachmentDownloader::fetchPart(QString partId)
{
    QModelIndex partIndex = m_messageModel->getModelIndexFromPartId(partId);
    Imap::Network::FileDownloadManager *fileDownloadManager =
        new Imap::Network::FileDownloadManager(this, m_manager, partIndex);
    connect(fileDownloadManager, SIGNAL(fileNameRequested(QString*)), this, SLOT(slotFileNameRequestedOnOpen(QString*)));
    connect(fileDownloadManager, SIGNAL(transferError(QString)), this, SLOT(onOpenFailed(QString)));
    // we aren't connecting to cancelled() as it cannot really happen -- the filename is never empty
    connect(fileDownloadManager, SIGNAL(succeeded()), this, SLOT(openDownloadedAttachment()));

    connect(fileDownloadManager, SIGNAL(succeeded()), fileDownloadManager, SLOT(deleteLater()));
    connect(fileDownloadManager, SIGNAL(transferError(QString)), fileDownloadManager, SLOT(deleteLater()));

    connect(fileDownloadManager, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgress(qint64,qint64)));
    fileDownloadManager->downloadPart();
}

void AttachmentDownloader::setMessageModel(Models::MessageModel *messageModel)
{
    m_messageModel = messageModel;
}

void AttachmentDownloader::setMsgPartNAM(Imap::Network::MsgPartNetAccessManager *nam)
{
    m_manager = nam;
}

void AttachmentDownloader::cleanTmpFile()
{
    delete m_tmpFile;
    m_tmpFile = 0;
}

void AttachmentDownloader::slotFileNameRequestedOnOpen(QString * fileName)
{
    Q_ASSERT(!m_tmpFile);
    m_tmpFile = new QTemporaryFile(QDir::tempPath() +
                                   QLatin1String("/trojita-attachment-XXXXXX-") +
                                   fileName->replace(QLatin1Char('/'), QLatin1Char('_')));
    m_tmpFile->open();
    *fileName = m_tmpFile->fileName();
}

void AttachmentDownloader::onOpenFailed(QString)
{
    cleanTmpFile();
    emit downloadFailed();
}

void AttachmentDownloader::openDownloadedAttachment()
{
    Q_ASSERT(m_tmpFile);
    m_tmpFile->setPermissions(QFile::ReadOwner);
    emit downloadComplete(QUrl::fromLocalFile(m_tmpFile->fileName()).toString());
}

}
}
