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
#include "Imap/Network/MsgPartNetAccessManager.h"

#include <QTemporaryFile>

#include "../Models/MessageModel.h"


namespace Dekko {

namespace Utils {

class AttachmentDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Dekko::Models::MessageModel *messageModel WRITE setMessageModel)
    Q_PROPERTY(Imap::Network::MsgPartNetAccessManager *msgPartNAM WRITE setMsgPartNAM)
public:
    explicit AttachmentDownloader(QObject *parent = 0);
    Q_INVOKABLE void fetchPart(QString partId);

    void setMessageModel(Dekko::Models::MessageModel *messageModel);
    void setMsgPartNAM(Imap::Network::MsgPartNetAccessManager *nam);
    Q_INVOKABLE void cleanTmpFile();

public slots:
    void slotFileNameRequestedOnOpen(QString * fileName);
    void onOpenFailed(QString);
    void openDownloadedAttachment();

signals:
    void downloadStart();
    void downloadProgress(qint64 downloaded, qint64 total);
    void downloadComplete(QString url);
    void downloadFailed();

private:
    Dekko::Models::MessageModel *m_messageModel;
    Imap::Network::MsgPartNetAccessManager *m_manager;
    QTemporaryFile *m_tmpFile;
};

}
}
