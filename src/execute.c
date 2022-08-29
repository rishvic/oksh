#include "execute.h"
#include "parser.h"
#include "runner.h"
#include "userinfo.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <readline/history.h>

static int BuiltinCd(const Command *cmd) {
  int err;

  err = chdir(cmd->argc > 1 ? cmd->argv[1] : GetHome());

  if (err) perror("cd");
  prev_state = err;
  return err;
}

static int BuiltinExit(const Command *cmd) {
  prev_state = EXIT_SUCCESS;
  if (cmd->argc > 1) {
    prev_state = strtol(cmd->argv[1], NULL, 10);
    if (errno == EINVAL) prev_state = EXIT_SUCCESS;
  }
  running = 0;
  return prev_state;
}

static int BuiltinHistory() {
  size_t i;
  HIST_ENTRY **list;

  list = history_list();
  for (i = 0; list[i]; i++) printf("%s\n", list[i]->line);
  return 0;
}

pid_t ExecuteCmd(const Command *cmd, int infd, int outfd) {
  int stat;
  pid_t cid;
  FILE *fp;

  if (!cmd || !cmd->argv) return -1;

  if (!strncmp(cmd->argv[0], "cd", 2)) {
    BuiltinCd(cmd);
    return 0;
  }
  if (!strncmp(cmd->argv[0], "exit", 4)) {
    BuiltinExit(cmd);
    return 0;
  }

  cid = fork();
  if (!cid) {
    if (cmd->infile) {
      fp = freopen(cmd->infile, "r", stdin);
      if (!fp) {
        perror("oksh: can't open input file");
        return -1;
      }
    }
    if (cmd->outfile) freopen(cmd->outfile, "w", stdout);
    if (cmd->errfile) freopen(cmd->outfile, "w", stderr);

    if (infd != STDIN_FILENO) dup2(infd, STDIN_FILENO);
    if (outfd != STDOUT_FILENO) dup2(outfd, STDOUT_FILENO);

    if (!strncmp(cmd->argv[0], "history", 7)) {
      stat = BuiltinHistory();
      exit(stat);
    }

    execvp(cmd->argv[0], cmd->argv);
    perror("oksh: command not found");
    exit(EXIT_FAILURE);
  }

  return cid;
}
