#ifndef OKSH_PARSER_H_
#define OKSH_PARSER_H_

#include <stddef.h>

typedef enum {
  kParseDone,
  kParseQuote,
  kParseDQuote,
  kParseBQuote,
  kParseCmdSubst
} ParseType;
typedef enum { kDoneCmd, kPipeCmd, kAndCmd, kOrCmd, kJobCmd } CmdNext;

typedef struct command_t {
  int argc;
  char **argv;
  char *infile, *outfile, *errfile;
} Command;

Command *ParseLine(const char *line, size_t sz, size_t *i, CmdNext *rel);

void FreeCmd(Command *cmd);

#endif /* OKSH_PARSER_H_ */
