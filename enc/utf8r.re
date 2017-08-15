/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdlib.h>

#include "enc/utf8.h"
#include "friends_error.h"

/*!re2c
  re2c:define:YYCTYPE = "unsigned char";
  re2c:define:YYMARKER = "mrk";
  re2c:yyfill:enable = 0;
  re2c:flags:8 = 1;
*/

#define friendsTestOverFlow(var)                \
  do {                                          \
    if (var < 0) {                              \
      friendsSetError(err, 2BIG);               \
      goto error;                               \
    }                                           \
  } while(0)

int friendsUtf8ToFChar(friendsChar **b, const char *buf, friendsError *err)
{
  int step;
  int n, limit;
  const char *cur;
  friendsChar *tmp;
  friendsChar *p;

#ifdef FRIENDS_USE_UTF8_INTERNAL
#else
  friendsChar t;
  const char *tok;
  const char *mrk;
#endif

  friendsAssert(b);
  friendsAssert(buf);

#ifdef FRIENDS_USE_UTF8_INTERNAL
  limit = 80;
#else
  limit = 40;
#endif

  tmp = (friendsChar*)malloc(sizeof(friendsChar)*limit);
  if (!tmp) { friendsSetError(err, NOMEM); return 2; }

  n = 0;
  for(step = 0; step < 2; ++step) {
    if (step) {
      free(tmp);
      tmp = (friendsChar*)malloc(sizeof(friendsChar)*n);
      if (!tmp) { friendsSetError(err, NOMEM); return 2; }
      limit = n;
      n = 0;
    }

    p = tmp;
#ifdef FRIENDS_USE_UTF8_INTERNAL
    cur = buf;
    while(*b) {
      if (n < limit) {
        *p++ = *cur;
      }
      cur++;
      n++;
      friendsTestOverFlow(n);
    }
    if (n < limit) {
      *p = '\0';
    }
    n++;

#else
    cur = buf;
    for(;;) {
      tok = cur;
      /*!re2c
        re2c:define:YYCURSOR = "cur";
        re2c:indent:top = 3;
        re2c:indent:string = "  ";

        * { friendsSetError(err, ILSEQ); goto error; }
        "\x00" {
          if (n < limit) {
            *p = 0;
          }
          n++;
          friendsTestOverFlow(n);
          break;
        }
        [\U00010000-\U0010ffff] {
          if (n + 1 < limit) {
            t = ((unsigned char)*tok++) & 0x07;
            t = (t << 6) | (((unsigned char)*tok++) & 0x3f);
            t = (t << 6) | (((unsigned char)*tok++) & 0x3f);
            *p++ = 0xd800 | (((t >> 2) - 0x100) >> 2);
            t = t & 0x0f;
            t = (t << 6) | (((unsigned char)*tok  ) & 0x3f);
            *p++ = 0xdc00 | t;
          }
          n += 2;
          friendsTestOverFlow(n);
          continue;
        }
        [\u0800-\uffff] {
          if (n < limit) {
            t = ((unsigned char)*tok++) & 0x0f;
            t = (t << 6) | (((unsigned char)*tok++) & 0x3f);
            t = (t << 6) | (((unsigned char)*tok  ) & 0x3f);
            *p++ = t;
          }
          n++;
          friendsTestOverFlow(n);
          continue;
        }
        [\u0080-\u07ff] {
          if (n < limit) {
            t = ((unsigned char)*tok++) & 0x3f;
            t = (t << 6) | (((unsigned char)*tok) & 0x1f);
            *p++ = t;
          }
          n++;
          friendsTestOverFlow(n);
          continue;
        }
        [\u0001-\u007f] {
          if (n < limit) {
            *p++ = (unsigned char)*tok;
          }
          n++;
          friendsTestOverFlow(n);
          continue;
        }
      */

      friendsUnreachable();
    }
#endif

    if (n <= limit) {
      break;
    }
  }

  *b = tmp;
  return n - 1;

 error:
  free(tmp);
  return -1;
}
