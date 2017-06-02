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
