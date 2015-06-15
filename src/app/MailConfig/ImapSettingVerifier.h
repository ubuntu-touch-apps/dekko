/* Copyright (C) 2015 Boren Zhang <bobo1993324@gmail.com>

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

#ifndef USERNAMEPASSWORDCHECK_H
#define USERNAMEPASSWORDCHECK_H

#include <QObject>
#include <memory>
#include <QSslCertificate>
#include <QSslError>
#include <QAbstractItemModel>
#include "Common/Logging.h"

namespace Dekko {
namespace Settings {
class ImapSettings;
}
}

namespace Streams {
class SocketFactory;
}

namespace Imap {
namespace Mailbox {
class Model;
class DummyNetworkWatcher;
class AbstractCache;
typedef std::unique_ptr<Streams::SocketFactory> SocketFactoryPtr;

}
}

namespace Common {
class LogMessage;
}

namespace Dekko {

class ImapSettingVerifier : public QObject
{
    Q_OBJECT

    Q_ENUMS(ResultStatus)
    Q_PROPERTY(QString username MEMBER m_username)
    Q_PROPERTY(QString password MEMBER m_password)
    Q_PROPERTY(Dekko::Settings::ImapSettings *_imapSettings MEMBER m_imapSettings)
    Q_PROPERTY(int result READ result WRITE setResult NOTIFY resultChanged)
    Q_PROPERTY(QString errorMessage MEMBER m_errorMessage)
    Q_PROPERTY(QAbstractItemModel * imapModel READ imapModel NOTIFY imapModelChanged)
public:

    typedef enum {
        IMAP_SETTINGS_CONNECT_UNKNOWN = 0,
        IMAP_SETTINGS_CONNECT_RUNNING,
        IMAP_SETTINGS_CONNECT_SUCCEED,
        IMAP_SETTINGS_CONNECT_FAILED
    } ResultStatus;

    explicit ImapSettingVerifier(QObject *parent = 0);

    Q_INVOKABLE void start();

    int result();
    void setResult(int r);
    QAbstractItemModel *imapModel();

signals:
    void resultChanged();
    void imapModelChanged();

public slots:
    void onLogged(uint i, Common::LogMessage message);
    void imapSslError(const QList<QSslCertificate> &sslCertificateChain, const QList<QSslError> &sslErrors);
    void onAuthRequested();
    void onAuthFailed(QString error);
    void onAuthSucceeded();
    void onNetworkError(QString error);

private:
    QString m_username;
    QString m_password;
    Dekko::Settings::ImapSettings *m_imapSettings;
    Imap::Mailbox::Model *m_imapModel;
    bool m_authRequested;
    ResultStatus m_result;
    Imap::Mailbox::DummyNetworkWatcher *m_netWatcher;
    Imap::Mailbox::AbstractCache *m_cache;
    QString m_errorMessage;

    void deleteImapModel();
};

}
#endif // USERNAMEPASSWORDCHECK_H
