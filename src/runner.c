#include "runner.h"

#include <stdio.h>

int RunShell(ssize_t (*getl)(char **linept), void (*freel)(void *ptr)) {
  ssize_t sz;
  char *line = NULL;
  while ((sz = getl(&line)) != -1) {
#ifdef DEBUG
    fprintf(stderr, "Running line '%s'\n", line);
#endif
    freel(line);
  }
  freel(line);

  return 0;
}
