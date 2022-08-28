#include "runner.h"
#include "parser.h"
#include "reader.h"

#include <stdio.h>
#include <stdlib.h>

#define OK_PIPE_STK_SZ 32L

int prev_state;

int RunShell(ssize_t (*getl)(char **lineptr, void *info),
             void (*freel)(void *ptr)) {
  ssize_t sz;
  char *line = NULL;
  size_t n = 0, i;
  Command *cmd;
  CmdNext newrel;
#ifdef DEBUG
  int j;
#endif

  prev_state = EXIT_SUCCESS;

  while ((sz = ReadCmd(&line, &n, getl, freel)) != -1) {
    for (i = 0; i < (size_t)sz;) {
      cmd = ParseLine(line, sz, &i, &newrel);
      if (!cmd) continue;

#ifdef DEBUG
      fprintf(stderr, "Command = [");
      for (j = 0; j < cmd->argc; j++) {
        fprintf(stderr, " '%s'%s", cmd->argv[j], j == cmd->argc - 1 ? "" : ",");
      }
      fprintf(stderr, " ]");
      if (cmd->infile) fprintf(stderr, ", infile = %s", cmd->infile);
      if (cmd->outfile) fprintf(stderr, ", outfile = %s", cmd->outfile);
      if (cmd->errfile) fprintf(stderr, ", errfile = %s", cmd->errfile);
      fprintf(stderr, "\n");
#endif

      if (cmd) FreeCmd(cmd);
    }
  }
  freel(line);

  return 0;
}
