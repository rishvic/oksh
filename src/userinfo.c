#include "userinfo.h"

#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

static struct {
  struct passwd* pwd;
  char host[OK_HOST_SIZE];
} userinfo;

void UpdateUserInfo() {
  uid_t uid;

  uid = getuid();
  userinfo.pwd = getpwuid(uid);
  gethostname(userinfo.host, OK_HOST_SIZE);
  userinfo.host[OK_HOST_SIZE - 1] = '\0';
}

char* GetUser() {
  char* name;
  name = getenv("LOGNAME");
  if (name) return name;
  return userinfo.pwd->pw_name;
}

char* GetHost() { return userinfo.host; }

char* GetHome() {
  char* home;
  home = getenv("HOME");
  if (home) return home;
  return userinfo.pwd->pw_dir;
}
