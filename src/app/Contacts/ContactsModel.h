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

#include <QAbstractListModel>
#include <QSqlDatabase>

namespace Dekko {
namespace Models {

class Contact {
public:
    Contact(QString id, QString firstName, QString lastName, QString organization)
        : id(id), firstName(firstName), lastName(lastName), organization(organization) { }
    QString id;
    QString firstName;
    QString lastName;
    QString organization;
    QList<QString> emails;
};

class ContactsModel: public QAbstractListModel {
    Q_OBJECT
    Q_ENUMS(ContactRole)
    Q_PROPERTY(int length READ rowCount)
    Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName)
public:

    typedef enum {
        RoleFirstName = Qt::UserRole + 1,
        RoleLastName,
        RoleEmails,
        RoleTag,
        RoleContactId,
        RoleOrganization
    } ContactRole;

    explicit ContactsModel(QObject *parent = 0);
    ~ContactsModel();

    Q_INVOKABLE void removeContact(QString contactId);
    Q_INVOKABLE void newContact(QString firstName, QString lastName, QString organization, QVariantList emails);
    Q_INVOKABLE void updateContact(QVariantMap contactMap);
    Q_INVOKABLE QVariantMap getByIndex(int idx);

    QString databaseName();
    void setDatabaseName(QString databaseName);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

protected:
    virtual QHash<int, QByteArray> roleNames() const;

private:
    void initDB(); // should have m_databaseName set before init is called
    void connectDB();
    void createOrUpdateDatabaseIfNecessary();
    void readData();
    int indexOfContactId(QString contactId);

    QList<Contact *> m_contactList;
    QMap<QString, Contact *> m_contactMap;

    QSqlDatabase m_db;

    QString m_databaseName;
};

}
}
