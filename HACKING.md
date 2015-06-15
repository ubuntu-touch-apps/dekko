# Hacking on Dekko email client for Ubuntu devices

Pre-requisites
==============
These instructions have been tested on:

    1. Ubuntu 14.10 (Utopic Unicorn)
    2. Ubuntu 15.04 (Vivid Vervet)

Ensure you have the Ubuntu SDK installed, as per [instructions found here](http://developer.ubuntu.com/start/ubuntu-sdk/installing-the-sdk/)

To contribute you will need to create an account on our Jira bug tracker [here](https://dekkoproject.atlassian.net/admin/users/sign-up)

Once setup this will give you access to [Jira](https://dekkoproject.atlassian.net), [Wiki](https://dekkoproject.atlassian.net/wiki/) and [Bitbucket](https://bitbucket.org/dekkoproject/dekko) code repository

Take a look through our bugs or create a new one you would like to work on. Then 
create a branch using the "Create branch" link in the Jira issue. This enables 
Jira to keep track of your branch and automatically move it along the kanban boards

Branches
========

All new branches should be branched from devel, and pull requests made against devel.
devel has been set as our "master" branch in bitbucket so it should automatically do this for you.

When creating a new branch it *should* always start with the Jira issue number, i.e "DEK-123-Create-contact"

The actual master branch is our release branch and we will merge devel into master each time we release
a new update to the store.

Debugging
=========
Dekko will log debug output on the phone to:

    /home/phablet/.cache/upstart/

The file name will resemble:

    application-click-dekko.dekkoproject-0.5.1.log


Building
=========

    git clone https://bitbucket.org/dekkoproject/dekko.git
    cd dekko

We now need to:

 1. ensure we have the armhf click chroot created
 2. the click chroot is up to date and has required dependencies installed

Run this once:

    ./bootstrap update

Now add this flag to the cmake arguments under Projects in the Ubuntu IDE:
    
    -DDEV_FATAL_WARNINGS=on

This ensures all warnings are fatal, for a general user building dekko we leave this turned
off as it would mean nothing to them and slightly speeds things up.

Then just use the default Run config in the Ubuntu IDE (you may need to select "dekko2")
