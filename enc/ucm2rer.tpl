/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdlib.h>
#include <stdio.h>

#include "@HEADER_NAME@"
#include "friends_defs.h"
#include "friends_error.h"

/*!re2c
  re2c:define:YYCTYPE = "unsigned char";
  re2c:define:YYMARKER = "mark";
  re2c:yyfill:enable = 0;
  re2c:flags:8 = 0;
  re2c:flags:x = 0;
*/

#define friendsTestOverFlow(var)  \
  do {                            \
    if (var < 0) {                \
      friendsSetError(err, 2BIG); \
      goto error;                 \
    }                             \
  } while(0)

int friendsOne@NAME@ToFChar(friendsChar fchar[FRIENDS_MAX_CHAR],
                            const char **buf, friendsError *err)
{
  const char *mark;
  const char *cur;
  int i;

  friendsAssert(buf);
  friendsAssert(*buf);
  friendsAssert(fchar);

  /* mark は使うかもしれないし、使わないかもしれないのです。 */
  friendsUnUsed(mark);

  cur = *buf;
  fchar[0] = 0;

#ifdef FRIENDS_USE_UTF8_INTERNAL
  /*!mktble
    mktble:dir = -1;
    mktble:utfbits = 8;
    mktble:from = "cur";
    mktble:to = "fchar";
    mktble:indent:top = 1;
    mktble:indent:string = "  ";

    OK  { goto ok; }
    NG  { friendsSetError(err, ILSEQ); goto ng; }
    END { goto ok; }
  */

#else
  /*!mktble
    mktble:dir = -1;
    mktble:utfbits = 16;

    OK  { goto ok; }
    NG  { friendsSetError(err, ILSEQ); goto ng; }
    END { goto ok; }
  */

#endif

  friendsUnreachable();

 ok:
  *buf = cur;
  for (i = 0; i < FRIENDS_MAX_CHAR; ++i) {
    if (fchar[i] == 0) break;
  }
  return i;

 ng:
  return -1;
}

int friends@NAME@ToFChar(friendsChar **ret, const char *buf, friendsError *err)
{
  int n, limit;
  int step, i;

  const char *cur;
  friendsChar *b;
  friendsChar *t;
  friendsChar fchar[FRIENDS_MAX_CHAR];

  friendsAssert(ret);
  friendsAssert(buf);

  limit = 80 / sizeof(friendsChar);

  b = (friendsChar *)malloc(sizeof(friendsChar) * limit);
  if (!b) { friendsSetError(err, NOMEM); return -1; }
  t = b;

  n = 0;
  for(step = 0; step < 2; ++step) {
    if (step) {
      free(b);
      b = (friendsChar *)malloc(sizeof(friendsChar) * n);
      if (!b) { friendsSetError(err, NOMEM); return -1; }
      t = b;
      limit = n;
      n = 0;
    }

    cur = buf;
    for(;;) {
      i = friendsOne@NAME@ToFChar(fchar, &cur, err);
      if (i < 0) { goto error; }
      if (i == 0) {
        if (limit - n > 0) {
          *t++ = 0;
        }
        n++;
        friendsTestOverFlow(n);
        break;
      }
      for (i = 0; i < FRIENDS_MAX_CHAR; ++i) {
        if (fchar[i] == 0) break;
        n++;
        friendsTestOverFlow(n);
      }
      if (limit - n > 0) {
        for (i = 0; i < FRIENDS_MAX_CHAR; ++i) {
          if (fchar[i] == 0) break;
          *t++ = fchar[i];
        }
      }
    }
    if (n <= limit) {
      break;
    }
  }

  *ret = b;
  return n - 1;

 error:
  free(b);
  return -1;
}
