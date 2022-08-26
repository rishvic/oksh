#define _POSIX_C_SOURCE 200809L
#include "baseio.h"
#include "rlio.h"
#include "runner.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *usage =
    "Usage: oksh -c command ...\n"
    "       oksh script-file ...\n"
    "       oksh ...\n";

int main(int argc, char *argv[]) {
  int c, errflg = 0, help = 0;
  char *comm = NULL;
  int stat;

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
    return errflg ? EXIT_FAILURE : EXIT_SUCCESS;
  }

  if (comm) {
#ifdef DEBUG
    fprintf(stderr, "Executing %s\n", comm);
#endif
    return EXIT_SUCCESS;
  }

  if (argv[optind]) {
    freopen(argv[optind], "r", stdin);
#ifdef DEBUG
    fprintf(stderr, "Running script %s\n", argv[optind]);
#endif
    stat = RunShell(BaseIOGetline, BaseIOFree);
    return stat;
  }

  stat = isatty(STDIN_FILENO) ? RunShell(RlGetline, RlFree)
                              : RunShell(BaseIOGetline, BaseIOFree);
  return stat;
}
