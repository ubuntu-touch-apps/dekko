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

#include "ComposerUtils.h"
#include <QDebug>
#include "Composer/PlainTextFormatter.h"
#include "Composer/SubjectMangling.h"

namespace Dekko
{

ComposerUtils::ComposerUtils(QObject *parent) :
    QObject(parent), m_fromName(QString()), m_message(QString()), m_subject(QString()), m_replyMode(0), m_quoteOriginal(false)
{
}

QString ComposerUtils::fromName()
{
    return m_fromName;
}

void ComposerUtils::setFromName(QString &fromName)
{
    if (fromName == m_fromName) {
        return;
    }
    m_fromName = fromName;
    emit fromNameChanged();
}

QDateTime ComposerUtils::date()
{
    return m_date;
}

void ComposerUtils::setDate(QDateTime &date)
{
    m_date = date;
    emit dateChanged();
}

QString ComposerUtils::message()
{
    if (!m_quoteOriginal) {
        return QString();
    }
    QString sender;
    if (!m_fromName.isEmpty()) {
        sender = m_fromName;
    } else {
        // TRANSLATORS: This is you as in "On Monday you wrote"
        sender = tr("you");
    }
    QString replyPart =  tr("On %1, %2 wrote:\n").arg(m_date.toLocalTime().toString(Qt::SystemLocaleLongDate)).arg(sender);
    QString quotedPart = replyPart + QStringList(Composer::Util::quoteText(m_message.split("\n"))).join("\n");
    if (m_replyPosition == QString("before")) {
        quotedPart.prepend(QString("\n\n"));
    } else if (m_replyPosition == QString("after")) {
        quotedPart.append(QString("\n\n"));
    } else {
        // Do nothing
    }
    return quotedPart;
}

void ComposerUtils::setMessage(QString &message)
{
    if (message == m_message) {
        return;
    }
    m_message = message;
    messageChanged();
}

QString ComposerUtils::subject()
{
    if (m_replyMode == ReplyMode::REPLY_FORWARD)
        return Composer::Util::forwardSubject(m_subject);
    else
        return Composer::Util::replySubject(m_subject);
}

void ComposerUtils::setSubject(QString &subject)
{
    if (subject == m_subject) {
        return;
    }
    m_subject = subject;
    subjectChanged();
}

QString ComposerUtils::plaintTextToHtml(const QString &message)
{
    QString m = Composer::Util::plainTextToHtml(message, Composer::Util::FORMAT_PLAIN);
    return m.replace(QLatin1Char('\n'), QLatin1String("<br />"));
}
}
