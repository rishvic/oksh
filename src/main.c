#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

int running;

int main() {
  char *line;

  if (isatty(STDOUT_FILENO)) {
    running = 1;
    printf("Hello tty\n");
    while (running) {
      line = readline(">>> ");

      if (!line) {
        running = 0;
        break;
      }
      if (!strcmp(line, "exit")) {
        rl_free(line);
        running = 0;
        break;
      }

      rl_free(line);
    }
    printf("BYE\n");
  } else {
    printf("Not tty\n");
    printf("BYE\n");
  }
}
