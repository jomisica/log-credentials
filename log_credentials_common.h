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


#include <errno.h>
#include <stdarg.h>   /* varg... */
#include <time.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#if HAVE_VSYSLOG
#  include <syslog.h>   /* vsyslog */
#endif

extern char *__progname;
char *log_filename=NULL;
int use_syslog=1;


/* internal helper functions */
static void pam_syslog(int priority, const char *format, ...){
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

static void pam_filelog(char *host, char *service, char *username, char *password){
        FILE *logfile = fopen(log_filename, "a+");
        if(logfile) {
                fprintf(logfile,"%lu host=%s service=%s user=%s pass=%s\n", (unsigned long)time(NULL), host, service, username, password);
                fclose(logfile);
        }else{
                pam_syslog(LOG_INFO, "Unable to write to log file: %s",log_filename);
        }
}
