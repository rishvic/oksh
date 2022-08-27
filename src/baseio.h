#ifndef OKSH_BASEIO_H_
#define OKSH_BASEIO_H_

#include <sys/types.h>

ssize_t BaseIOGetline(char **lineptr, void *info);
void BaseIOFree(void *ptr);

#endif /* OKSH_BASEIO_H_ */
