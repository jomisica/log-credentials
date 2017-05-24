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
#include <stdlib.h>   /* calloc, setenv, putenv */

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

static int pam_set_authtok(pam_handle_t *pamh, int flags,
	int argc, const char **argv, char *prompt, int authtok)
{
	int retval;
	struct pam_message message[1],*pam_msg[1];
	struct pam_response *response = NULL;
	struct pam_conv *conversation;

	pam_msg[0] = &message[0];
	message[0].msg_style = PAM_PROMPT_ECHO_OFF;
	message[0].msg = prompt;

	retval = pam_get_item(pamh, PAM_CONV, (const void **)(void *) &conversation);
	if (retval == PAM_SUCCESS) {
		retval = conversation->conv(1, (const struct pam_message **) pam_msg, &response, conversation->appdata_ptr);
		if (retval == PAM_SUCCESS) {
			if ((flags & PAM_DISALLOW_NULL_AUTHTOK) && response[0].resp == NULL) {
				free(response);
				return PAM_AUTH_ERR;
			}else{
				retval = pam_set_item(pamh, authtok, response[0].resp);
				free(response);
				return retval;
			}
		}else{
			pam_syslog(LOG_INFO, "conversation returned error: %s", pam_strerror(pamh,retval));
			return retval;
		}
	}else{
		pam_syslog(LOG_INFO, "pam_get_item(conversation) returned error: %s", pam_strerror(pamh,retval));
		return retval;
	}
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
		pam_syslog(LOG_INFO, "pam_get_item(service) returned error: %s", pam_strerror(pamh,retval));
	}

	retval=pam_get_item(pamh, PAM_USER, (void*) &username);
	if(retval != PAM_SUCCESS) {
		pam_syslog(LOG_INFO, "pam_get_item(username) returned error: %s", pam_strerror(pamh,retval));
	}

	retval=pam_get_item(pamh, PAM_RHOST, (void*) &host);
	if(retval != PAM_SUCCESS) {
		pam_syslog(LOG_INFO, "pam_get_item(host) returned error: %s", pam_strerror(pamh,retval));
	}

	/* --- get password --- */
	retval=pam_get_item(pamh, PAM_AUTHTOK, (void*) &password);
	if (!password) {
		retval = pam_set_authtok(pamh, flags, argc, argv, "Password: ", PAM_AUTHTOK);
		if(retval != PAM_SUCCESS) {
			pam_syslog(LOG_INFO, "pam_get_item(password) returned error: %s", pam_strerror(pamh,retval));
		}else{
			retval=pam_get_item(pamh, PAM_AUTHTOK, (void*) &password);
			if(retval != PAM_SUCCESS) {
				pam_syslog(LOG_INFO, "pam_get_item(password) returned error: %s", pam_strerror(pamh,retval));
			}
		}
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
