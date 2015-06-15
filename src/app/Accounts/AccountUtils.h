/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the Dekko

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

#ifndef ACCOUNTUTILS_H
#define ACCOUNTUTILS_H

#include <QObject>
#include "Imap/Model/Utils.h"
#include "Account.h"

namespace Dekko {
namespace Accounts {

/*!
 \brief

 \class AccountUtils AccountUtils.h "src/app/Accounts/AccountUtils.h"
*/
class AccountUtils : public QObject
{
    Q_OBJECT
public:
    /*!
     \brief

     \fn AccountUtils
     \param parent
    */
    explicit AccountUtils(QObject *parent = 0);

    /*!
     \brief

     \fn getDomainIcon
     \param emailAddress
     \return QString
    */
    static QString getDomainIcon(const QString &emailAddress);
    /*!
     \brief

     \fn findDomainConfFile
     \return QString
    */
    static QString findDomainConfFile();
    /*!
     \brief

     \fn nukeCache
     \param accountName
    */
    static void nukeCache(QString &accountName);
    /*!
     \brief

     \fn checkSettingsFilePermissions
     \param s
    */
    static void checkSettingsFilePermissions(QSettings *s);
signals:

public slots:

private:


};

}
}

#endif // ACCOUNTUTILS_H
