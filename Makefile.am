###########################################################################
##
## File:        ./Makefile.am
## Versions:    $Id: configure.ac,v 1.14 2017/05/22 03:50:05 $
## Created:     2017/05/22
## Author:      José Miguel Silva Caldeira <miguel@ncdc.pt>
##
###########################################################################

ACLOCAL_AMFLAGS= -I config
mdir = $(PAMDIR)
m_LTLIBRARIES = log_credentials.la
log_credentials_la_SOURCES = log_credentials.c
log_credentials_la_LDFLAGS = -module -avoid-version

help:
	@echo ""
	@echo "make		- compile sources to a shared library module"
	@echo "make all		- same as above"
	@echo "make install	- copy module to destination"
	@echo "make clean	- cleans out most useless files"
	@echo ""
