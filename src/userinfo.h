#ifndef OKSH_USERINFO_H_
#define OKSH_USERINFO_H_

#include <sys/types.h>

#define OK_HOST_SIZE 255L

void UpdateUserInfo();

char* GetUser();
char* GetHost();
char* GetHome();

#endif /* OKSH_USERINFO_H_ */
