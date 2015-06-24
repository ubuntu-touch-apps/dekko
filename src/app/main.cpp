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
// NOTE: uncomment to enable profiling
//#define QML_COMPILER_STATS
#define QT_QML_DEBUG
#include <QtQuick>
#include <QDebug>
#include <QQmlContext>
#include <QSettings>
#include <QtGui/QGuiApplication>
#include <QtQml/QtQml>
#include <QtQuick/QQuickView>
#include <QTranslator>
#include <QLocale>
#include <QScopedPointer>
#include "AppVersion/SetCoreApplication.h"
#include "Common/Application.h"
#include "Common/MetaTypes.h"
#include "Common/SettingsNames.h"
#include "app/Composition/ReplyMode.h"
#include "app/Components/ListViewWithPageHeader.h"
#include "app/Components/QmlObjectListModel.h"
#include "app/Components/ImageHelper.h"
#include "app/Components/qlimitproxymodelqml.h"
#include "app/Contacts/ContactsModel.h"
#include "app/Contacts/ContactFilterModel.h"
#include "app/Contacts/FlattenContactModel.h"
#include "app/Contacts/FlattenContactFilterModel.h"
#include "app/Accounts/Account.h"
#include "app/Accounts/AccountsManager.h"
#include "app/Accounts/PasswordManager.h"
#include "app/Models/AccountsListModel.h"
#include "app/Accounts/SenderIdentity.h"
#include "app/Accounts/SenderIdentityModel.h"
#include "app/Accounts/MultipleAccountSenderIdentityModel.h"
#include "app/Composition/SubmissionManager.h"
#include "app/Composition/ComposerUtils.h"
#include "app/MailConfig/AutoConfig.h"
#include "app/MailConfig/MailConfig.h"
#include "app/MailConfig/ImapSettingVerifier.h"
#include "app/Models/Recipient.h"
#include "app/Models/RecipientListModel.h"
#include "app/Models/RecipientListFilterModel.h"
#include "app/Models/MessageListModel.h"
#include "app/Models/MessageListFilterModel.h"
#include "app/Models/MessageModel.h"
#include "app/Models/PreSetProviderModel.h"
#include "app/Models/MailboxProxyModel.h"
#include "app/Models/MailboxFilterModel.h"
#include "app/Settings/Settings.h"
#include "app/Settings/GlobalSettings.h"
#include "app/Utils/AttachmentDownloader.h"
#include "app/Utils/Path.h"
#include "app/Utils/PlainTextFormatter.h"
#include "app/Utils/TabEventSignaler.h"
#include "app/Models/MessageBuilder.h"
#include "app/Utils/MailboxSearch.h"
#include "app/Utils/MailboxUtils.h"
#include "app/Utils/UidIterator.h"
#include "app/Utils/SearchForSpecialMailbox.h"
#include "app/Theme/Theme.h"
#include "app/Network/MsgPartNetAccessManagerFactory.h"
#include "Imap/Model/ImapAccess.h"
#include "Imap/Model/ThreadingMsgListModel.h"
#include "Imap/Model/kdeui-itemviews/kdescendantsproxymodel.h"
#include "Imap/Network/MsgPartNetAccessManager.h"
#include "MSA/Account.h"
#include "app/Settings/MailboxSettings.h"
#include "app/Settings/SettingsFileBase.h"
#include "app/Settings/NotificationSettings.h"
#include "Plugins/PluginManager.h"
#include "static_plugins.h"

#include "app/Notifications/NotificationService.h"
#include "app/Notifications/PostalServiceWorker.h"

QQuickView *qQuickViewer;

static void registerDekkoTypes(const char* uri, int major = 0, int minor = 2)
{
    // @uri DekkoCore
    qmlRegisterType<QSettings>(uri, major, minor, "QSettings");
    qmlRegisterType<Dekko::Notifications::NotificationService>(uri, major, minor, "NotificationService");
    qmlRegisterType<Dekko::Components::ListViewWithPageHeader>(uri, major, minor, "ListViewWithPageHeader");
    qmlRegisterType<QLimitProxyModelQML>(uri, major, minor, "LimitProxyModel");
    qmlRegisterType<Dekko::Components::ImageHelper>(uri, major, minor, "ImageHelper");
    qmlRegisterType<Dekko::AutoConfig>(uri, major, minor, "AutoConfig");
    qmlRegisterType<Dekko::ImapSettingVerifier>(uri, major, minor, "ImapSettingVerifier");
    qmlRegisterType<Dekko::ReplyMode>(uri, major, minor, "ReplyMode");
    qmlRegisterType<Dekko::ComposerUtils>(uri, major, minor, "ComposerUtils");
    qmlRegisterType<Dekko::Models::RecipientListFilterModel>(uri, major, minor, "RecipientListFilterModel");
    qmlRegisterType<Dekko::SubmissionManager>(uri, major, minor, "SubmissionManager");
    qmlRegisterType<Dekko::Models::Recipient>(uri, major, minor, "Recipient");
    qmlRegisterType<Dekko::Models::RecipientListModel>(uri, major, minor, "RecipientListModel");
    qmlRegisterType<Dekko::Accounts::Account>(uri, major, minor, "Account");
    qmlRegisterType<Dekko::Accounts::AccountsManager>(uri, major, minor, "AccountsManager");
    qmlRegisterType<Dekko::Accounts::PasswordManager>(uri, major, minor, "PasswordManager");
    qmlRegisterType<Dekko::Models::AccountsListModel>(uri, major, minor, "AccountsListModel");
    qmlRegisterType<Dekko::Accounts::SenderIdentity>(uri, major, minor, "SenderIdentity");
    qmlRegisterType<Dekko::Accounts::SenderIdentityModel>(uri, major, minor, "SenderIdentityModel");
    qmlRegisterType<Dekko::Accounts::MultipleAccountSenderIdentityModel>(uri, major, minor, "MultipleAccountSenderIdentityModel");
    qmlRegisterType<Dekko::Models::RecipientListModel>(uri, major, minor, "RecipientListModel");
    qmlRegisterType<Dekko::Models::MessageListModel>(uri, major, minor, "MessageListModel");
    qmlRegisterType<Dekko::Models::MessageListFilterModel>(uri, major, minor, "MessageListFilterModel");
    qmlRegisterType<Dekko::Models::MessageModel>(uri, major, minor, "MessageModel");
    qmlRegisterType<Dekko::Utils::AttachmentDownloader>(uri, major, minor, "AttachmentDownloader");
    qmlRegisterType<Dekko::Utils::PlainTextFormatter>(uri, major, minor, "PlainTextFormatter");
    qmlRegisterType<Dekko::Utils::MessageBuilder>(uri, major, minor, "MessageBuilder");
    qmlRegisterType<Dekko::Utils::MailboxSearch>(uri, major, minor, "MailboxSearch");
    qmlRegisterType<Dekko::Utils::MailboxUtils>(uri, major, minor, "MailboxUtils");
    qmlRegisterType<Dekko::Utils::UidIterator>(uri, major, minor, "UidIterator");
    qmlRegisterType<Dekko::Utils::SearchForSpecialMailbox>(uri, major, minor, "SearchForSpecialMailbox");
    qmlRegisterType<Dekko::Settings::ImapSettings>(uri, major, minor, "ImapSettings");
    qmlRegisterType<Dekko::Settings::SmtpSettings>(uri, major, minor, "SmtpSettings");
    qmlRegisterType<Dekko::Settings::AccountProfile>(uri, major, minor, "AccountProfile");
    qmlRegisterType<Dekko::Settings::Preferences>(uri, major, minor, "Preferences");
    qmlRegisterType<Dekko::Settings::OfflineSettings>(uri, major, minor, "OfflineSettings");
    qmlRegisterType<Dekko::Settings::DeveloperSettings>(uri, major, minor, "DeveloperSettings");
    qmlRegisterType<Dekko::Settings::UOASettings>(uri, major, minor, "UOASettings");
    qmlRegisterType<Dekko::Settings::MailboxSettings>(uri, major, minor, "MailboxSettings");
    qmlRegisterType<Dekko::Settings::NotificationSettings>(uri, major, minor, "NotificationSettings");
    qmlRegisterType<Dekko::Models::PreSetProviderModel>(uri, major, minor, "PreSetProvidersModel");
    qmlRegisterType<Dekko::Models::MailboxProxyModel>(uri, major, minor, "MailboxProxyModel");
    qmlRegisterType<Dekko::Models::MailboxFilterModel>(uri, major, minor, "MailboxFilterModel");
    qmlRegisterType<Dekko::Models::ContactsModel>(uri, major, minor, "DekkoContactsModel");
    qmlRegisterType<Dekko::Models::ContactFilterModel>(uri, major, minor, "ContactFilterModel");
    qmlRegisterType<Dekko::Models::FlattenContactModel>(uri, major, minor, "FlattenContactModel");
    qmlRegisterType<Dekko::Models::FlattenContactFilterModel>(uri, major, minor, "FlattenContactFilterModel");
    qmlRegisterType<KDescendantsProxyModel>(uri, major, minor, "FlatteningProxyModel");
    qmlRegisterType<Imap::Network::MsgPartNetAccessManager>(uri, major, minor, "MsgPartNetAccessManager");
    qmlRegisterUncreatableType<Dekko::Components::QmlObjectListModel>(uri, major, minor, "QmlObjectListModel", QLatin1String("Can't create an instance from qml"));
    qmlRegisterUncreatableType<Dekko::ComposerUtils>(uri, major, minor, "RecipientKind",
                                                            QLatin1String("Can't create recipientkind from qml"));
    qmlRegisterSingletonType<Dekko::Utils::TabEventSignaler>(uri, major, minor, "TabEventSignaler", Dekko::Utils::TabEventSignaler::factory);
    qmlRegisterSingletonType<Dekko::Settings::GlobalSettings>(uri, major, minor, "GlobalSettings", Dekko::Settings::GlobalSettings::factory);
    qmlRegisterSingletonType<Dekko::Themes::Theme>(uri, major, minor, "Style", Dekko::Themes::Theme::factory);
}

static void registerTrojitaCoreTypes(const char *uri, int major = 0, int minor = 1)
{
    // @uri TrojitaCore
    qmlRegisterUncreatableType<Imap::Mailbox::ThreadingMsgListModel>(uri, major, minor, "ThreadingMsgListModel",
            QLatin1String("ThreadingMsgListModel can only be created from the C++ code.\
                          Use ImapAccess if you need access to an instance."));
    qmlRegisterSingletonType<UiUtils::Formatting>(uri, major, minor, "UiFormatting", UiUtils::Formatting::factory);
    qmlRegisterUncreatableType<Composer::Submission>(uri, major, minor, "SubmissionMethod",
            QLatin1String("Composer::Submission::Method can be only created from the C++ code."));
    qmlRegisterUncreatableType<UiUtils::PasswordWatcher>(uri, major, minor, "PasswordWatcher",
            QLatin1String("PasswordWatcher can only be created from the C++ code. Use ImapAccess\
                          if you need access to an instance."));
}


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Q_INIT_RESOURCE(actionicons);
    Q_INIT_RESOURCE(providericons);

    // Configure mbox settings
    QScopedPointer<Dekko::Settings::SettingsFileBase> mboxSettings(new Dekko::Settings::SettingsFileBase);
    Dekko::Settings::MailboxSettings::setDefaultFile(mboxSettings.data());
    mboxSettings.data()->setPath(Dekko::configLocation(QStringLiteral("mailboxConfig.json")));
    // Notification settings
    QScopedPointer<Dekko::Settings::SettingsFileBase> notificationSettings(new Dekko::Settings::SettingsFileBase);
    Dekko::Settings::NotificationSettings::setSettingsFile(notificationSettings.data());
    notificationSettings.data()->setPath(Dekko::configLocation(QStringLiteral("notifications.json")));

    //Install translations
    QTranslator dekkoTranslater;
    QTranslator dekkoBuildTranslater;
    // It would appear Qt has a real problem loading translations for anywhere but in the same directory
    // as the executable. I have no idea if this is click package related or a bug in Qt but this works
    // so i'm not going to complain.... (*much*)
    if (dekkoTranslater.load(QLocale::system().name(), app.applicationDirPath())) {
        qDebug() << "Translation loaded" ;
        app.installTranslator(&dekkoTranslater);
    } else if (dekkoTranslater.load(QLocale::system().name(), QCoreApplication::applicationDirPath() + QLatin1String("/locale/"))) {
        qDebug() << "Installing translations in build dir";
        app.installTranslator(&dekkoBuildTranslater);
    }


    qQuickViewer = new QQuickView();
    qQuickViewer->setResizeMode(QQuickView::SizeRootObjectToView);
    app.addLibraryPath(QCoreApplication::applicationDirPath());

    // let's first check all standard locations
    QString qmlFile = Dekko::findQmlFile(QString::fromLatin1("qml/dekko.qml"));

    Common::registerMetaTypes();
    Common::Application::name = QString::fromLatin1("dekko.dekkoproject");
    Common::Application::organization = QString::fromLatin1("dekko.dekkoproject");
    AppVersion::setVersion();
    AppVersion::setCoreApplicationData();

    QUrl addressbookStoreLocation(QString::fromLatin1(
            "file://%1/.local/share/dekko.dekkoproject/addressbook.vcard")
            .arg(QDir::homePath()));
    qQuickViewer->engine()->rootContext()->setContextProperty(QLatin1String("addressbookStoreLocation"), QVariant::fromValue(addressbookStoreLocation));

    Dekko::MsgPartNetAccessManagerFactory msgPartNetAccessManagerFactory;
    qQuickViewer->engine()->setNetworkAccessManagerFactory(&msgPartNetAccessManagerFactory);
    qQuickViewer->engine()->rootContext()->setContextProperty(QLatin1String("msgPartNetAccessManagerFactory"), &msgPartNetAccessManagerFactory);

    qQuickViewer->engine()->rootContext()->setContextProperty(QLatin1String("appUris"), QString::fromLatin1(qgetenv("APP_URIS")));

    registerDekkoTypes("DekkoCore");
    registerTrojitaCoreTypes("TrojitaCore");


    qQuickViewer->setTitle(QObject::trUtf8("Dekko"));
    qQuickViewer->setSource(QUrl::fromLocalFile(qmlFile));
    qQuickViewer->show();
    return app.exec();
}
