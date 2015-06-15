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
#ifndef MAILBOXSEARCH_H
#define MAILBOXSEARCH_H

#include <QObject>
#include "Imap/Model/ThreadingMsgListModel.h"

namespace Dekko
{
namespace Utils
{
// This currently is a very simple search and only does a search on sender, recipient subject and body.
// In the future we need to support FUZZY and other complex searches.
// Let's see how we get on with this to start with.
class MailboxSearch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *msgModel READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(bool searchSender READ searchSender WRITE setSearchSender NOTIFY searchFiltersChanged)
    Q_PROPERTY(bool searchSubject READ searchSubject WRITE setSearchSubject NOTIFY searchFiltersChanged)
    Q_PROPERTY(bool searchBody READ searchBody WRITE setSearchBody NOTIFY searchFiltersChanged)
    Q_PROPERTY(bool searchRecipients READ searchRecipients WRITE setSearchRecipients NOTIFY searchFiltersChanged)
    Q_PROPERTY(QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)

public:
    explicit MailboxSearch(QObject *parent = 0);

    QObject *model();
    void setModel(QObject *model);

    bool searchSender() const;
    void setSearchSender(const bool set);
    bool searchSubject() const;
    void setSearchSubject(const bool set);
    bool searchBody() const;
    void setSearchBody(const bool set);
    bool searchRecipients() const;
    void setSearchRecipients(const bool set);

    QString searchString() const;
    void setSearchString(const QString &searchText);


signals:
    void searchFiltersChanged();
    void modelChanged();

    void searchStringChanged();

    void sortFailed();

public slots:

    void requestSearch();

protected:
    QStringList searchCriteria() const;
private:
    Imap::Mailbox::ThreadingMsgListModel *m_model;
    bool m_searchSender;
    bool m_searchSubject;
    bool m_searchBody;
    bool m_searchRecipients;
    QString m_searchText;

};
}
}
#endif // MAILBOXSEARCH_H
