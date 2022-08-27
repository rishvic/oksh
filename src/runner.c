#include "runner.h"
#include "reader.h"

#include <stdio.h>
#include <stdlib.h>

int RunShell(ssize_t (*getl)(char **lineptr, void *info),
             void (*freel)(void *ptr)) {
  ssize_t sz;
  char *line = NULL;
  size_t n = 0;

  while ((sz = ReadCmd(&line, &n, getl, freel)) != -1) {
#ifdef DEBUG
    fprintf(stderr, "Running '%s'\n", line);
#else
    if (sz > 1) {
      system(line);
    }
#endif
  }
  freel(line);

  return 0;
}
