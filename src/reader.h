#ifndef OKSH_READER_H_
#define OKSH_READER_H_

#include <sys/types.h>

typedef enum {
  kDone,
  kNotDone,
  kPipe,
  kCmdAnd,
  kCmdOr,
  kQuote,
  kDQuote,
  kBQuote,
  kCmdSubst
} ReadType;

int IsFinalReadType(ReadType rt);

ssize_t ReadCmd(char **lineptr, size_t *n,
                ssize_t (*getl)(char **lineptr, void *info),
                void (*freel)(void *ptr));

#endif /* OKSH_READER_H_ */
