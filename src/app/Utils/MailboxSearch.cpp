/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
#include "MailboxSearch.h"
#include <QDebug>

namespace Dekko
{
namespace Utils
{

MailboxSearch::MailboxSearch(QObject *parent) :
    QObject(parent), m_model(0), m_searchSender(false), m_searchSubject(false),
    m_searchBody(false), m_searchRecipients(false)
{
    connect(this, SIGNAL(searchStringChanged()), this, SLOT(requestSearch()));
}

QObject *MailboxSearch::model()
{
    return m_model;
}

void MailboxSearch::setModel(QObject *model)
{
    if (!model) {
        return;
    }
    m_model = qobject_cast<Imap::Mailbox::ThreadingMsgListModel *>(model);
    Q_ASSERT(m_model);
    emit modelChanged();
}

bool MailboxSearch::searchSender() const
{
    return m_searchSender;
}

void MailboxSearch::setSearchSender(const bool set)
{
    m_searchSender = set;
    emit searchFiltersChanged();
}

bool MailboxSearch::searchSubject() const
{
    return m_searchSubject;
}

void MailboxSearch::setSearchSubject(const bool set)
{
    m_searchSubject = set;
    emit searchFiltersChanged();
}

bool MailboxSearch::searchBody() const
{
    return m_searchBody;
}

void MailboxSearch::setSearchBody(const bool set)
{
    m_searchBody = set;
    emit searchFiltersChanged();
}

bool MailboxSearch::searchRecipients() const
{
    return m_searchRecipients;
}

void MailboxSearch::setSearchRecipients(const bool set)
{
    m_searchRecipients = set;
    emit searchFiltersChanged();
}

QString MailboxSearch::searchString() const
{
    return m_searchText;
}

void MailboxSearch::setSearchString(const QString &searchText)
{
    m_searchText = searchText;
    emit searchStringChanged();
}

void MailboxSearch::requestSearch()
{
    m_model->setUserSearchingSortingPreference(searchCriteria(), m_model->currentSortCriterium(), m_model->currentSortOrder());
}

QStringList MailboxSearch::searchCriteria() const
{
    if (m_searchText.isEmpty()) {
        return QStringList();
    }
    QStringList imapKeys;
    if (m_searchSender) {
        imapKeys << QLatin1String("FROM");
    }
    if (m_searchSubject) {
        imapKeys << QLatin1String("SUBJECT");
    }
    if (m_searchRecipients) {
        imapKeys << QLatin1String("TO") << QLatin1String("CC") << QLatin1String("BCC");
    }
    if (m_searchBody) {
        imapKeys << QLatin1String("BODY");
    }

    if (imapKeys.isEmpty()) {
        return imapKeys;
    }

    QStringList searchConditions;
    Q_FOREACH(const QString &key, imapKeys) {
        searchConditions << key << m_searchText;
    }
    if (imapKeys.size() > 1) {
        int num = imapKeys.size() - 1;
        for (int i = 0; i < num; ++i) {
            searchConditions.prepend(QLatin1String("OR"));
        }
    }
    qDebug() << "SearchConditions" << searchConditions;
    return searchConditions;
}
}
}
