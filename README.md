# Dekko email client for Ubuntu devices #

[![Build Status](http://dekko-ci.cloudapp.net:8080/job/d-test-devel/badge/icon)](http://dekko-ci.cloudapp.net:8080/job/d-test-devel/)

This is a general README. For in-depth instructions on hacking, see HACKING.md

Pre-requisites
==============
These instructions have been tested on:

    1. Ubuntu 14.10 (Utopic Unicorn)
    2. Ubuntu 15.04 (Vivid Vervet)

Ensure you have the Ubuntu SDK installed, as per [instructions found here](http://developer.ubuntu.com/start/ubuntu-sdk/installing-the-sdk/)

There are also some additional dependencies

    libconnectivity-qt1-dev (vivid chroots only)
    qtpim5-dev
    qml-module-qtcontacts
    qttools5-dev-tools

Bootstrap set-up
================
If your lazy like me and don't want to have to create a click chroot and install the dependencies

    ./bootstrap.sh

Once this is complete you will find the new click chroot kit is available in the Ubuntu IDE. 

If you already have one created then you can just update the current chroot by running

    ./bootstrap.sh update

By default this sets up an armhf click chroot. If you want to target another architecture you can set the CLICK_SDK_ARCH environment variable

    CLICK_SDK_ARCH=i386 ./bootstrap.sh

Build & run on desktop
======================
We have a simple build procedure (which is nice)

    mkdir __build && cd __build
    cmake ..
    make -j2
    ./dekko

NOTE: you will need to install the additional dependencies yourself for the desktop

Build & run on phablets
=======================

You can just use the "Run" config of the Ubuntu IDE for the target architecture

Hacking
=======
Please find more in-depth instructions in HACKING.md

Dependencies
===========
qtbase5-private-dev qtdeclarative5-private-dev libgsettings-qt-dev libgsettings-qt1 account-plugin-tools account-plugin-ubuntuone accountsservice-ubuntu-schemas accountsservice-ubuntu-touch-schemas address-book-service adwaita-icon-theme apparmor-easyprof apparmor-easyprof-ubuntu click click-apparmor dbus-property-service dconf-cli evolution-data-server evolution-data-server-common evolution-data-server-online-accounts folks-common gir1.2-accounts-1.0 gir1.2-click-0.4 gir1.2-gee-0.8 gir1.2-json-1.0 gir1.2-packagekitglib-1.0 gir1.2-signon-1.0 gsettings-ubuntu-schemas history-service hud humanity-icon-theme indicator-bluetooth indicator-network indicator-power libaccounts-qt5-1 libandroid-properties1 libboost-locale1.55.0 libboost-log1.55.0 libboost-regex1.55.0 libboost-serialization1.55.0 libcamel-1.2-49 libcapnp-0.4.0 libcgmanager0 libclick-0.4-0 libcolumbus1 libcolumbus1-common libconnectivity-qt1 libconnectivity-qt1-dev libcontent-hub0 libdbus-cpp4 libdee-1.0-4 libebackend-1.2-7 libebook-1.2-14 libebook-contacts-1.2-0 libecal-1.2-16 libedata-book-1.2-20 libedata-cal-1.2-23 libedataserver-1.2-18 libfolks-eds25 libfolks25 libgdata-common libgdata19 libgee-0.8-2 libgeocode-glib0 libgflags2 libgoa-1.0-0b libgoa-1.0-common libgoogle-glog0 libgweather-3-6 libgweather-common libhardware2 libhistoryservice0 libhud2 libhybris libhybris-common1 libhybris-utils libjson0 libjsoncpp0 liblttng-ust-ctl2 liblttng-ust0 libmedia1 libmediascanner-2.0-3 libmessaging-menu0 libmirplatform6 libmirserver30 libnet-cpp1 liboauth0 libofono-qt1 libonline-accounts-client1 liboxideqt-qmlplugin liboxideqtcore0 liboxideqtquick0 libpackagekit-glib2-16 libpay2 libpgm-5.1-0 libphonenumber6 libpocketsphinx1 libprocess-cpp2 libqdjango-db0 libqmenumodel0 libqofono-qt5-0 libqt5contacts5 libqt5feedback5 libqt5keychain0 libqt5location5 libqt5location5-plugins libqt5organizer5 libqt5positioning5 libqt5positioning5-plugins libqt5systeminfo5 libqt5versit5 libqt5versitorganizer5 libqt5websockets5 librsvg2-common libsignon-extension1 libsignon-glib1 libsignon-plugins-common1 libsignon-qt5-1 libsodium13 libsphinxbase1 libsystemsettings1 libthumbnailer0 libtimezonemap-data libtimezonemap1 libtrust-store1 libu1db-qt5-3 libubuntu-app-launch2 libubuntu-application-api2 libubuntu-download-manager-client0 libubuntu-download-manager-common0 libubuntu-location-service2 libubuntu-platform-hardware-api2 libubuntuoneauth-2.0-0 libudm-common0 libudm-priv-common0 libunity-action-qt1 libunity-api0 libunity-protocol-private0 libunity-scopes-json-def-desktop libunity-scopes3 libunity9 libunityvoice1 libunwind8 liburcu2 liburl-dispatcher1 libusermetricsinput1 libusermetricsoutput1 libzeitgeist-2.0-0 libzmq3 libzmqpp3 mediascanner2.0 mir-platform-graphics-mesa1 ofono oxideqt-codecs packagekit-tools pay-service powerd python-xdg python-zeitgeist python3-apparmor python3-apparmor-click python3-aptdaemon.pkcompat python3-click python3-debian python3-gnupg python3-libapparmor python3-xdg qmenumodel-qml qml-module-qt-labs-folderlistmodel qml-module-qt-labs-settings qml-module-qt-websockets qml-module-qtfeedback qml-module-qtlocation qml-module-qtorganizer qml-module-qtpositioning qml-module-qtquick-localstorage qml-module-qtquick-particles2 qml-module-qtquick-xmllistmodel qml-module-qtsensors qml-module-qtsysteminfo qml-module-ubuntu-connectivity qml-module-ubuntu-onlineaccounts qml-module-ubuntu-onlineaccounts-client qtcontact5-galera qtdeclarative5-accounts-plugin qtdeclarative5-folderlistmodel-plugin qtdeclarative5-gsettings1.0 qtdeclarative5-localstorage-plugin qtdeclarative5-ofono0.2 qtdeclarative5-online-accounts-client0.1 qtdeclarative5-particles-plugin qtdeclarative5-poppler1.0 qtdeclarative5-qtfeedback-plugin qtdeclarative5-qtmir-plugin qtdeclarative5-qtorganizer-plugin qtdeclarative5-systeminfo-plugin qtdeclarative5-u1db1.0 qtdeclarative5-ubuntu-content1 qtdeclarative5-ubuntu-download-manager0.1 qtdeclarative5-ubuntu-mediascanner0.1 qtdeclarative5-ubuntu-push-plugin qtdeclarative5-ubuntu-settings-components qtdeclarative5-ubuntu-syncmonitor0.1 qtdeclarative5-ubuntu-telephony-phonenumber0.1 qtdeclarative5-ubuntu-telephony0.1 qtdeclarative5-ubuntu-thumbnailer0.1 qtdeclarative5-ubuntu-ui-extras-browser-plugin qtdeclarative5-ubuntu-ui-toolkit-plugin qtdeclarative5-ubuntu-web-plugin qtdeclarative5-ubuntu-web-plugin-assets qtdeclarative5-ubuntuone1.0 qtdeclarative5-unity-action-plugin qtdeclarative5-unity-notifications-plugin qtdeclarative5-usermetrics0.1 qtdeclarative5-window-plugin qtdeclarative5-xmllistmodel-plugin qtmir-desktop session-migration signon-plugin-oauth2 signon-plugin-password signon-ui signon-ui-service signon-ui-x11 signond sphinx-voxforge-hmm-en sphinx-voxforge-lm-en sqlite3 suru-icon-theme system-image-common system-image-dbus telepathy-ofono telephony-service thumbnailer-common thumbnailer-service tone-generator ubuntu-app-launch ubuntu-app-launch-tools ubuntu-application-api2-test ubuntu-download-manager ubuntu-html5-container ubuntu-html5-ui-toolkit ubuntu-keyboard-data ubuntu-mobile-icons ubuntu-mono ubuntu-sdk-libs ubuntu-sounds ubuntu-system-settings ubuntu-system-settings-online-accounts ubuntu-touch-sounds ubuntu-ui-toolkit-theme ubuntuone-client-data ubuntuone-credentials-common unity-asset-pool unity-plugin-scopes unity-schemas unity-scope-click unity-scope-click-departmentsdb unity-scope-mediascanner2 unity-scope-scopes unity-voice-service unity-webapps-qml unity8 unity8-common unity8-private upstart upstart-bin urfkill usermetricsservice zeitgeist zeitgeist-core zeitgeist-datahub
