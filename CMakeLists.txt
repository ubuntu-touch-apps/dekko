set(UBUNTU_PROJECT_TYPE "ClickApp" CACHE INTERNAL "Let Qtc know were a click app")
set(UBUNTU_MANIFEST_PATH "click/manifest.json" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")
set(CMAKE_INSTALL_PREFIX "/")

# we have to hard pick qt5 in the click chroot otherwise we can't build translations
SET(ENV{QT_SELECT} qt5)

find_program(DPKG_ARCHITECTURE_EXECUTABLE dpkg-architecture)
if(DPKG_ARCHITECTURE_EXECUTABLE)
    execute_process(
        COMMAND ${DPKG_ARCHITECTURE_EXECUTABLE} -qDEB_HOST_MULTIARCH
            OUTPUT_VARIABLE ARCH_TRIPLET
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        # Get the build host arch and test it is armhf, since
        # CMAKE_LIBRARY_ARCHITECTURE and CMAKE_SYSTEM_PROCESSOR don't contain
        # any value inside the click chroot?? query the DEB_HOST_ARCH
        COMMAND ${DPKG_ARCHITECTURE_EXECUTABLE} -qDEB_HOST_ARCH
            OUTPUT_VARIABLE HOST_ARCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    message(FATAL_ERROR "Ubuntu click mode requires dpkg-architecture be installed, stopping build")
endif()

cmake_minimum_required(VERSION 2.8.9)
project(dekko)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# POSITION_INDEPENDENT_CODE is only available on cmake 2.8.9 or later
# Add needed flags for supported compilers which simulate POSITION_INDEPENDENT_CODE property
if(CMAKE_VERSION VERSION_LESS "2.8.9")
    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    else()
        message(FATAL_ERROR "Needs GNU or Clang C++ compiler or CMake 2.8.9 (or later)")
    endif()
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

include(FindCXXFeatures)
if(NOT CXXFeatures_auto_FOUND)
    message(SEND_ERROR "Your compiler doesn't support C++11's auto")
endif()
if(NOT CXXFeatures_static_assert_FOUND)
    message(SEND_ERROR "Your compiler doesn't support C++11's static_assert")
endif()
if(NOT CXXFeatures_alignof_FOUND)
    if(NOT CMAKE_COMPILER_IS_GNUCXX AND NOT MSVC)
        message(SEND_ERROR "Your compiler doesn't support C++11's alignof and it also isn't gcc or MSVC. Either would work.")
    endif()
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")

include(TrojitaOption)

trojita_option(WITH_RAGEL "Build with Ragel library" AUTO)
trojita_option(WITH_ZLIB "Build with zlib library" AUTO)
trojita_option(WITH_SHARED_PLUGINS "Enable shared dynamic plugins" OFF)
trojita_option(WITH_TESTS "Build tests" ON)
trojita_option(DEV_FATAL_WARNINGS "Turn build warnings into errors (developers only)" OFF)
trojita_option(UPDATE_TRANSLATIONS "Update dekko .pot template" ON)

trojita_plugin_option(WITH_SIMPLECRYPT_PLUGIN "Build simplecrypt password plugin" STATIC)

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network 5.4 REQUIRED)
find_package(Qt5Sql 5.4 REQUIRED)
#find_package(Qt5LinguistTools 5.4 REQUIRED)
if(WITH_TESTS)
    find_package(Qt5Test REQUIRED)
endif()
if(Qt5LinguistTools_FOUND)
    find_package(Qt5LinguistForDekko)
endif()
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Qml 5.4 REQUIRED)
find_package(Qt5Svg 5.4 REQUIRED)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib${LIB_SUFFIX}")
endif()
mark_as_advanced(CMAKE_INSTALL_LIBDIR)

if(NOT CMAKE_INSTALL_PLUGIN_DIR)
    set(CMAKE_INSTALL_PLUGIN_DIR "${CMAKE_INSTALL_LIBDIR}/dekko")
endif()
mark_as_advanced(CMAKE_INSTALL_PLUGIN_DIR)

if(NOT PLUGIN_DIR)
    if(IS_ABSOLUTE ${CMAKE_INSTALL_PLUGIN_DIR})
        set(PLUGIN_DIR "${CMAKE_INSTALL_PLUGIN_DIR}")
    else()
        set(PLUGIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PLUGIN_DIR}")
    endif()
endif()
mark_as_advanced(PLUGIN_DIR)

set(CMAKE_CXX_FLAGS "-O2 -g -Wall -Wsign-compare ${CMAKE_CXX_FLAGS}")
if(DEV_FATAL_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdParty/trojita
    ${Qt5Qml_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DQT_STRICT_ITERATORS)

set(CMAKE_AUTOMOC True)


trojita_find_package(RagelForTrojita "" "" "" "" WITH_RAGEL)

if(NOT ZLIB_FOUND)
    trojita_find_package(ZLIB "" "" "" "" WITH_ZLIB)
endif()

if(WITH_ZLIB)
    set(TROJITA_HAVE_ZLIB True)
    message(STATUS "Support for COMPRESS=DEFLATE enabled")
else()
    set(TROJITA_HAVE_ZLIB False)
    message(STATUS "Disabling COMPRESS=DEFLATE, zlib is not available")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/configure.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/configure.cmake.h)

feature_summary(FATAL_ON_MISSING_REQUIRED_PACKAGES DESCRIPTION "\n" WHAT ALL)

set(trojita_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdParty/trojita)

set(path_Common ${trojita_PATH}/Common)
set(libCommon_SOURCES
    ${path_Common}/Application.cpp
    ${path_Common}/ConnectionId.cpp
    ${path_Common}/DeleteAfter.cpp
    ${path_Common}/FileLogger.cpp
    ${path_Common}/MetaTypes.cpp
    ${path_Common}/Paths.cpp
    ${path_Common}/SettingsNames.cpp
)

set(path_Plugins ${trojita_PATH}/Plugins)
set(libPlugins_SOURCES
    ${path_Plugins}/AddressbookPlugin.cpp
    ${path_Plugins}/PasswordPlugin.cpp
    ${path_Plugins}/PluginJob.cpp
    ${path_Plugins}/PluginManager.cpp
)

set(path_UiUtils ${trojita_PATH}//UiUtils)
set(libUiUtils_SOURCES
    ${path_UiUtils}/Formatting.cpp
    ${path_UiUtils}/PasswordWatcher.cpp
)

set(path_Composer ${trojita_PATH}//Composer)
set(libComposer_SOURCES
    ${path_Composer}/ComposerAttachments.cpp
    ${path_Composer}/Mailto.cpp
    ${path_Composer}/MessageComposer.cpp
    ${path_Composer}/PlainTextFormatter.cpp
    ${path_Composer}/Recipients.cpp
    ${path_Composer}/ReplaceSignature.cpp
    ${path_Composer}/SenderIdentitiesModel.cpp
    ${path_Composer}/SubjectMangling.cpp
    ${path_Composer}/Submission.cpp
)

set(path_MSA ${trojita_PATH}/MSA)
set(libMSA_SOURCES
    ${path_MSA}/AbstractMSA.cpp
    ${path_MSA}/FakeMSA.cpp
    ${path_MSA}/ImapSubmit.cpp
    ${path_MSA}/SMTP.cpp
    ${path_MSA}/Sendmail.cpp
)

set(path_Streams ${trojita_PATH}/Streams)
set(libStreams_SOURCES
    ${path_Streams}/DeletionWatcher.cpp
    ${path_Streams}/FakeSocket.cpp
    ${path_Streams}/IODeviceSocket.cpp
    ${path_Streams}/Socket.cpp
    ${path_Streams}/SocketFactory.cpp
)

if(WITH_ZLIB)
    set(libStreams_SOURCES ${libStreams_SOURCES}
        ${path_Streams}/3rdparty/rfc1951.cpp)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

set(libqwwsmtpclient_SOURCES ${trojita_PATH}/qwwsmtpclient/qwwsmtpclient.cpp)

set(libAppVersion_SOURCES ${trojita_PATH}/AppVersion/SetCoreApplication.cpp)

set(path_Imap ${trojita_PATH}/Imap)
set(libImap_SOURCES
    ${path_Imap}/ConnectionState.cpp
    ${path_Imap}/Encoders.cpp
    ${path_Imap}/Exceptions.cpp
    ${path_Imap}/Parser/3rdparty/kcodecs.cpp
    ${path_Imap}/Parser/3rdparty/rfccodecs.cpp

    ${path_Imap}/Parser/Command.cpp
    ${path_Imap}/Parser/Data.cpp
    ${path_Imap}/Parser/LowLevelParser.cpp
    ${path_Imap}/Parser/MailAddress.cpp
    ${path_Imap}/Parser/Message.cpp
    ${path_Imap}/Parser/Parser.cpp
    ${path_Imap}/Parser/Response.cpp
    ${path_Imap}/Parser/Sequence.cpp
    ${path_Imap}/Parser/ThreadingNode.cpp

    ${path_Imap}/Network/FileDownloadManager.cpp
    ${path_Imap}/Network/ForbiddenReply.cpp
    ${path_Imap}/Network/MsgPartNetAccessManager.cpp
    ${path_Imap}/Network/MsgPartNetworkReply.cpp
    ${path_Imap}/Network/QQuickNetworkReplyWrapper.cpp

    ${path_Imap}/Model/Cache.cpp
    ${path_Imap}/Model/CombinedCache.cpp
    ${path_Imap}/Model/DragAndDrop.cpp
    ${path_Imap}/Model/DiskPartCache.cpp
    ${path_Imap}/Model/DummyNetworkWatcher.cpp
    ${path_Imap}/Model/FindInterestingPart.cpp
    ${path_Imap}/Model/FlagsOperation.cpp
    ${path_Imap}/Model/FullMessageCombiner.cpp
    ${path_Imap}/Model/ImapAccess.cpp
    ${path_Imap}/Model/MailboxFinder.cpp
    ${path_Imap}/Model/MailboxMetadata.cpp
    ${path_Imap}/Model/MailboxModel.cpp
    ${path_Imap}/Model/MailboxTree.cpp
    ${path_Imap}/Model/MemoryCache.cpp
    ${path_Imap}/Model/Model.cpp
    ${path_Imap}/Model/MsgListModel.cpp
    ${path_Imap}/Model/NetworkWatcher.cpp
    ${path_Imap}/Model/OneMessageModel.cpp
    ${path_Imap}/Model/ParserState.cpp
    ${path_Imap}/Model/SpecialFlagNames.cpp
    ${path_Imap}/Model/SQLCache.cpp
    ${path_Imap}/Model/SubtreeModel.cpp
    ${path_Imap}/Model/SystemNetworkWatcher.cpp
    ${path_Imap}/Model/UnityNetworkWatcher.cpp
    ${path_Imap}/Model/TaskFactory.cpp
    ${path_Imap}/Model/TaskPresentationModel.cpp
    ${path_Imap}/Model/ThreadingMsgListModel.cpp
    ${path_Imap}/Model/Utils.cpp
    ${path_Imap}/Model/VisibleTasksModel.cpp

    # The ModelTest is only needed when debugging manually
    #${path_Imap}/Model/ModelTest/modeltest.cpp
    # The ModelWatcher is another debugging aid
    ${path_Imap}/Model/ModelWatcher.cpp

    ${path_Imap}/Model/kdeui-itemviews/kdescendantsproxymodel.cpp

    ${path_Imap}/Tasks/AppendTask.cpp
    ${path_Imap}/Tasks/CopyMoveMessagesTask.cpp
    ${path_Imap}/Tasks/CreateMailboxTask.cpp
    ${path_Imap}/Tasks/DeleteMailboxTask.cpp
    ${path_Imap}/Tasks/EnableTask.cpp
    ${path_Imap}/Tasks/ExpungeMailboxTask.cpp
    ${path_Imap}/Tasks/ExpungeMessagesTask.cpp
    ${path_Imap}/Tasks/Fake_ListChildMailboxesTask.cpp
    ${path_Imap}/Tasks/Fake_OpenConnectionTask.cpp
    ${path_Imap}/Tasks/FetchMsgMetadataTask.cpp
    ${path_Imap}/Tasks/FetchMsgPartTask.cpp
    ${path_Imap}/Tasks/GenUrlAuthTask.cpp
    ${path_Imap}/Tasks/GetAnyConnectionTask.cpp
    ${path_Imap}/Tasks/IdTask.cpp
    ${path_Imap}/Tasks/IdleLauncher.cpp
    ${path_Imap}/Tasks/ImapTask.cpp
    ${path_Imap}/Tasks/KeepMailboxOpenTask.cpp
    ${path_Imap}/Tasks/ListChildMailboxesTask.cpp
    ${path_Imap}/Tasks/NoopTask.cpp
    ${path_Imap}/Tasks/NumberOfMessagesTask.cpp
    ${path_Imap}/Tasks/ObtainSynchronizedMailboxTask.cpp
    ${path_Imap}/Tasks/OfflineConnectionTask.cpp
    ${path_Imap}/Tasks/OpenConnectionTask.cpp
    ${path_Imap}/Tasks/SortTask.cpp
    ${path_Imap}/Tasks/SubscribeUnsubscribeTask.cpp
    ${path_Imap}/Tasks/ThreadTask.cpp
    ${path_Imap}/Tasks/UidSubmitTask.cpp
    ${path_Imap}/Tasks/UnSelectTask.cpp
    ${path_Imap}/Tasks/UpdateFlagsTask.cpp
    ${path_Imap}/Tasks/UpdateFlagsOfAllMessagesTask.cpp
)

if(WITH_RAGEL)
    message(STATUS "Using Ragel for the RFC 5322 parser")
    ragel_parser(${path_Imap}/Parser/Rfc5322HeaderParser.cpp)
    set(libImap_SOURCES ${libImap_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/Rfc5322HeaderParser.generated.cpp)
else()
    message(STATUS "Using pregenerated RFC 5322 parser")
    set(libImap_SOURCES ${libImap_SOURCES}
        ${path_Imap}/Parser/Rfc5322HeaderParser.generated.cpp)
endif()

set(dekko_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.cpp
)
set(push_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/push-helper.cpp
)

set(simpleCryptPlugin_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/SimpleCryptPassword/SimpleCrypt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/SimpleCryptPassword/EncryptedPassword.cpp
)

set(version_files ${CMAKE_CURRENT_BINARY_DIR}/trojita-version.h ${CMAKE_CURRENT_BINARY_DIR}/trojita-git-version.h)
if(WITH_NSIS)
    set(version_files ${version_files} ${CMAKE_CURRENT_BINARY_DIR}/trojita-version.nsi)
    set(NSIS TRUE)
endif()

add_custom_target(version DEPENDS version_fake_file)
add_custom_command(OUTPUT version_fake_file ${version_files}
    COMMAND ${CMAKE_COMMAND} -DGIT_EXECUTABLE=${GIT_EXECUTABLE} -DNSIS=${NSIS} -DCLICK_MODE=${CLICK_MODE} -DHOST_ARCH=${HOST_ARCH} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/TrojitaVersion.cmake)
set_source_files_properties(${version_files}
    PROPERTIES GENERATED TRUE
    HEADER_FILE_ONLY TRUE)

add_library(Common STATIC ${libCommon_SOURCES})
add_dependencies(Common version)
target_link_libraries(Common)

add_library(AppVersion STATIC ${libAppVersion_SOURCES})
add_dependencies(AppVersion version)
target_link_libraries(AppVersion Common)

if(WITH_SHARED_PLUGINS)
    add_library(Plugins SHARED ${libPlugins_SOURCES})
else()
    add_library(Plugins STATIC ${libPlugins_SOURCES})
    set_property(TARGET Plugins APPEND PROPERTY COMPILE_DEFINITIONS QT_STATICPLUGIN)
endif()
set_target_properties(Plugins PROPERTIES OUTPUT_NAME trojita_plugins)
qt5_use_modules(Plugins Core)

add_library(UiUtils STATIC ${libUiUtils_SOURCES})
target_link_libraries(UiUtils Plugins Composer)

add_library(Streams STATIC ${libStreams_SOURCES})
target_link_libraries(Streams)
if(WITH_ZLIB)
    target_link_libraries(Streams ${ZLIB_LIBRARIES})
endif()

add_library(qwwsmtpclient STATIC ${libqwwsmtpclient_SOURCES})
target_link_libraries(qwwsmtpclient)

add_library(MSA STATIC ${libMSA_SOURCES})
target_link_libraries(MSA Imap Streams qwwsmtpclient)

add_library(Composer STATIC ${libComposer_SOURCES})
target_link_libraries(Composer Common MSA Streams qwwsmtpclient)
include(FindPkgConfig)

pkg_search_module(CONNECTIVITY REQUIRED connectivity-qt1)
if(NOT CONNECTIVITY_FOUND AND CONNECTIVITY_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find connectivity-qt1")
else()
    message(STATUS "Connectivity-api FOUND")
    message(STATUS "CONNECTIVITY_INCLUDE_DIRS=${CONNECTIVITY_INCLUDE_DIRS}")
    message(STATUS "CONNECTIVITY_LIBDIR=${CONNECTIVITY_LIBDIR}")
    message(STATUS "CONNECTIVITY_LIBRARIES=${CONNECTIVITY_LIBRARIES}")
endif()
include_directories(${CONNECTIVITY_INCLUDE_DIRS})
link_directories(${CONNECTIVITY_INCLUDE_DIRS})

pkg_search_module(GSETTINGS_QT REQUIRED gsettings-qt)
if(NOT GSETTINGS_QT_FOUND)
    MESSAGE(FATAL_ERROR "Could not find gsettings-qt")
else()
    MESSAGE(STATUS "GSETTINGS-QT INCLUDE DIRS = ${GSETTINGS_QT_INCLUDE_DIRS}")
endif()
include_directories(${GSETTINGS_QT_INCLUDE_DIRS})
link_directories(${GSETTINGS_QT_INCLUDE_DIRS})

add_library(Imap STATIC ${libImap_SOURCES})
target_link_libraries(Imap Common Streams UiUtils connectivity-qt1)
if(WITH_ZLIB)
    target_link_libraries(Imap ${ZLIB_LIBRARIES})
endif()

if(WITH_SIMPLECRYPT_PLUGIN)
    trojita_add_plugin(dekko_plugin_SimpleCryptPasswordPlugin WITH_SIMPLECRYPT_PLUGIN ${simpleCryptPlugin_SOURCES})
endif()

# Generate file static_plugins.h.in
get_property(STATIC_PLUGINS GLOBAL PROPERTY TROJITA_STATIC_PLUGINS)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h.in "#include <QtPlugin>\n")
foreach(PLUGIN ${STATIC_PLUGINS})
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h.in "Q_IMPORT_PLUGIN(${PLUGIN})\n")
endforeach()
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h.in ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h)

qt5_use_modules(AppVersion Core)
qt5_use_modules(Common Core Network)
qt5_use_modules(UiUtils Core Gui Network Widgets)
qt5_use_modules(Streams Network)
qt5_use_modules(qwwsmtpclient Network)
qt5_use_modules(MSA Network)
qt5_use_modules(Composer Gui Network)
qt5_use_modules(Imap Gui Network Sql)


set(DEKKO_UBUNTU_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DEKKO_QML_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml)
set(DEKKO_ICON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/icons)
set(AUTOPILOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/autopilot/dekko)
set(CLICK_MANIFESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/click/.excludes
    ${CMAKE_CURRENT_SOURCE_DIR}/click/dekko.apparmor
    ${CMAKE_CURRENT_SOURCE_DIR}/click/com.ubuntu.developer.dpniel.dekko_dekko.application
    ${CMAKE_CURRENT_SOURCE_DIR}/click/com.ubuntu.developer.dpniel.dekko_dekko.service
    ${CMAKE_CURRENT_SOURCE_DIR}/click/dekko.url-dispatcher
    ${CMAKE_CURRENT_SOURCE_DIR}/click/dekko-content.json
    ${CMAKE_CURRENT_SOURCE_DIR}/click/pushHelper.json
    ${CMAKE_CURRENT_SOURCE_DIR}/click/pushHelper-apparmor.json
    ${CMAKE_CURRENT_SOURCE_DIR}/push-helper.py
)
set(CLICK_PKG_MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/click/manifest.json)
set(DEKKO_DESKTOP_FILE ${DEKKO_UBUNTU_PATH}/app/dekko.desktop)
set(DEKKO_CONFIGURATION_DIR ${DEKKO_UBUNTU_PATH}/configuration)
set(DEKKO_MAIN_QML
    ${DEKKO_QML_PATH}/main.qml
)
set(DEKKO_CONFIG_FILES
    ${DEKKO_CONFIGURATION_DIR}/domainService.conf
    ${DEKKO_CONFIGURATION_DIR}/serviceProviders.conf
    ${DEKKO_CONFIGURATION_DIR}/styles.conf
)

set(lib_DekkoComponents
    ${DEKKO_UBUNTU_PATH}/app/Components/ListViewWithPageHeader.cpp
    ${DEKKO_UBUNTU_PATH}/app/Components/QmlObjectListModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Components/ImageHelper.cpp
    ${DEKKO_UBUNTU_PATH}/app/Components/qlimitproxymodelqml.cpp
)

set(lib_DekkoComposition
    ${DEKKO_UBUNTU_PATH}/app/Composition/SubmissionManager.cpp
    ${DEKKO_UBUNTU_PATH}/app/Composition/ReplyMode.cpp
    ${DEKKO_UBUNTU_PATH}/app/Composition/ComposerUtils.cpp
)

set(lib_DekkoAccounts
    ${DEKKO_UBUNTU_PATH}/app/Accounts/SenderIdentity.cpp
    ${DEKKO_UBUNTU_PATH}/app/Accounts/SenderIdentityModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Accounts/MultipleAccountSenderIdentityModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Accounts/AccountsManager.cpp
    ${DEKKO_UBUNTU_PATH}/app/Accounts/PasswordManager.cpp
    ${DEKKO_UBUNTU_PATH}/app/Accounts/Account.cpp
    ${DEKKO_UBUNTU_PATH}/app/Accounts/AccountUtils.cpp
)

set(lib_DekkoMailConfig
    ${DEKKO_UBUNTU_PATH}/app/MailConfig/MailConfig.cpp
    ${DEKKO_UBUNTU_PATH}/app/MailConfig/AutoConfig.cpp
    ${DEKKO_UBUNTU_PATH}/app/MailConfig/MailConfigFromAutoconfig.cpp
    ${DEKKO_UBUNTU_PATH}/app/MailConfig/ImapSettingVerifier.cpp
)

set(lib_DekkoModels
    ${DEKKO_UBUNTU_PATH}/app/Models/Recipient.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/RecipientListModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/AttachmentListModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/MessageListModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/MessageListFilterModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/MessageModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/PreSetProviderModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/MailboxFilterModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/MailboxProxyModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/RecipientListFilterModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/AccountsListModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Models/MessageBuilder.cpp
)

set(lib_DekkoSettings
    ${DEKKO_UBUNTU_PATH}/app/Settings/Settings.cpp
    ${DEKKO_UBUNTU_PATH}/app/Settings/GlobalSettings.cpp
    ${DEKKO_UBUNTU_PATH}/app/Settings/SettingsNames.cpp
    ${DEKKO_UBUNTU_PATH}/app/Settings/SettingsFileBase.cpp
    ${DEKKO_UBUNTU_PATH}/app/Settings/SettingsObjectBase.cpp
    ${DEKKO_UBUNTU_PATH}/app/Settings/MailboxSettings.cpp
    ${DEKKO_UBUNTU_PATH}/app/Settings/NotificationSettings.cpp
)

set(lib_DekkoUtils
    ${DEKKO_UBUNTU_PATH}/app/Utils/AttachmentDownloader.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/FindTextPart.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/Path.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/PlainTextFormatter.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/TabEventSignaler.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/MailboxSearch.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/MailboxUtils.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/UidIterator.cpp
    ${DEKKO_UBUNTU_PATH}/app/Utils/SearchForSpecialMailbox.cpp
)

set(lib_DekkoTheme
    ${DEKKO_UBUNTU_PATH}/app/Theme/Theme.cpp
    ${DEKKO_UBUNTU_PATH}/app/Theme/Styles.h
)

set(lib_DekkoNotifications
    ${DEKKO_UBUNTU_PATH}/app/Notifications/NotificationService.cpp
    ${DEKKO_UBUNTU_PATH}/app/Notifications/NotificationWorker.cpp
    ${DEKKO_UBUNTU_PATH}/app/Notifications/PostalServiceWorker.cpp
    ${DEKKO_UBUNTU_PATH}/app/Notifications/MailboxWatcher.cpp
)

set(lib_DekkoNetwork
    ${DEKKO_UBUNTU_PATH}/app/Network/MsgPartNetAccessManagerFactory.cpp
)

set(lib_DekkoContacts
    ${DEKKO_UBUNTU_PATH}/app/Contacts/ContactsModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Contacts/ContactFilterModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Contacts/FlattenContactFilterModel.cpp
    ${DEKKO_UBUNTU_PATH}/app/Contacts/FlattenContactModel.cpp
)

set(lib_3rdParty
    ${DEKKO_UBUNTU_PATH}/3rdParty/RowsJoinerProxy.cpp
)

add_library(Settings STATIC ${lib_DekkoSettings})
target_link_libraries(Settings Imap)

add_library(3rdParty STATIC ${lib_3rdParty})
target_link_libraries(3rdParty)

add_library(Theme STATIC ${lib_DekkoTheme})
target_link_libraries(Theme)

add_library(Accounts STATIC ${lib_DekkoAccounts})
target_link_libraries(Accounts Imap Common 3rdParty Plugins Settings UiUtils Composer connectivity-qt1)

add_library(NetAccess STATIC ${lib_DekkoNetwork})
target_link_libraries(NetAccess Imap)

add_library(MailConfig STATIC ${lib_DekkoMailConfig})
target_link_libraries(MailConfig Imap Settings Common)

add_library(Utils STATIC ${lib_DekkoUtils})
target_link_libraries(Utils Imap Accounts Settings UiUtils Composer)

add_library(Components STATIC ${lib_DekkoComponents})
target_link_libraries(Components Utils ${Qt5Quick_LIBRARIES} ${Qt5Gui_LIBRARIES})

add_library(Models STATIC ${lib_DekkoModels})
add_library(Composition STATIC ${lib_DekkoComposition})
target_link_libraries(Models Accounts Settings Imap UiUtils Utils Composition)
target_link_libraries(Composition Models Imap Composer Accounts Settings MSA)

add_library(Notifications STATIC ${lib_DekkoNotifications})
target_link_libraries(Notifications Imap Utils Accounts Common Components Settings)

add_library(Contacts STATIC ${lib_DekkoContacts})

set(DEKKO_QML_COMPOSER
    ${DEKKO_QML_PATH}/Composer/AttachmentField.qml
    ${DEKKO_QML_PATH}/Composer/MessageField.qml
    ${DEKKO_QML_PATH}/Composer/ComposePanel.qml
    ${DEKKO_QML_PATH}/Composer/ComposePage.qml
    ${DEKKO_QML_PATH}/Composer/ComposePageHeadConfiguration.qml
    ${DEKKO_QML_PATH}/Composer/RecipientTypeSelector.qml
    ${DEKKO_QML_PATH}/Composer/RecipientField.qml
    ${DEKKO_QML_PATH}/Composer/RecipientInput.qml
    ${DEKKO_QML_PATH}/Composer/MessageComposer.qml
    ${DEKKO_QML_PATH}/Composer/SubjectField.qml
    ${DEKKO_QML_PATH}/Composer/DraftsPanel.qml
    ${DEKKO_QML_PATH}/Composer/RecipientTypeSelectorListView.qml
    ${DEKKO_QML_PATH}/Composer/TypedRecipientField.qml
    ${DEKKO_QML_PATH}/Composer/IdentitySelector.qml
)

set(DEKKO_QML_CONTACT
    ${DEKKO_QML_PATH}/Contact/AddressbookModel.qml
    ${DEKKO_QML_PATH}/Contact/AddressbookPage.qml
    ${DEKKO_QML_PATH}/Contact/ContactChoicePage.qml
    ${DEKKO_QML_PATH}/Contact/ContactDetailPage.qml
    ${DEKKO_QML_PATH}/Contact/ContactEditPage.qml
    ${DEKKO_QML_PATH}/Contact/PlainTextInput.qml
    ${DEKKO_QML_PATH}/Contact/imports/ContactAvatar.qml
    ${DEKKO_QML_PATH}/Contact/imports/ContactDelegate.qml
    ${DEKKO_QML_PATH}/Contact/imports/Contacts.js
    ${DEKKO_QML_PATH}/Contact/imports/SectionDelegate.qml
)

set(DEKKO_QML_SETTINGS
    ${DEKKO_QML_PATH}/Settings/ServerView.qml
    ${DEKKO_QML_PATH}/Settings/AboutPage.qml
    ${DEKKO_QML_PATH}/Settings/common/SettingsGroupBase.qml
    ${DEKKO_QML_PATH}/Settings/common/SettingsGroupView.qml
    ${DEKKO_QML_PATH}/Settings/common/SettingsListItem.qml
    ${DEKKO_QML_PATH}/Settings/global/GlobalSettingsListView.qml
    ${DEKKO_QML_PATH}/Settings/global/GlobalSettingsListModel.qml
    ${DEKKO_QML_PATH}/Settings/global/OfflineSettingsPage.qml
    ${DEKKO_QML_PATH}/Settings/global/CompositionSettingsPage.qml
    ${DEKKO_QML_PATH}/Settings/global/MarkReadSelector.qml
    ${DEKKO_QML_PATH}/Settings/global/PreviewSelector.qml
    ${DEKKO_QML_PATH}/Settings/global/CachePolicySelector.qml

    ${DEKKO_QML_PATH}/Settings/user/AccountsListPage.qml
    ${DEKKO_QML_PATH}/Settings/user/AccountListModel.qml
    ${DEKKO_QML_PATH}/Settings/user/SettingsGroupListModel.qml
    ${DEKKO_QML_PATH}/Settings/user/SenderIdentityInput.qml
    ${DEKKO_QML_PATH}/Settings/user/ServerSettings.qml
    ${DEKKO_QML_PATH}/Settings/user/OutgoingServerSettings.qml
    ${DEKKO_QML_PATH}/Settings/user/IdentitiesList.qml
    ${DEKKO_QML_PATH}/Settings/user/NewIdentityPage.qml
    ${DEKKO_QML_PATH}/Settings/user/MailboxBehaviour.qml
    ${DEKKO_QML_PATH}/Settings/user/MboxSelector.qml
    ${DEKKO_QML_PATH}/Settings/user/QuoteSelector.qml
    ${DEKKO_QML_PATH}/Settings/user/SignatureSelector.qml
    ${DEKKO_QML_PATH}/Settings/user/ProfileSettingsPage.qml
)

set(DEKKO_QML_MESSAGELISTVIEW
    ${DEKKO_QML_PATH}/MessageListView/MessageListPage.qml
    ${DEKKO_QML_PATH}/MessageListView/MessageListDelegate.qml
    ${DEKKO_QML_PATH}/MessageListView/MessagesListView.qml
    ${DEKKO_QML_PATH}/MessageListView/MultiSelectToolbar.qml
    ${DEKKO_QML_PATH}/MessageListView/MessageListActionPopover.qml
)

set(DEKKO_QML_MAILBOXVIEW
    ${DEKKO_QML_PATH}/MailboxView/MailboxListPage.qml
    ${DEKKO_QML_PATH}/MailboxView/MailboxListView.qml
    ${DEKKO_QML_PATH}/MailboxView/MailboxListDelegate.qml
    ${DEKKO_QML_PATH}/MailboxView/LoadingOverlay.qml
    ${DEKKO_QML_PATH}/MailboxView/MailboxActionPopover.qml
)

set(DEKKO_QML_MESSAGEVIEW
    ${DEKKO_QML_PATH}/MessageView/DekkoWebView.qml
    ${DEKKO_QML_PATH}/MessageView/OneMessagePage.qml
    ${DEKKO_QML_PATH}/MessageView/app.css
    ${DEKKO_QML_PATH}/MessageView/app.js
    ${DEKKO_QML_PATH}/MessageView/contact-message.js
    ${DEKKO_QML_PATH}/MessageView/UbuntuUI/core.js
)

set(DEKKO_QML_ACCOUNTSVIEW
    ${DEKKO_QML_PATH}/AccountsView/AccountsPage.qml
    ${DEKKO_QML_PATH}/AccountsView/AccountsListView.qml
    ${DEKKO_QML_PATH}/AccountsView/AccountsDelegate.qml
    ${DEKKO_QML_PATH}/AccountsView/AccountsDrawer.qml
    ${DEKKO_QML_PATH}/AccountsView/AccountsDrawerDelegate.qml
)

set(DEKKO_QML_COMPONENTS
    ${DEKKO_QML_PATH}/Components/AddressWidget.qml
    ${DEKKO_QML_PATH}/Components/AddressWidgets.qml
    ${DEKKO_QML_PATH}/Components/AddressField.qml
    ${DEKKO_QML_PATH}/Components/PercentageSectionScroller.qml
    ${DEKKO_QML_PATH}/Components/HeaderComponent.qml
    ${DEKKO_QML_PATH}/Components/HeaderRecipientRow.qml
    ${DEKKO_QML_PATH}/Components/MessageListToolbar.qml
    ${DEKKO_QML_PATH}/Components/TaskProgressBar.qml
    ${DEKKO_QML_PATH}/Components/ListItemWithActions.qml
    ${DEKKO_QML_PATH}/Components/MultipleSelectionListView.qml
    ${DEKKO_QML_PATH}/Components/MultipleSelectionVisualModel.qml
    ${DEKKO_QML_PATH}/Components/ListItemWithActionsCheckBox.qml
    ${DEKKO_QML_PATH}/Components/TitledTextField.qml
    ${DEKKO_QML_PATH}/Components/TitledTextArea.qml
    ${DEKKO_QML_PATH}/Components/EncryptionSelector.qml
    ${DEKKO_QML_PATH}/Components/CheckboxWithLabel.qml
    ${DEKKO_QML_PATH}/Components/HorizontalGradiant.qml
    ${DEKKO_QML_PATH}/Components/DekkoPage.qml
    ${DEKKO_QML_PATH}/Components/ImageWithFallBack.qml
    ${DEKKO_QML_PATH}/Components/DekkoHeader.qml
    ${DEKKO_QML_PATH}/Components/HeaderButton.qml
    ${DEKKO_QML_PATH}/Components/ActionDrawer.qml
    ${DEKKO_QML_PATH}/Components/CircleGravatar.qml
    ${DEKKO_QML_PATH}/Components/circle.png
    ${DEKKO_QML_PATH}/Components/FilterBar.qml
    ${DEKKO_QML_PATH}/Components/SectionDivider.qml
    ${DEKKO_QML_PATH}/Components/Selector.qml
    ${DEKKO_QML_PATH}/Components/MailboxPicker.qml
    ${DEKKO_QML_PATH}/Components/CachedImage.qml
)

set(DEKKO_QML_DIALOGS
    ${DEKKO_QML_PATH}/Dialogs/InfoDialog.qml
    ${DEKKO_QML_PATH}/Dialogs/SslSheet.qml
    ${DEKKO_QML_PATH}/Dialogs/SubmissionStatusDialog.qml
    ${DEKKO_QML_PATH}/Dialogs/NetworkPolicyDialog.qml
    ${DEKKO_QML_PATH}/Dialogs/DraftsDialog.qml
    ${DEKKO_QML_PATH}/Dialogs/ConfirmationDialog.qml
    ${DEKKO_QML_PATH}/Dialogs/ContentPickerDialog.qml
    ${DEKKO_QML_PATH}/Dialogs/PopupQueue.qml
    ${DEKKO_QML_PATH}/Dialogs/DialogBase.qml
    ${DEKKO_QML_PATH}/Dialogs/PasswordDialog.qml
    ${DEKKO_QML_PATH}/Dialogs/MoveMessageActionDialog.qml
)

set(DEKKO_QML_THEME
   ${DEKKO_QML_PATH}/Theme/CheckBoxStyle.qml
        ${DEKKO_QML_PATH}/Theme/SwitchStyle.qml
        ${DEKKO_QML_PATH}/Theme/PartialColorize.qml
)

set(DEKKO_QML_UTILS
    ${DEKKO_QML_PATH}/Utils/Utils.js
    ${DEKKO_QML_PATH}/Utils/DummyNetworkStatus.qml
    ${DEKKO_QML_PATH}/Utils/QueuedApplicationAlertManager.qml
)

file(GLOB_RECURSE DEKKO_ICONS ${DEKKO_ICON_DIR}/*.svg *.png *.qrc)

set(AUTOPILOT_FILES
    ${AUTOPILOT_DIR}/__init__.py
    ${AUTOPILOT_DIR}/emulators.py
    ${AUTOPILOT_DIR}/tests/__init__.py
    ${AUTOPILOT_DIR}/tests/test_dekko.py
)

set(DEKKO_SETUP_WIZARD
    ${DEKKO_QML_PATH}/SetupWizard/NewAccountTypeListView.qml
    ${DEKKO_QML_PATH}/SetupWizard/NewAccountPage.qml
    ${DEKKO_QML_PATH}/SetupWizard/AccountSetup.qml
    ${DEKKO_QML_PATH}/SetupWizard/BasicSetup.qml
    ${DEKKO_QML_PATH}/SetupWizard/ManualSetup.qml
    ${DEKKO_QML_PATH}/SetupWizard/SetupWalkThrough.qml
)
set(DEKKO_DEVELOPERMODE
    ${DEKKO_QML_PATH}/DeveloperMode/DevSettingsTabs.qml
    ${DEKKO_QML_PATH}/DeveloperMode/ImapCapabilitiesView.qml
    ${DEKKO_QML_PATH}/DeveloperMode/DeveloperSettings.qml
)

set(UCS_COMPONENTS
    ${DEKKO_QML_PATH}/UCSComponents/EmptyState.qml
    ${DEKKO_QML_PATH}/UCSComponents/RadialBottomEdge.qml
    ${DEKKO_QML_PATH}/UCSComponents/RadialAction.qml
    ${DEKKO_QML_PATH}/UCSComponents/PageWithBottomEdge.qml
)

set(DEKKO_NOTIFICATIONS
    ${DEKKO_QML_PATH}/Notifications/NotificationsService.qml
)

#make files visible to QtCreator
add_custom_target(
    ubuntu_Files
    ALL SOURCES
    ${CLICK_MANIFESTS}
    ${CLICK_PKG_MANIFEST}
    ${DEKKO_CONFIG_FILES}
    ${DEKKO_QML_PAGES}
    ${DEKKO_QML_SETTINGS}
    ${DEKKO_QML_COMPONENTS}
    ${DEKKO_QML_COMPOSER}
    ${DEKKO_QML_CONTACT}
    ${DEKKO_QML_DELEGATES}
    ${DEKKO_QML_DIALOGS}
    ${DEKKO_QML_THEME}
    ${DEKKO_QML_UTILS}
    ${DEKKO_MAIN_QML}
    ${DEKKO_DESKTOP_FILE}
    ${AUTOPILOT_FILES}
    ${DEKKO_QML_MESSAGELISTVIEW}
    ${DEKKO_QML_MESSAGEVIEW}
    ${DEKKO_QML_ACCOUNTSVIEW}
    ${DEKKO_QML_MAILBOXVIEW}
    ${DEKKO_SETUP_WIZARD}
    ${DEKKO_DEVELOPERMODE}
    ${UCS_COMPONENTS}
    ${DEKKO_ICONS}
    ${DEKKO_NOTIFICATIONS}
)

add_custom_target(ubuntu_QmlComponents ALL
   COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${DEKKO_QML_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/qml
   COMMENT "Copying QML files to build tree"
)
add_custom_target(ubuntu_Configuration ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${DEKKO_CONFIGURATION_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/configuration
    COMMENT "Copying configuration files to build tree"
)

if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/")
    set(CMAKE_INSTALL_PREFIX "/")
endif()
set(DATA_DIR ${CMAKE_INSTALL_PREFIX})
# This magic variable is set by the qtc_chroot_cmake2 script in the ubuntu sdk plugin
# http://bazaar.launchpad.net/~ubuntu-sdk-team/qtcreator-plugin-ubuntu/trunk/view/head:/share/qtcreator/ubuntu/scripts/qtc_chroot_cmake2
# It doesn't hurt to ignore this if we are not building from QtCreator.
if(QT_IMPORTS_DIR)
    set(QT_IMPORTS_DIR ${QT_IMPORTS_DIR})
endif()
set(BIN_DIR lib/${ARCH_TRIPLET}/bin)
set(ICON dekko.png)
set(ICON_DIR ${DATA_DIR})
set(DESKTOP_DIR ${DATA_DIR})

set(PROVIDER_ICONS ${DEKKO_ICON_DIR}/providers/providericons.qrc)
set(ACTION_ICONS ${DEKKO_ICON_DIR}/actions/actionicons.qrc)
qt5_add_resources(APP_RESOURCES ${ACTION_ICONS} ${PROVIDER_ICONS})
qt5_use_modules(Settings Core Quick Qml)
qt5_use_modules(3rdParty Core)
qt5_use_modules(Theme Core Quick Qml)
qt5_use_modules(Accounts Core)
qt5_use_modules(NetAccess Core Network Quick Qml)
qt5_use_modules(MailConfig Core Network Xml)
qt5_use_modules(Utils Core Network Svg Quick Qml)
qt5_use_modules(Components Core Quick Qml)
qt5_use_modules(Models Core Quick Qml)
qt5_use_modules(Composition Core)
qt5_use_modules(Notifications Core Network DBus)
qt5_use_modules(Contacts Sql)
add_executable(dekko ${dekko_SOURCES} ${APP_RESOURCES})
set_property(TARGET dekko APPEND PROPERTY COMPILE_DEFINITIONS QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII)
set_property(TARGET dekko APPEND PROPERTY INCLUDE_DIRECTORIES ${CONNECTIVITY_INCLUDE_DIRS})
set_property(TARGET dekko APPEND PROPERTY INCLUDE_DIRECTORIES ${GSETTINGS_QT_INCLUDE_DIRS})
set_property(TARGET dekko APPEND PROPERTY INCLUDE_DIRECTORIES
    ${QT_MKSPECS_DIR}/default ${CMAKE_CURRENT_SOURCE_DIR}/src/app)
target_link_libraries(dekko AppVersion Imap MSA Streams qwwsmtpclient Common Composer Plugins UiUtils Settings 3rdParty Theme Accounts NetAccess MailConfig Utils Components Models Composition Notifications Contacts ${STATIC_PLUGINS})
qt5_use_modules(dekko Network Core Qml Quick Xml)
add_executable(push-helper ${push_SOURCES})
qt5_use_modules(push-helper Core)

configure_file(${DEKKO_DESKTOP_FILE} ${CMAKE_CURRENT_BINARY_DIR}/dekko.desktop)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/dekko.desktop
    DESTINATION ${DESKTOP_DIR}
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/dekko/dekko.png
    DESTINATION ${ICON_DIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sounds
    DESTINATION ${DATA_DIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/qml
    DESTINATION ${DATA_DIR}
)
install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/configuration
    DESTINATION ${DATA_DIR}
)

install(
    TARGETS dekko
    DESTINATION ${BIN_DIR}
)

install(
    PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/push-helper
    DESTINATION ${DATA_DIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/manifest.json
    ${CLICK_MANIFESTS}
    DESTINATION ${DATA_DIR}
)

include(FindGettext)
find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
file(GLOB_RECURSE I18N_SRC_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.qml *.cpp *.h
    qml/*.qml *.js)

set(DOMAIN ${PROJECT_NAME})
set(POT_FILE dekko.dekkoproject.pot)
file(GLOB PO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/po/*.po)

if(UPDATE_TRANSLATIONS)
    add_custom_target(${POT_FILE} ALL
        COMMENT "Generating translation template"
        COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE} -o ${POT_FILE}
            -D ${CMAKE_CURRENT_SOURCE_DIR}
            -D ${CMAKE_CURRENT_BINARY_DIR}
            --from-code=UTF-8
            --c++ --qt --add-comments=TRANSLATORS:
            --keyword=tr --keyword=tr:1,2
            --keyword=qsTr --keyword=qsTr:1,2
            --package-name='dekko.dekkoproject'
            --copyright-holder='Dan Chapman.'
            ${I18N_SRC_FILES}
    )
endif()

if(Qt5LinguistForDekko_FOUND)
    qt5_wrap_po(dekko_QM ${PO_FILES})
    set(lan_summary "")

    foreach(PO_FILE ${PO_FILES})
        list(APPEND lan_summary ${PO_FILE})
    endforeach(PO_FILE)
    set(INSTALL_DIR lib/${ARCH_TRIPLET}/bin)
    list(SORT lan_summary)
    list(LENGTH lan_summary total_lan)
    if(total_lan)
        message(STATUS "Avalable languages: ${lan_summary}")
        if(CLICK_MODE)
            install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/locale/ DESTINATION ${INSTALL_DIR})
        endif()
    else()
        message(STATUS "No PO files")
    endif()
endif()
if(WITH_TESTS)
#    set(test_LibMailboxSync_SOURCES
#        tests/Utils/ModelEvents.cpp
#        tests/Utils/LibMailboxSync.cpp
#    )
#    add_library(test_LibMailboxSync STATIC ${test_LibMailboxSync_SOURCES})
#    qt5_use_modules(test_LibMailboxSync Test Network)
#    if(SUPPORTS_TARGET_INCLUDES)
#        set_property(TARGET test_LibMailboxSync APPEND PROPERTY INCLUDE_DIRECTORIES
#            ${CMAKE_CURRENT_SOURCE_DIR}/tests
#            ${CMAKE_CURRENT_SOURCE_DIR}/tests/Utils)
#    endif()
#    target_link_libraries(test_LibMailboxSync Imap MSA Streams Common Composer ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY})

#    macro(trojita_test dir fname)
#        set(test_${fname}_SOURCES tests/${dir}/test_${fname}.cpp)
#        add_executable(test_${fname} ${test_${fname}_SOURCES})
#        target_link_libraries(test_${fname} Imap MSA Streams Common Composer test_LibMailboxSync)
#        qt5_use_modules(test_${fname} Network Sql Test Widgets)
#        if(WITH_ZLIB)
#            target_link_libraries(test_${fname} ${ZLIB_LIBRARIES})
#        endif()
#        if(SUPPORTS_TARGET_INCLUDES)
#            set_property(TARGET test_${fname} APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#        endif()
#        if(NOT CMAKE_CROSSCOMPILING)
#            add_test(test_${fname} test_${fname})
#        endif()
#    endmacro()

    enable_testing()
#    trojita_test(Composer Composer_Submission)
#    trojita_test(Composer Composer_responses)
#    trojita_test(Composer Html_formatting)
#    qt5_use_modules(test_Composer_responses WebKitWidgets)
#    qt5_use_modules(test_Html_formatting WebKitWidgets)
#    trojita_test(Imap Imap_DisappearingMailboxes)
#    trojita_test(Imap Imap_Idle)
#    trojita_test(Imap Imap_LowLevelParser)
#    trojita_test(Imap Imap_Message)
#    trojita_test(Imap Imap_Model)
#    trojita_test(Imap Imap_MsgPartNetAccessManager)
#    trojita_test(Imap Imap_Parser_parse)
#    trojita_test(Imap Imap_Responses)
#    trojita_test(Imap Imap_SelectedMailboxUpdates)
#    trojita_test(Imap Imap_Tasks_CreateMailbox)
#    trojita_test(Imap Imap_Tasks_DeleteMailbox)
#    trojita_test(Imap Imap_Tasks_ListChildMailboxes)
#    trojita_test(Imap Imap_Tasks_ObtainSynchronizedMailbox)
#    trojita_test(Imap Imap_Tasks_OpenConnection)
#    trojita_test(Imap Imap_Threading)
#    trojita_test(Imap Imap_BodyParts)
#    trojita_test(Imap Imap_Offline)
#    trojita_test(Imap Imap_CopyAndFlagOperations)
#    trojita_test(Misc Rfc5322)
#    trojita_test(Misc RingBuffer)
#    trojita_test(Misc SenderIdentitiesModel)
#    trojita_test(Misc SqlCache)
#    trojita_test(Misc algorithms)
#    trojita_test(Misc rfccodecs)

    find_program(PYTHON3_EXECUTABLE python3)
    if(PYTHON3_EXECUTABLE)
        execute_process(COMMAND ${PYTHON3_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
            OUTPUT_VARIABLE PYTHON_PACKAGE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE
        )
#       install(DIRECTORY ${AUTOPILOT_DIR}
#            DESTINATION ${PYTHON_PACKAGE_DIR}
#       )
    else()
        message(SEND_ERROR "Python3 executable not found, autopilot tests will not been installed")
    endif()
endif()
