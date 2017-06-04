# log-credentials
PAM log-credentials has been written by José Miguel Silva Caldeira <miguel@ncdc.pt>.

## Description:
PAM log-credentials allows you to log credentials.
It will log messages like that:
**May 22 18:50:15 localhost log-credentials: host=xxx.xxx.xxx.xxx service=sshd user=root pass=xxxxxxx**.

The idea of this module is to facilitate obtaining information that allows you to analyze brute force attacks on ssh and other services.

Through these data, with scripts and in a simple way it is possible to perceive:
* From where the attacks are initiated, the countries, the networks, etc. Through IP;
* You can see which users are most used in login attempts;
* The type of passwords that are used in attack attempts;
* Among others.

In this way it is possible for the administrator to be aware and to take measures so that good practices are taken to avoid these attacks.

Use at your own risk!

Do not use on a production system where multiple users have access to the logs. If you do, everyone has access to all credentials.

If you still want to do so, configure the log file access permissions.

## Ethic
Since this module logs all the credentials it is clear that it also logs the correct ones.

This module can be used as "KeyLogger" if it is badly installed on some system.

My intention is to use it for study purposes but stay of the conscience of each one uses it for a certain purpose.

Be cool!

## Installation

### Clone the project
You can clone the repository wherever you want. (I like to keep it in `~/Projects/log-credentials`.)

```Bash
$ cd ~/Projects
$ git clone https://github.com/jomisica/log-credentials.git
```

### Dependent software
In order to compile this software it is necessary to first install the dependencies. This will depend on the system in question below is the example for CentOS and Ubuntu.

#### CentOS 7
In CentOS it is necessary to install the "Development Tools" group that installs the necessary software to compile this software.

It is also necessary to install the package "pam-devel" that has the necessary headers so that it is possible to develop modules for the PAM in the system that we use.

```Bash
# yum group install "Development Tools"
# yum install pam-devel
```

#### Ubuntu
In Ubuntu it is necessary to install the software dependent to compile this software.

It is also necessary to install the package "libpam0g-dev" that has the necessary headers so that it is possible to develop modules for the PAM in the system that we use.

```bash
# apt-get build-dep pam
# apt-get install libpam0g-dev
```

### Build
```Bash
$ cd log-credentials
$ libtoolize --force
$ aclocal
$ autoheader
$ automake --force-missing --add-missing
$ autoconf
```

The directory where the PAM and NSS modules are installed are different on several systems, as such, the correct location may be required when configuring.

```bash
$ ./configure
...
...
...
...
configure: Summary of build options:
  Version:              1.0.0
  Host type:            x86_64-unknown-linux-gnu
  Compiler:             gcc
  CFLAGS:               -g -O2
  Library types:        Shared=yes, Static=no
  PAM Module Directory: /lib64/security
  NSS Module Directory: /lib64
```

*configure* tries to find the correct location on the system where the modules are installed.
It is possible to see the result in the summary, if the result is not correct for the system in question we have to pass the correct place to configure.

#### Example
```bash
  $ ./configure --with-pam-dir=/lib64/security --with-nss-dir=/lib64
```

```bash
$ make
# make install
```

## Configure PAM module
In order for the module to work we need to add the following line to the /etc/pam.d/sshd configuration file before any other module or file inclosion.

```
auth       optional     log_credentials.so
```

## Configure NSS module
Linux systems can use various types of databases to store users' credentials. Among them files, by default the users and passwords are stored in the passwd, shadow etc, files. Ldap a service that is important when we always have access with the same credentials on several machines in a network. Among others.

However when a user is not found on any of the configured systems files, ldap, etc it is not possible to know for sure who is trying to log in. In this way the user is passed to the PAM but with the password changed.

This way it would not be possible to have the username / password pair. As such it is necessary to configure the NSS module so that it is possible to work around this problem and have access to the user / password pair entered by the user.

The module has to be configured as the last module so that it is possible to log into the system with normal users. Only if it is not found in any other module then it is used this small module that will conturn this problem passing the missing data to as id, gid etc.

For the module to work, we need to modify the following line in the /etc/nsswitch.conf configuration file.

```
passwd:     files sss log_credentials
```

As we can see the module log_credentials is in last

**It's done!**


## Problem/BUGS report:
If you find any bugs or problems just mail me José Miguel Silva Caldeira <miguel@ncdc.pt>
