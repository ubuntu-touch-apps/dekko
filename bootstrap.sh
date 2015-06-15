#!/bin/bash

set -e

SERIES=utopic
CLICK_CHROOT_NAME=click
CLICK_SDK_ARCH=${CLICK_SDK_ARCH:=armhf}
CLICK_SDK_FRAMEWORK=${CLICK_SDK_FRAMEWORK:=ubuntu-sdk-15.04}

echo -e "\033[1;32m [ ${CLICK_SDK_ARCH} ] Bootstrapping development environment for Dekko email client\033[0m"


update_chroot() {
    echo -e "\033[0;31m [ ${CLICK_SDK_ARCH} ] Updating click chroot\033[0m"

    click chroot -a ${CLICK_SDK_ARCH} -f ${CLICK_SDK_FRAMEWORK} -n ${CLICK_CHROOT_NAME} upgrade
    click chroot -a ${CLICK_SDK_ARCH} -f ${CLICK_SDK_FRAMEWORK} -n ${CLICK_CHROOT_NAME} maint apt-get install -y \
    qtpim5-dev:${CLICK_SDK_ARCH} \
    qml-module-qtcontacts \
    qttools5-dev-tools \
    libconnectivity-qt1-dev:${CLICK_SDK_ARCH} \
    libgsettings-qt-dev:${CLICK_SDK_ARCH} \
    qtdeclarative5-private-dev:${CLICK_SDK_ARCH} \
    qtscript5-private-dev:${CLICK_SDK_ARCH} \
    qml-module-qtquick-privatewidgets:${CLICK_SDK_ARCH} \
    qtbase5-private-dev:${CLICK_SDK_ARCH} \
    qtbase5-dev:${CLICK_SDK_ARCH} \
    qtbase5-dbg:${CLICK_SDK_ARCH} \
    qtbase5-dev-tools-dbg \
    qtbase5-dbg:${CLICK_SDK_ARCH} \
    qtbase5-dev-tools \
    intltool \
    bzr \
    --no-install-recommends
}

create_chroot() {
    echo -e "\033[1;31m [ ${CLICK_SDK_ARCH} ] Creating click chroot " \
        "${CLICK_SDK_FRAMEWORK}-${CLICK_SDK_ARCH} and installing dependencies\033[0m"
    sudo /usr/share/qtcreator/ubuntu/scripts/click_create_target \
            ${CLICK_SDK_ARCH} ${CLICK_SDK_FRAMEWORK} $SERIES $CLICK_CHROOT_NAME
    update_chroot

}

if ! click chroot -a ${CLICK_SDK_ARCH} -f ${CLICK_SDK_FRAMEWORK} -n ${CLICK_CHROOT_NAME} exists; then
    create_chroot
fi

if [ "$1" == "update" ]; then
    update_chroot
fi

echo -e "\033[1;32m [ ${CLICK_SDK_ARCH} ] Bootstrapping complete.\033[0m" 
