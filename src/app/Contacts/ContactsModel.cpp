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

#include "ContactsModel.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QSqlError>
#include <QDir>
#include <cstdlib>
#include <QCryptographicHash>

namespace Dekko {
namespace Models  {

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel(parent) {
}

ContactsModel::~ContactsModel()
{
    m_db.close();
    emit beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    for (int i = 0; i < m_contactList.count(); i++) {
        delete m_contactList[i];
    }
    m_contactList.clear();
    emit endRemoveRows();
}

void ContactsModel::removeContact(QString contactId)
{
    int idx = indexOfContactId(contactId);
    
    QSqlQuery query(QString("DELETE FROM contact WHERE id = ?"), m_db);
    query.bindValue(0, contactId);
    query.exec();
    
    QSqlQuery removeEmailsQuery(QString("DELETE FROM email WHERE id = ?"), m_db);
    removeEmailsQuery.bindValue(0, contactId);
    removeEmailsQuery.exec();

    emit beginRemoveRows(QModelIndex(), idx, idx);
    m_contactList.removeAt(idx);
    m_contactMap.remove(contactId);
    emit endRemoveRows();
}

void ContactsModel::newContact(QString firstName, QString lastName, QString organization, QVariantList emails)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QString hashInput = (firstName + " " + lastName + (QString("%1").arg(rand())));
    hash.addData(hashInput.toLocal8Bit());
    QString contactId = QString(hash.result().toHex());

    Contact *c = new Contact(contactId, firstName, lastName, organization);
    for (int i = 0; i < emails.length(); i++) {
        c->emails.append(emails[i].toString());
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO contact(id, firstName, lastName, organization) VALUES(?, ?, ?, ?)");
    query.bindValue(0, c->id);
    query.bindValue(1, c->firstName);
    query.bindValue(2, c->lastName);
    query.bindValue(3, c->organization);
    query.exec();

    for (int i = 0; i < emails.length(); i++) {
        query.prepare("INSERT INTO email VALUES (?, ?)");
        query.bindValue(0, c->id);
        query.bindValue(1, c->emails[i]);
        query.exec();
    }

    emit beginInsertRows(QModelIndex(), m_contactList.length(), m_contactList.length());
    m_contactList.push_back(c);
    m_contactMap[c->id] = c;
    emit endInsertRows();
}

void ContactsModel::updateContact(QVariantMap contactMap)
{
    QString contactId = contactMap.value("id").toString();
    int idx = indexOfContactId(contactId);
    Contact *contact = m_contactList[idx];
    QSqlQuery query(m_db);
    if (contactMap.contains("firstName")) {
        QString firstName = contactMap.value("firstName").toString();
        if (firstName != contact->firstName) {
            query.prepare("UPDATE contact SET firstName = ? WHERE id = ?");
            query.bindValue(0, firstName);
            query.bindValue(1, contactId);
            query.exec();
            contact->firstName = firstName;
        }
    }
    if (contactMap.contains("lastName")) {
        QString lastName = contactMap.value("lastName").toString();
        if (lastName != contact->lastName) {
            query.prepare("UPDATE contact SET lastName = ? WHERE id = ?");
            query.bindValue(0, lastName);
            query.bindValue(1, contactId);
            query.exec();
            contact->lastName = lastName;
        }
    }
    if (contactMap.contains("organization")) {
        QString organization = contactMap.value("organization").toString();
        if (organization != contact->organization) {
            query.prepare("UPDATE contact SET organization = ? WHERE id = ?");
            query.bindValue(0, organization);
            query.bindValue(1, contactId);
            query.exec();
            contact->organization = organization;
        }
    }
    if (contactMap.contains("emails")) {
        query.prepare("DELETE FROM email where id = ?");
        query.bindValue(0, contactId);
        query.exec();
        QList<QVariant> emails = contactMap.value("emails").toList();
        contact->emails.clear();
        for (int i = 0; i < emails.length(); i++) {
            contact->emails.append(emails[i].toString());
        }
        for (int i = 0; i < contact->emails.length(); i++) {
            query.prepare("INSERT INTO email VALUES (?, ?)");
            query.bindValue(0, contact->id);
            query.bindValue(1, contact->emails[i]);
            query.exec();
        }
    }
    emit dataChanged(index(idx), index(idx));
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
    return m_contactList.count();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > m_contactList.count() - 1)
        return QVariant();
    Contact *c = m_contactList[index.row()];
    switch (role) {
    case RoleFirstName:
        return c->firstName;
    case RoleLastName:
        return c->lastName;
    case RoleEmails:
        return QVariant::fromValue(c->emails);
    case RoleTag:
        if (!c->firstName.isEmpty()) {
            return c->firstName + " " + c->lastName;
        }
        if (!c->lastName.isEmpty()) {
            return c->lastName;
        }
        return "#";
    case RoleContactId:
        return c->id;
    case RoleOrganization:
        return c->organization;
    }
    return QVariant();
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    static QHash<int, QByteArray> roleNames;
    if (roleNames.isEmpty()) {
        roleNames[RoleFirstName] = "firstName";
        roleNames[RoleLastName] = "lastName";
        roleNames[RoleEmails] = "emails";
        roleNames[RoleTag] = "tag";
        roleNames[RoleContactId] = "contactId";
        roleNames[RoleOrganization] = "organization";
    }
    return roleNames;
}

void ContactsModel::initDB()
{
    connectDB();
    createOrUpdateDatabaseIfNecessary();
    readData();
}

void ContactsModel::connectDB()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(
                QString("/%1/.local/share/dekko.dekkoproject/%2.sqlite")
                .arg(QDir::homePath()).arg(m_databaseName));
    m_db.open();
}

void ContactsModel::createOrUpdateDatabaseIfNecessary()
{
    QSqlQuery query("SELECT * FROM version", m_db);
    if (!query.exec()) {
        query.exec("CREATE TABLE version ( version INTEGER )");
        query.exec("INSERT INTO version VALUES ( 1 )");
        query.exec("CREATE TABLE contact (id TEXT PRIMARY KEY, firstName TEXT, lastName TEXT, organization TEXT, starred INTEGER)");
        query.exec("CREATE TABLE email (id TEXT, email TEXT)");
    }
}

void ContactsModel::readData()
{
    QSqlQuery query("SELECT * from contact", m_db);
    while (query.next()) {
        QString contactId = query.record().value(0).toString();
        QString firstName = query.record().value(1).toString();
        QString lastName = query.record().value(2).toString();
        QString organization = query.record().value(3).toString();
        Contact *c = new Contact(contactId, firstName, lastName, organization);
        emit beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_contactList.append(c);
        m_contactMap[c->id] = c;
        emit endInsertRows();
    }
    query.exec("SELECT * from email");
    while (query.next()) {
        QString id = query.record().value(0).toString();
        QString email = query.record().value(1).toString();
        if (m_contactMap.contains(id))
            m_contactMap[id]->emails.append(email);
    }
    emit dataChanged(index(0), index(rowCount() - 1), QVector<int>() << RoleEmails);
}

int ContactsModel::indexOfContactId(QString contactId)
{
    for (int i = 0; i < m_contactList.size(); i++) {
        if (contactId == m_contactList[i]->id)
            return i;
    }
    return -1;
}

QVariantMap ContactsModel::getByIndex(int idx) {
    QVariantMap map;
    Contact *c = m_contactList[idx];
    map["contactId"] = c->id;
    map["firstName"] = c->firstName;
    map["lastName"] = c->lastName;
    map["organization"] = c->organization;
    QVariantList list;
    for (int i = 0; i < c->emails.length(); i++) {
        list.append(QVariant::fromValue(c->emails[i]));
    }
    map["emails"] = list;
    return map;
}

QString ContactsModel::databaseName()
{
    return m_databaseName;
}

void ContactsModel::setDatabaseName(QString databaseName)
{
    m_databaseName = databaseName;
    initDB();
}

}
}
