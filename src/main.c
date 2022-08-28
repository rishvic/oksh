#define _POSIX_C_SOURCE 200809L
#include "main.h"
#include "baseio.h"
#include "execute.h"
#include "parser.h"
#include "rlio.h"
#include "runner.h"
#include "userinfo.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <readline/history.h>

int isinteractive = 0;

const char *usage =
    "Usage: oksh -c command ...\n"
    "       oksh script-file ...\n"
    "       oksh ...\n";

int main(int argc, char *argv[]) {
  int c, errflg = 0, help = 0;
  size_t commsz;
  char *comm = NULL, histfile[1024L];
  char *fcomm;
  int stat;
  FILE *new_stdin;

  while ((c = getopt(argc, argv, "c:h")) != -1) {
    switch (c) {
      case 'c':
        comm = optarg;
        break;
      case 'h':
        help = 1;
        break;
      case '?':
        errflg = 1;
    }
  }
  if (errflg || help) {
#ifdef DEBUG
    fprintf(stderr, "%s", usage);
#endif
    stat = errflg ? EXIT_FAILURE : EXIT_SUCCESS;
    goto quit;
  }

  UpdateUserInfo();

  if (comm) {
#ifdef DEBUG
    fprintf(stderr, "Executing %s\n", comm);
#endif
    prev_state = EXIT_SUCCESS;
    running = 1;
    RunLine(comm, strlen(comm));
    running = 0;
    stat = prev_state;
    goto quit;
  }

  if (argv[optind]) {
    new_stdin = freopen(argv[optind], "r", stdin);
    if (!new_stdin) {
      perror("oksh: can't open file");
      stat = EXIT_FAILURE;
      goto quit;
    }
#ifdef DEBUG
    fprintf(stderr, "Running script %s\n", argv[optind]);
#endif
    stat = RunShell(BaseIOGetline, BaseIOFree);
    goto quit;
  }

  isinteractive = isatty(STDIN_FILENO);

  if (isinteractive) {
    signal(SIGINT, ClearLine);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    GetHistFile(histfile);
    using_history();
    read_history(histfile);
    stat = RunShell(RlGetline, RlFree);
  } else {
    stat = RunShell(BaseIOGetline, BaseIOFree);
  }

quit:
  return stat;
}
