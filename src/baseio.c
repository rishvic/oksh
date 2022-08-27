#include "baseio.h"

#include <stdio.h>
#include <stdlib.h>

ssize_t BaseIOGetline(char** lineptr, void* info) {
  char* line = NULL;
  size_t n = 0;
  ssize_t res;

  res = getdelim(&line, &n, '\n', stdin);
  *lineptr = line;
  if (res != (ssize_t)-1 && line[res - 1] == '\n') {
    line[res - 1] = '\0';
    res--;
  }

  return res;
}

void BaseIOFree(void* ptr) { free(ptr); }
