#!/bin/bash

set -e

CLICK_SDK_FRAMEWORK=${CLICK_SDK_FRAMEWORK:=ubuntu-sdk-15.04}
FAT_DIR=${FAT_DIR:=/tmp/fatpackage}

echo -e "\033[1;32m [ ${CLICK_SDK_FRAMEWORK} ] Building super duper fat click package\033[0m"

for ARCH in armhf i386 amd64
do
    echo -e "\033[0;31m [ $ARCH ] Building..........\033[0m"
    BUILD_DIR=build-$ARCH
    mkdir ${BUILD_DIR}
    cd ${BUILD_DIR}
    click chroot -a $ARCH -f ${CLICK_SDK_FRAMEWORK} run cmake ..
    click chroot -a $ARCH -f ${CLICK_SDK_FRAMEWORK} run make
    click chroot -a $ARCH -f ${CLICK_SDK_FRAMEWORK} run make DESTDIR=${FAT_DIR} install
    cd ..
    echo -e "\033[0;31m [ $ARCH ] Cleaning up build dir\033[0m"
    rm -rf ${BUILD_DIR}
done

echo -e "\033[0;31m [ ${CLICK_SDK_FRAMEWORK} ] Modifying manifest.json\033[0m"
sed -i 's/^    "architecture.*/    "architecture": ["armhf", "i386", "amd64"],/' ${FAT_DIR}/manifest.json

echo -e "\033[0;31m [ ${CLICK_SDK_FRAMEWORK} ] Build *multi.click package\033[0m"
click build ${FAT_DIR}
echo -e "\033[1;32m And were done!! \o/ \033[0m"
