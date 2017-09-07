/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdlib.h>

#include "@HEADER_NAME@"
#include "friends_error.h"

/*!re2c
  re2c:define:YYCTYPE = "friendsChar";
  re2c:define:YYMARKER = "mark";
  re2c:yyfill:enable = 0;
*/

#define friendsTestOverFlow(var)                \
  do {                                          \
    if (var < 0) {                              \
      friendsSetError(err, 2BIG);               \
      goto error;                               \
    }                                           \
  } while(0)

int friendsFCharTo@NAME@(char **buf, const friendsChar *b, friendsError *err)
{
  int step;
  int n, limit;
  char *t, *p;
  const friendsChar *cur;
  const friendsChar *mark;
  const char *mbcs;

  /* mark は使うかもしれないし、使わないかもしれないのです。 */
  friendsUnUsed(mark);

  friendsAssert(b);
  friendsAssert(buf);

  limit = 80;
  t = (char*)malloc(sizeof(char) * limit);
  if (!t) { friendsSetError(err, NOMEM); return 2; }
  p = t;

  n = 0;
  for (step = 0; step < 2; ++step) {
    if (step) {
      free(t);
      t = (char*)malloc(sizeof(char) * n);
      if (!t) { friendsSetError(err, NOMEM); return 2; }
      p = t;
      limit = n;
      n = 0;
    }

    cur = b;
    for (;;) {
      /*!mktble
        mktble:dir = 1;
        mktble:from = "cur";
        mktble:to = "mbcs";
        mktble:indent:top = 3;
        mktble:indent:string = "  ";

        OK  {
          while (*mbcs) {
            if(n < limit) { *p++ = *mbcs; }
            n++;
            mbcs++;
            friendsTestOverFlow(n);
          }
          continue;
        }
        NG  { friendsSetError(err, ILSEQ); goto error; }
        END {
          if (n < limit) {
            *p++ = '\0';
          }
          n++;
          friendsTestOverFlow(n);
          break;
        }
      */

      friendsUnreachable();
    }
    if (n <= limit) {
      break;
    }
  }

  *buf = t;
  return n - 1;

 error:
  free(t);
  return -1;
}

int friends@NAME@MaxChar(void) { return @MAX_CHAR@; }
const char *friends@NAME@ICUName(void) { return "@INAME@"; }
const char *friends@NAME@IANAName(void) { return "@IANA_NAME@"; }

static friendsCodeSet friends@NAME@CodeSetData = {
  friendsOne@NAME@ToFChar, friends@NAME@ToFChar, friendsFCharTo@NAME@,
  friends@NAME@MaxChar, "@IANA_NAME@"
};

const friendsCodeSet *friends@NAME@Set(void)
{
  return &friends@NAME@CodeSetData;
}
