/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdlib.h>
#include <stdio.h>

#include "@HEADER_NAME@"
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

int friends@NAME@ToFChar(friendsChar **ret, const char *buf, friendsError *err)
{
  int n, limit;
  int step, i;

  const char *mark;
  const char *cur;
  friendsChar *b;
  friendsChar *t;
#ifdef FRIENDS_USE_UTF8_INTERNAL
  friendsChar fchar[4];
#else
  friendsChar fchar[2];
#endif

  friendsAssert(ret);
  friendsAssert(buf);

#ifdef FRIENDS_USE_UTF8_INTERNAL
  limit = 80;
#else
  limit = 40;
#endif

  b = (friendsChar *)malloc(sizeof(friendsChar) * limit);
  if (!b) { friendsSetError(err, NOMEM); return 2; }
  t = b;

  n = 0;
  for(step = 0; step < 2; ++step) {
    if (step) {
      free(b);
      b = (friendsChar *)malloc(sizeof(friendsChar) * n);
      if (!b) { friendsSetError(err, NOMEM); return 2; }
      t = b;
      limit = n;
      n = 0;
    }

    cur = buf;
    for(;;) {
#ifdef FRIENDS_USE_UTF8_INTERNAL
      /*!mktble
        mktble:dir = -1;
        mktble:utfbits = 8;
        mktble:from = "cur";
        mktble:to = "fchar";
        mktble:indent:top = 3;
        mktble:indent:string = "  ";

        OK {
          for(i = 0; i < 4; ++i) {
            if (fchar[i] == '\0') break;
            n++;
            friendsTestOverFlow(n);
          }
          if (limit - n > 0) {
            for (i = 0; i < 4; ++i) {
              if (fchar[i] == '\0') break;
              *t++ = fchar[i];
            }
          }
          continue;
        }
        NG { friendsSetError(err, ILSEQ); goto error; }
        END {
          if (limit - n > 0) {
            *t++ = '\0';
          }
          n++;
          friendsTestOverFlow(n);
          break;
        }
      */

      friendsUnreachable();

#else
      /*!mktble
        mktble:dir = -1;
        mktble:utfbits = 16;

        OK {
          for(i = 0; i < 2; ++i) {
          if (fchar[i] == 0) break;
            n++;
            friendsTestOverFlow(n);
          }
          if (limit - n > 0) {
            for (i = 0; i < 2; ++i) {
              if (fchar[i] == 0) break;
              *t++ = fchar[i];
            }
          }
          continue;
        }
        NG { friendsSetError(err, ILSEQ); goto error; }
        END {
          if (limit - n > 0) {
            *t++ = 0;
          }
          n++;
          friendsTestOverFlow(n);
          break;
        }
      */

      friendsUnreachable();
#endif
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
