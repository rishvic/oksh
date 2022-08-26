#ifndef OKSH_RUNNER_H_
#define OKSH_RUNNER_H_

#include <sys/types.h>

int RunShell(ssize_t (*getl)(char **linept), void (*freel)(void *ptr));

#endif /* OKSH_RUNNER_H_ */
