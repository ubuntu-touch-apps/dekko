# Dekko email client for Ubuntu devices #

[![Build Status](http://dekko-ci.cloudapp.net:8080/job/d-test-devel/badge/icon)](http://dekko-ci.cloudapp.net:8080/job/d-test-devel/)

Donate
======
You can [support Dekko with donations via PayPal](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dpniel%40ubuntu%2ecom&lc=GB&item_name=Dekko%20email%20client&item_number=Dekko&currency_code=GBP&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted).

[![Support Dekko now.](http://manlybeachrunningclub.com/wp-content/uploads/2015/01/paypal-donate-button115.png)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dpniel%40ubuntu%2ecom&lc=GB&item_name=Dekko%20email%20client&item_number=Dekko&currency_code=GBP&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted)

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