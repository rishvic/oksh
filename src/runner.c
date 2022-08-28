#include "runner.h"
#include "execute.h"
#include "main.h"
#include "parser.h"
#include "reader.h"
#include "userinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <readline/history.h>

#define OK_PIPE_STK_SZ 32L
#define OK_BUF_SZ 1024L

int prev_state;
int running;

int RunShell(ssize_t (*getl)(char **lineptr, void *info),
             void (*freel)(void *ptr)) {
  int stk_sz, si;
  int pipedes[2];
  int infd, outfd;
  pid_t idstk[OK_PIPE_STK_SZ];
  pid_t pid;
  char buf[OK_BUF_SZ];
  ssize_t sz;
  char *line = NULL;
  size_t n = 0, i;
  Command *cmd;
  CmdNext rel, newrel;
#ifdef DEBUG
  int j;
#endif

  prev_state = EXIT_SUCCESS;
  running = 1;

  while (running && (sz = ReadCmd(&line, &n, getl, freel)) != -1) {
    rel = kDoneCmd;
    stk_sz = 0;
    infd = STDIN_FILENO;
    for (i = 0; running && i < (size_t)sz;) {
      cmd = ParseLine(line, sz, &i, &newrel);
      if (!cmd) continue;

      if ((rel == kAndCmd && prev_state != EXIT_SUCCESS) ||
          (rel == kOrCmd && prev_state == EXIT_SUCCESS))
        goto cmdend;

      if (isinteractive) {
        if (line[sz - 1] == '\n') line[sz - 1] = '\0';
        /* Add history to line. */
        add_history(line);

        /* Write to file. */
        GetHistFile(buf);
        write_history(buf);
        if (line[sz - 1] == '\0') line[sz - 1] = '\n';
      }

#ifdef DEBUG
      fprintf(stderr, "Command = [");
      for (j = 0; cmd->argv[j]; j++) {
        fprintf(stderr, " '%s'%s", cmd->argv[j], j == cmd->argc - 1 ? "" : ",");
      }
      fprintf(stderr, " ]");
      fprintf(stderr, ", rel = %d, newrel = %d", rel, newrel);
      if (cmd->infile) fprintf(stderr, ", infile = %s", cmd->infile);
      if (cmd->outfile) fprintf(stderr, ", outfile = %s", cmd->outfile);
      if (cmd->errfile) fprintf(stderr, ", errfile = %s", cmd->errfile);
      fprintf(stderr, "\n");
#endif
      infd = rel == kPipeCmd ? pipedes[0] : STDIN_FILENO;
      if (newrel == kPipeCmd) {
        pipe(pipedes);
        outfd = pipedes[1];
      } else
        outfd = STDOUT_FILENO;

      pid = ExecuteCmd(cmd, infd, outfd);
      if (infd != STDIN_FILENO) close(infd);
      if (outfd != STDOUT_FILENO) close(outfd);

      if (pid > 0) {
        switch (newrel) {
          case kPipeCmd:
            idstk[stk_sz++] = pid;
            break;

          case kJobCmd:
            infd = STDIN_FILENO;
            stk_sz = 0;
            break;

          default:
            idstk[stk_sz++] = pid;
            infd = STDIN_FILENO;
#ifdef DEBUG
            fprintf(stderr, "Pipe stack size = %d\n", stk_sz);
#endif
            for (si = 0; si < stk_sz; si++) waitpid(idstk[si], &prev_state, 0);
            stk_sz = 0;
            break;
        }
      }

    cmdend:
      if (cmd) FreeCmd(cmd);
      rel = newrel;
    }
  }
  free(line);

  return prev_state;
}
