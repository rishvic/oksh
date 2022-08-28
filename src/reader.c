#include "reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t kLineSz = 120L;
#define R_STK_SZ 32L

int IsFinalReadType(ReadType rt) {
  return rt != kQuote && rt != kDQuote && rt != kBQuote && rt != kCmdSubst;
}

static char *AppendStr(char *dest, size_t *n, size_t *sz, const char *src,
                       size_t src_sz) {
  while (*n < *sz + src_sz + 1) *n <<= 1;
  dest = (char *)realloc(dest, *n * sizeof(char));
  memcpy(dest + *sz, src, src_sz * sizeof(char));
  *sz += src_sz;
  dest[*sz] = '\0';
  return dest;
}

static ssize_t AppendCmd(char **lineptr, size_t *n, size_t *sz,
                         ssize_t (*getl)(char **lineptr, void *info),
                         void (*freel)(void *ptr), ReadType stk[]) {
  char *seg = NULL;
  ssize_t seg_sz;

  seg_sz = getl(&seg, stk);
  if (seg_sz == -1) {
    freel(seg);
    return -1;
  }
  *lineptr = AppendStr(*lineptr, n, sz, seg, seg_sz);
  freel(seg);
  *lineptr = AppendStr(*lineptr, n, sz, "\n", 1);

  return *sz;
}

static void ProcBare(char **lineptr, size_t *n, size_t *sz,
                     ssize_t (*getl)(char **lineptr, void *info),
                     void (*freel)(void *ptr), size_t *i, ReadType stk[],
                     size_t *ssz, int *nm) {
  ssize_t res;

  switch ((*lineptr)[*i]) {
    case '\\':
      (*i)++;
      if (nm) *nm = *nm && (*lineptr)[*i] == '\n';
      if (*i + 1 == *sz && (*lineptr)[*i] == '\n') {
        res = AppendCmd(lineptr, n, sz, getl, freel, stk);
        if (res == -1) break;
      }
      break;

    case '"':
      stk[(*ssz)++] = kDQuote;
      stk[*ssz] = kDone;
      break;
    case '\'':
      stk[(*ssz)++] = kQuote;
      stk[*ssz] = kDone;
      break;
    case '`':
      stk[(*ssz)++] = kBQuote;
      stk[*ssz] = kDone;
      break;

    case '$':
      if ((*lineptr)[*i + 1] == '(') {
        (*i)++;
        stk[(*ssz)++] = kCmdSubst;
        stk[*ssz] = kDone;
      }
      break;

    case '|':
      stk[*ssz] = kPipe;
      if (nm) *nm = 1;
      if ((*lineptr)[*i + 1] == '|') {
        stk[*ssz] = kCmdOr;
        (*i)++;
      }
      break;

    case '&':
      if ((*lineptr)[*i + 1] == '&') {
        stk[*ssz] = kCmdAnd;
        (*i)++;
        if (nm) *nm = 1;
      }
      break;

    case '#':
      *i = *sz - 1;
      break;

    case ' ':
    case '\t':
    case '\n':
      break;

    default:
      if (nm) *nm = 0;
      break;
  }
}

static void ProcQuote(char **lineptr, size_t *i, ReadType stk[], size_t *ssz) {
  if ((*lineptr)[*i] == '\'') {
    stk[--(*ssz)] = kDone;
  }
}

static void ProcDQuote(char **lineptr, size_t *i, ReadType stk[], size_t *ssz) {
  switch ((*lineptr)[*i]) {
    case '"':
      stk[--(*ssz)] = kDone;
      break;

    case '\\':
      (*i)++;
      break;

    case '`':
      stk[(*ssz)++] = kBQuote;
      stk[*ssz] = kDone;
      break;

    case '$':
      if ((*lineptr)[*i + 1] == '(') {
        (*i)++;
        stk[(*ssz)++] = kCmdSubst;
        stk[*ssz] = kDone;
      }
      break;
  }
}

static void ProcBQuote(char **lineptr, size_t *n, size_t *sz,
                       ssize_t (*getl)(char **lineptr, void *info),
                       void (*freel)(void *ptr), size_t *i, ReadType stk[],
                       size_t *ssz) {
  if ((*lineptr)[*i] == '`') {
    stk[--(*ssz)] = kDone;
    return;
  }

  ProcBare(lineptr, n, sz, getl, freel, i, stk, ssz, NULL);
}

static void ProcCmdSubst(char **lineptr, size_t *n, size_t *sz,
                         ssize_t (*getl)(char **lineptr, void *info),
                         void (*freel)(void *ptr), size_t *i, ReadType stk[],
                         size_t *ssz) {
  if ((*lineptr)[*i] == ')') {
    stk[--(*ssz)] = kDone;
    return;
  }

  ProcBare(lineptr, n, sz, getl, freel, i, stk, ssz, NULL);
}

ssize_t ReadCmd(char **lineptr, size_t *n,
                ssize_t (*getl)(char **lineptr, void *info),
                void (*freel)(void *ptr)) {
  int nm;
  size_t sz = 0, i, ssz;
  ssize_t res;
  ReadType stk[R_STK_SZ];
  stk[0] = kDone;

  if (!lineptr) {
    return -1;
  }
  if (!*lineptr || !*n) {
    *n = kLineSz;
    *lineptr = (char *)malloc(*n * sizeof(char));
  }

  res = AppendCmd(lineptr, n, &sz, getl, freel, stk);
  if (res == -1) {
    return -1;
  }

  ssz = 0;
  nm = 0;
  for (i = 0; i < sz; i++) {
    if (!ssz) {
      ProcBare(lineptr, n, &sz, getl, freel, &i, stk, &ssz, &nm);
      goto fincheck;
    }

    nm = 0;
    switch (stk[ssz - 1]) {
      case kQuote:
        ProcQuote(lineptr, &i, stk, &ssz);
        break;

      case kDQuote:
        ProcDQuote(lineptr, &i, stk, &ssz);
        break;

      case kBQuote:
        ProcBQuote(lineptr, n, &sz, getl, freel, &i, stk, &ssz);
        break;

      case kCmdSubst:
        ProcCmdSubst(lineptr, n, &sz, getl, freel, &i, stk, &ssz);
        break;

      default:
        break;
    }

  fincheck:
    if (i + 1 == sz && (ssz || nm)) {
      res = AppendCmd(lineptr, n, &sz, getl, freel, stk);
      if (res == -1) break;
    }
  }

  return sz;
}
