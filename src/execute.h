#ifndef OKSH_EXECUTE_H_
#define OKSH_EXECUTE_H_

#include "parser.h"

#include <sys/types.h>

pid_t ExecuteCmd(const Command *cmd, int infd, int outfd);

#endif /* OKSH_EXECUTE_H_ */
