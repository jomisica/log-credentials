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
 
#include <nss.h>
#include <pwd.h>
#include <stdio.h>

enum nss_status _nss_log_credentials_getpwnam_r(const char *name, struct passwd *result, char *buffer, size_t buflen, int *errnop) {
  if(name == NULL) {
      *errnop = EINVAL;
      return NSS_STATUS_UNAVAIL;
  }

  result->pw_name=name;
  result->pw_uid=60000;
  result->pw_gid=60000;
  result->pw_dir="/";
  result->pw_shell="/sbin/nologin";

  return NSS_STATUS_SUCCESS;
}
