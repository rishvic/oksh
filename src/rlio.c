#include "rlio.h"

#include <stdio.h>
#include <string.h>

#include <readline/readline.h>

ssize_t RlGetline(char** lineptr) {
  char* line;
  line = readline(">>> ");
  *lineptr = line;

  return line ? (ssize_t)strlen(line) : (ssize_t)-1;
}

void RlFree(void* ptr) { rl_free(ptr); }
