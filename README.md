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
The directory where the PAM modules are installed are different in several systems as such we have to pass the correct directory when configuring.

#### CentOS 64bits
```bash
  $ ./configure --with-pam-dir=/lib64/security
```

#### Ubuntu 64bits
```bash
  $ ./configure --with-pam-dir=/lib/x86_64-linux-gnu/security
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

**It's done!**

## Problem/BUGS report:
If you find any bugs or problems just mail me José Miguel Silva Caldeira <miguel@ncdc.pt>
