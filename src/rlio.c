#include "rlio.h"
#include "reader.h"
#include "userinfo.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <readline/readline.h>

#define RESET "\x1B[0m"
#define BOLD_RED "\x1B[1;31m"
#define CYAN "\x1B[36m"

#define OK_DIR_SIZE 1024L

static void GetDir(char* dir) {
  char pwd[OK_DIR_SIZE], *home;
  size_t homelen;

  getcwd(pwd, OK_DIR_SIZE);
  pwd[OK_DIR_SIZE - 1] = '\0';
  home = GetHome();
  homelen = strlen(home);
  while (homelen && home[homelen - 1] == '/') home[homelen--] = '\0';

  if (!homelen || !strncmp(pwd, home, homelen)) {
    strncpy(dir, "~", OK_DIR_SIZE);
    strncpy(dir + 1, pwd + homelen, OK_DIR_SIZE - homelen);
  } else {
    strncpy(dir, pwd, OK_DIR_SIZE);
  }
}

void ClearLine(int sig) {
  (void)sig;
  printf("\n");
  rl_replace_line("", 0);
  rl_on_new_line();
  rl_redisplay();
}

ssize_t RlGetline(char** lineptr, void* info) {
  int i;
  char* line;
  char *word, prompt[512] = "";
  char *user, *host, dir[OK_DIR_SIZE];
  ReadType* stk = (ReadType*)info;

  if (stk[0] == kDone) {
    user = GetUser();
    host = GetHost();
    GetDir(dir);
    sprintf(prompt, RESET BOLD_RED "%s" RESET "@%s " CYAN "%s " RESET, user,
            host, dir);
    line = readline(prompt);
  } else {
    for (i = 0; !IsFinalReadType(stk[i]); i++) {
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
        default:
          break;
      }
      strcat(prompt, word);
    }

    switch (stk[i]) {
      case kPipe:
        strcat(prompt, i ? " pipe> " : "pipe> ");
        break;
      case kCmdAnd:
        strcat(prompt, i ? " cmdand> " : "cmdand> ");
        break;
      case kCmdOr:
        strcat(prompt, i ? " cmdor> " : "cmdor> ");
        break;
      default:
        strcat(prompt, "> ");
        break;
    }
    line = readline(prompt);
  }

  *lineptr = line;

  return line ? strlen(line) : -1;
}

void RlFree(void* ptr) { rl_free(ptr); }
