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

#ifndef COMPOSERUTILS_H
#define COMPOSERUTILS_H

#include <QObject>
#include <QtCore>
#include "Composer/PlainTextFormatter.h"
#include "Composer/SubjectMangling.h"
#include "ReplyMode.h"

namespace Dekko
{

class ComposerUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fromName READ fromName WRITE setFromName NOTIFY fromNameChanged)
    Q_PROPERTY(QDateTime date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)
    Q_PROPERTY(bool quoteOriginalMessage MEMBER m_quoteOriginal)
    Q_PROPERTY(QString replyPosition MEMBER m_replyPosition)
    Q_PROPERTY(int replyMode MEMBER m_replyMode)
public:
    explicit ComposerUtils(QObject *parent = 0);

    QString fromName();
    void setFromName(QString &fromName);
    QDateTime date();
    void setDate(QDateTime &date);
    QString message();
    void setMessage(QString &message);
    QString subject();
    void setSubject(QString &subject);

    Q_INVOKABLE QString plaintTextToHtml(const QString &message);

signals:
    void messageChanged();
    void subjectChanged();
    void fromNameChanged();
    void dateChanged();

public slots:

private:
    QString m_fromName;
    QDateTime m_date;
    QString m_message;
    QString m_subject;
    int m_replyMode; // ReplyMode::Mode
    bool m_quoteOriginal;
    QString m_replyPosition;

};
}
#endif // COMPOSERUTILS_H
