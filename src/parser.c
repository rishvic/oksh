#include "parser.h"

#include <stdlib.h>

#define OK_P_STK_SZ 32L
#define OK_PARSE_WORD_SZ 60L

static Command *NewCmd() {
  Command *cmd;

  cmd = (Command *)malloc(sizeof(Command));
  cmd->argc = 0;
  cmd->argv = (char **)malloc(sizeof(char *));
  cmd->argv[0] = NULL;
  cmd->infile = cmd->outfile = cmd->errfile = NULL;

  return cmd;
}

static Command *AddCharToCmd(Command *cmd, size_t *n, size_t *sz, char c,
                             int *in, int *out, int *err) {
  if (*in < 0 && *out < 0 && *err < 0) {
    if (!cmd) cmd = NewCmd();
    if (!cmd->argv[cmd->argc]) {
      *n = OK_PARSE_WORD_SZ;
      *sz = 0;
      cmd->argv[cmd->argc] = (char *)malloc(*n * sizeof(char));
      cmd->argv[cmd->argc][0] = '\0';
    }
    if (*n == *sz + 1) {
      *n <<= 1;
      cmd->argv[cmd->argc] =
          (char *)realloc(cmd->argv[cmd->argc], *n * sizeof(char));
    }

    cmd->argv[cmd->argc][(*sz)++] = c;
    cmd->argv[cmd->argc][*sz] = '\0';
  }

  if (!cmd) return NULL;

  if (*in >= 0) {
    (*in)++;
    cmd->infile = (char *)realloc(cmd->infile, (*in + 1) * sizeof(char));
    cmd->infile[*in - 1] = c;
    cmd->infile[*in] = '\0';
  }
  if (*out >= 0) {
    (*out)++;
    cmd->outfile = (char *)realloc(cmd->outfile, (*out + 1) * sizeof(char));
    cmd->outfile[*out - 1] = c;
    cmd->outfile[*out] = '\0';
  }
  if (*err >= 0) {
    (*err)++;
    cmd->errfile = (char *)realloc(cmd->errfile, (*err + 1) * sizeof(char));
    cmd->errfile[*err - 1] = c;
    cmd->errfile[*err] = '\0';
  }

  return cmd;
}

static void CloseWordInCmd(Command *cmd, size_t *n, size_t *sz) {
  if (!cmd || !cmd->argv[cmd->argc]) return;

  cmd->argc++;
  cmd->argv = (char **)realloc(cmd->argv, (cmd->argc + 1) * sizeof(char *));
  cmd->argv[cmd->argc] = NULL;
  *n = 0;
  *sz = 0;
}

static void ParseBare(const char *line, size_t sz, size_t *i, CmdNext *rel,
                      Command **cmdptr, size_t *wn, size_t *wsz, ParseType *stk,
                      size_t *ssz, int *done, int *in, int *out, int *err) {
  switch (line[*i]) {
    case '\\':
      (*i)++;
      if (line[*i] == '\n') break;
      *cmdptr = AddCharToCmd(*cmdptr, wn, wsz, line[*i], in, out, err);
      break;

    case ' ':
    case '\t':
      CloseWordInCmd(*cmdptr, wn, wsz);
      if (*in >= 0 && (*cmdptr)->infile) *in = -1;
      if (*out >= 0 && (*cmdptr)->outfile) *out = -1;
      if (*err >= 0 && (*cmdptr)->errfile) *err = -1;
      break;

    case '"':
      stk[(*ssz)++] = kParseDQuote;
      stk[*ssz] = kParseDone;
      break;

    case '\'':
      stk[(*ssz)++] = kParseQuote;
      stk[*ssz] = kParseDone;
      break;

    case '`':
      stk[(*ssz)++] = kParseBQuote;
      stk[*ssz] = kParseDone;
      break;

    case '$':
      if (line[*i + 1] == '(') {
        (*i)++;
        stk[(*ssz)++] = kParseCmdSubst;
        stk[*ssz] = kParseDone;
      }
      break;

    case '<':
      CloseWordInCmd(*cmdptr, wn, wsz);
      *in = 0;
      *out = *err = -1;
      break;

    case '>':
      CloseWordInCmd(*cmdptr, wn, wsz);
      *out = 0;
      *in = *err = -1;
      if (line[*i + 1] == '&') {
        *err = 0;
        (*i)++;
      }
      break;

    case '\n':
    case ';':
      CloseWordInCmd(*cmdptr, wn, wsz);
      *rel = kDoneCmd;
      *done = 1;
      break;

    case '|':
      CloseWordInCmd(*cmdptr, wn, wsz);
      if (line[*i + 1] == '|') {
        (*i)++;
        *rel = kOrCmd;
      } else
        *rel = kPipeCmd;
      *done = 1;
      break;

    case '&':
      if (line[*i + 1] == '>') {
        *out = *err = 0;
        *in = -1;
        (*i)++;
        break;
      }
      CloseWordInCmd(*cmdptr, wn, wsz);
      if (line[*i + 1] == '&') {
        (*i)++;
        *rel = kAndCmd;
      } else
        *rel = kJobCmd;
      *done = 1;
      break;

    case '#':
      CloseWordInCmd(*cmdptr, wn, wsz);
      while (*i < sz && line[*i] != '\n') (*i)++;
      break;

    default:
      *cmdptr = AddCharToCmd(*cmdptr, wn, wsz, line[*i], in, out, err);
      break;
  }
}

static void ParseQuote(const char *line, size_t *i, Command **cmdptr,
                       size_t *wn, size_t *wsz, ParseType *stk, size_t *ssz,
                       int *in, int *out, int *err) {
  if (line[*i] == '\'') {
    stk[--(*ssz)] = kParseDone;
    return;
  }
  AddCharToCmd(*cmdptr, wn, wsz, line[*i], in, out, err);
}

static void ParseDQuote(const char *line, size_t *i, Command **cmdptr,
                        size_t *wn, size_t *wsz, ParseType *stk, size_t *ssz,
                        int *in, int *out, int *err) {
  switch (line[*i]) {
    case '"':
      stk[--(*ssz)] = kParseDone;
      break;

    case '\\':
      (*i)++;
      if (line[*i] == '\n') break;
      AddCharToCmd(*cmdptr, wn, wsz, line[*i], in, out, err);
      break;

    case '`':
      stk[(*ssz)++] = kParseBQuote;
      stk[*ssz] = kParseDone;
      break;

    case '$':
      if (line[*i + 1] == '(') {
        (*i)++;
        stk[(*ssz)++] = kParseCmdSubst;
        stk[*ssz] = kParseDone;
      }
      break;

    default:
      AddCharToCmd(*cmdptr, wn, wsz, line[*i], in, out, err);
      break;
  }
}

static void ParseBQuote(const char *line, size_t sz, size_t *i, CmdNext *rel,
                        Command **cmdptr, size_t *wn, size_t *wsz,
                        ParseType *stk, size_t *ssz, int *done, int *in,
                        int *out, int *err) {
  if (line[*i] == '`') {
    stk[--(*ssz)] = kParseDone;
    return;
  }

  ParseBare(line, sz, i, rel, cmdptr, wn, wsz, stk, ssz, done, in, out, err);
}

static void ParseCmdSubst(const char *line, size_t sz, size_t *i, CmdNext *rel,
                          Command **cmdptr, size_t *wn, size_t *wsz,
                          ParseType *stk, size_t *ssz, int *done, int *in,
                          int *out, int *err) {
  if (line[*i] == ')') {
    stk[--(*ssz)] = kParseDone;
    return;
  }

  ParseBare(line, sz, i, rel, cmdptr, wn, wsz, stk, ssz, done, in, out, err);
}

Command *ParseLine(const char *line, size_t sz, size_t *i, CmdNext *rel) {
  int done;
  int in, out, err;
  size_t wn, wsz;
  size_t ssz = 0;
  Command *cmd = NULL;
  ParseType stk[OK_P_STK_SZ];
  stk[0] = kParseDone;

  wn = 0;
  wsz = 0;
  done = 0;
  in = out = err = -1;
  for (; !done && *i < sz; (*i)++) {
    if (!ssz) {
      ParseBare(line, sz, i, rel, &cmd, &wn, &wsz, stk, &ssz, &done, &in, &out,
                &err);
      continue;
    }

    switch (stk[ssz - 1]) {
      case kParseQuote:
        ParseQuote(line, i, &cmd, &wn, &wsz, stk, &ssz, &in, &out, &err);
        break;

      case kParseDQuote:
        ParseDQuote(line, i, &cmd, &wn, &wsz, stk, &ssz, &in, &out, &err);
        break;

      case kParseBQuote:
        ParseBQuote(line, sz, i, rel, &cmd, &wn, &wsz, stk, &ssz, &done, &in,
                    &out, &err);
        break;

      case kParseCmdSubst:
        ParseCmdSubst(line, sz, i, rel, &cmd, &wn, &wsz, stk, &ssz, &done, &in,
                      &out, &err);
        break;

      case kParseDone:
        break;
    }
  }

  return cmd;
}

void FreeCmd(Command *cmd) {
  int i;

  if (!cmd) return;
  for (i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
  free(cmd->argv);
  free(cmd->infile);
  free(cmd->outfile);
  free(cmd->errfile);
  free(cmd);
}
