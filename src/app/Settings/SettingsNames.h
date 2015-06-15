#ifndef SETTINGSNAMES_H
#define SETTINGSNAMES_H

#include <QString>

namespace Dekko {

struct SettingsNames {
    static const QString identitiesKey, realNameKey, addressKey, organisationKey, signatureKey, obsRealNameKey, obsAddressKey;
    static const QString msaMethodKey, methodSMTP, methodSSMTP, methodSENDMAIL, methodImapSendmail, smtpHostKey,
           smtpPortKey, smtpAuthKey, smtpStartTlsKey, smtpUserKey, smtpAuthType,
           sendmailKey, sendmailDefaultCmd;
    static const QString imapMethodKey, methodTCP, methodSSL, methodProcess, imapHostKey,
           imapPortKey, imapStartTlsKey, imapUserKey, imapProcessKey,
           imapStartOffline, imapEnableId, obsImapSslPemCertificate, imapSslPemPubKey,
           imapBlacklistedCapabilities, imapUseSystemProxy, imapNeedsNetwork, imapAuthType, plainAuth, loginAuth, oAuth;
    static const QString composerSaveToImapKey, composerImapSentKey, smtpUseBurlKey;
    static const QString cacheMetadataKey, cacheMetadataMemory,
           cacheOfflineKey, cacheOfflineNone, cacheOfflineXDays, cacheOfflineAll, cacheOfflineNumberDaysKey;
    static const QString appLoadHomepage;
    static const QString knownEmailsKey;
    static const QString addressbookPlugin, passwordPlugin;
    static const QString imapAutoExpunge;
    static const QString imapAutoLoadImages;
    static const QString preferPlainText, theme;
    static const QString developerModeEnabled, enableImapModelLog, devModeEnableThreading, devModeEnableContacts;
    static const QString profileName, profileDescription, profileOrganization;
    static const QString uoaAccountId, uoaProvider, uoaDekkoProvider, uoaGmailProvider;
    static const QString markAsRead, neverMarkRead, immediatelyMarkRead, secondsBeforeMarkRead, markAsReadAfter;
    static const QString hideMarkedDeleted, allowGravatar, previewLines;

};

}

#endif // SETTINGSNAMES_H
