#ifndef OKSH_RLIO_H_
#define OKSH_RLIO_H_

#include <sys/types.h>

void ClearLine(int);
ssize_t RlGetline(char **lineptr, void *info);
void RlFree(void *ptr);

#endif /* OKSH_RLIO_H_ */
