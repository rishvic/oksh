#include "rlio.h"
#include "reader.h"

#include <stdio.h>
#include <string.h>

#include <readline/readline.h>

ssize_t RlGetline(char** lineptr, void* info) {
  int i;
  char* line;
  char *word, prompt[512] = "";
  ReadType* stk = (ReadType*)info;

  if (stk[0] == kDone) {
    line = readline(">>> ");
  } else {
    for (i = 0; stk[i] != kDone; i++) {
      switch (stk[i]) {
        case kQuote:
          word = i ? " quote" : "quote";
          break;
        case kDQuote:
          word = i ? " dquote" : "dquote";
          break;
        case kBQuote:
          word = i ? " bquote" : "bquote";
          break;
        case kCmdSubst:
          word = i ? " cmdsubst" : "cmdsubst";
          break;
        case kDone:
          break;
      }
      strcat(prompt, word);
    }
    strcat(prompt, "> ");
    line = readline(prompt);
  }

  *lineptr = line;

  return line ? strlen(line) : -1;
}

void RlFree(void* ptr) { rl_free(ptr); }
