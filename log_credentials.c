/*
 *  Written by José Miguel Silva Caldeira <miguel@ncdc.pt> 2017/05/22
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program - see the file LICENSE.
 *
 * As figuras imaginárias têm mais relevo e verdade que as reais.
 * "Fernando Pessoa"
 */

/* --- includes --- */
#include <stdio.h>
#include <stdarg.h>   /* varg... */
#include <string.h>   /* strcmp,strncpy,... */

#include <security/pam_appl.h>  /* pam_* */
#include <security/pam_modules.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#if HAVE_VSYSLOG
#  include <syslog.h>   /* vsyslog */
#endif

/* internal helper functions */
static void pam_syslog(int priority, const char *format, ...) {
								va_list args;
								va_start(args, format);

#if HAVE_VSYSLOG

								openlog(PACKAGE, LOG_CONS, LOG_AUTHPRIV);
								vsyslog(priority, format, args);
								closelog();
#else
								vfprintf(stderr, format, args);
#endif
}

/* --- authentication management functions --- */
PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc,
																								const char **argv)
{
								int retval;
								char *password;
								char *host;
								char *username;
								char *service;

								retval=pam_get_item(pamh, PAM_SERVICE, (void*) &service);
								if(retval != PAM_SUCCESS) {
																pam_syslog(LOG_ALERT, "pam_get_item(service) returned error: %s", pam_strerror(pamh,retval));
								}
								if (service == NULL) {
																pam_syslog(LOG_ALERT, "service not known");
								}

								retval=pam_get_item(pamh, PAM_USER, (void*) &username);
								if(retval != PAM_SUCCESS) {
																pam_syslog(LOG_ALERT, "pam_get_item(username) returned error: %s", pam_strerror(pamh,retval));
								}
								if (username == NULL) {
																pam_syslog(LOG_ALERT, "username not known");
								}


								pam_get_item(pamh, PAM_RHOST, (void*) &host);
								if(retval != PAM_SUCCESS) {
																pam_syslog(LOG_ALERT, "pam_get_item(host) returned error: %s", pam_strerror(pamh,retval));
								}
								if (host == NULL) {
																pam_syslog(LOG_ALERT, "host not known");
								}

								pam_get_item(pamh, PAM_AUTHTOK, (void*) &password);
								if(retval != PAM_SUCCESS) {
																pam_syslog(LOG_ALERT, "pam_get_item(password) returned error: %s", pam_strerror(pamh,retval));
								}
								if (password == NULL) {
																pam_syslog(LOG_ALERT, "password not known");
								}

								pam_syslog(LOG_INFO,"host=%s service=%s user=%s pass=%s", host, service, username, password);

								return PAM_SUCCESS;

}

PAM_EXTERN
int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc,
																			const char **argv)
{
								return PAM_SUCCESS;
}

/* end of module definition */
#ifdef PAM_STATIC

/* static module data */
struct pam_module _pam_script_modstruct = {
								"log-credentials",
								pam_sm_authenticate,
								pam_sm_setcred
};

#endif
