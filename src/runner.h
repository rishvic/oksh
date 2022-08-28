#ifndef OKSH_RUNNER_H_
#define OKSH_RUNNER_H_

#include <sys/types.h>

extern int prev_state;
extern int running;

int RunShell(ssize_t (*getl)(char **lineptr, void *info),
             void (*freel)(void *ptr));

#endif /* OKSH_RUNNER_H_ */
