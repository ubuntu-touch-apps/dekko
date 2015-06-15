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
#ifndef SENDERIDENTITY_H
#define SENDERIDENTITY_H

#include <QObject>

namespace Dekko
{
namespace Accounts
{

/*!
 \brief

 \class SenderIdentity SenderIdentity.h "src/app/Accounts/SenderIdentity.h"
*/
class SenderIdentity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY dataChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY dataChanged)
    Q_PROPERTY(QString organization READ organization WRITE setOrganization NOTIFY dataChanged)
    Q_PROPERTY(QString signature READ signature WRITE setSignature NOTIFY dataChanged)
    Q_PROPERTY(bool useDefaultSignature READ useDefaultSignature WRITE setUseDefaultSignature NOTIFY dataChanged)
public:
    /*!
     \brief

     \fn SenderIdentity
     \param parent
    */
    explicit SenderIdentity(QObject *parent = 0);
    /*!
     \brief

     \fn ~SenderIdentity
    */
    ~SenderIdentity(){ this->deleteLater(); }

    /*!
     \brief

     \fn name
     \return QString
    */
    QString name() const;
    /*!
     \brief

     \fn setName
     \param name
    */
    void setName(const QString &name);
    /*!
     \brief

     \fn email
     \return QString
    */
    QString email() const;
    /*!
     \brief

     \fn setEmail
     \param email
    */
    void setEmail(const QString &email);
    /*!
     \brief

     \fn organization
     \return QString
    */
    QString organization()const;
    /*!
     \brief

     \fn setOrganization
     \param org
    */
    void setOrganization(const QString &org);
    /*!
     \brief

     \fn signature
     \return QString
    */
    QString signature() const;
    /*!
     \brief

     \fn setSignature
     \param signature
    */
    void setSignature(const QString &signature);
    /*!
     \brief

     \fn useDefaultSignature
     \return bool
    */
    bool useDefaultSignature() const;
    /*!
     \brief

     \fn setUseDefaultSignature
     \param shouldUse
    */
    void setUseDefaultSignature(const bool shouldUse);

    /*!
     \brief

     \fn toFormattedString
     \return QString
    */
    Q_INVOKABLE QString toFormattedString();

signals:
    /*!
     \brief

     \fn dataChanged
    */
    void dataChanged();

private:
    QString m_name; /*!< TODO */
    QString m_email; /*!< TODO */
    QString m_organization; /*!< TODO */
    QString m_signature; /*!< TODO */
    bool m_useDefaultSignature; /*!< TODO */

    /*!
     \brief

     \fn getDefaultSignature
     \return QString
    */
    QString getDefaultSignature() const;

};
}
}
#endif // SENDERIDENTITY_H
