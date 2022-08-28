#ifndef OKSH_READER_H_
#define OKSH_READER_H_

#include <sys/types.h>

typedef enum {
  kDone,
  kPipe,
  kCmdAnd,
  kCmdOr,
  kQuote,
  kDQuote,
  kBQuote,
  kCmdSubst
} ReadType;

typedef struct command_t {
  int argc;
  char **argv;
  char *in, *out, *err;
  struct command_t *pipecmd;
} Command;

int IsFinalReadType(ReadType rt);

ssize_t ReadCmd(char **lineptr, size_t *n,
                ssize_t (*getl)(char **lineptr, void *info),
                void (*freel)(void *ptr));

#endif /* OKSH_READER_H_ */
