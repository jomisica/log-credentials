# log-credentials

	PAM log-credentials has been written by José Miguel Silva Caldeira <miguel@ncdc.pt>.

## Description:
	PAM log-credentials allows you to log credentials after authorization.
	It will log messages like that:
	May 22 18:50:15 localhost log-credentials: host=xxx.xxx.xxx.xxx service=sshd user=root pass=xxxxxxx.

	The idea of this module is to facilitate obtaining information that allows you
	to analyze brute force attacks on ssh and other services.

	Through these data, with scripts and in a simple way it is possible to perceive:
	From where the attacks are initiated, the countries, the networks, etc. Through IP;
	You can see which users are most used in login attempts;
	The type of passwords that are used in attack attempts;
	Among others.

	In this way it is possible for the administrator to be aware and to take measures
	so that good practices are taken to avoid these attacks.

	Use at your own risk!

## Installation

### Using Git

	You can clone the repository wherever you want. (I like to keep it in
	`~/Projects/log-credentials`.)

```bash
	$ git clone https://github.com/jomisica/log-credentials.git
	$ cd log-credentials
	$ libtoolize --force
	$ aclocal
	$ autoheader
	$ automake --force-missing --add-missing
	$ autoconf
	$ ./configure
	$ make
	# make install
	```
## Problem/BUGS report:
	If you find any bugs or problems just mail me
	José Miguel Silva Caldeira <miguel@ncdc.pt>
