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

int friendsOneUtf8ToFChar(friendsChar fchar[FRIENDS_MAX_CHAR],
                          const char **buf, friendsError *err)
{
  const char *cur;
  const char *mrk;
  int i;

#ifndef FRIENDS_USE_UTF8_INTERNAL
  friendsChar t;
#endif

  friendsAssert(fchar);
  friendsAssert(buf);
  friendsAssert(*buf);

  fchar[0] = 0;
  cur = *buf;

  /*!re2c
    re2c:define:YYCURSOR = "cur";
    re2c:indent:top = 3;
    re2c:indent:string = "  ";

    * { friendsSetError(err, ILSEQ); goto error; }
    "\x00" { goto end; }
    [\U00010000-\U0010FFFF] { goto ok4; }
    [\u0800-\uffff]         { goto ok3; }
    [\u0080-\u07ff]         { goto ok2; }
    [\u0001-\u007f]         { goto ok1; }
  */

  friendsUnreachable();

 ok1:
#ifndef FRIENDS_USE_UTF8_INTERNAL
  fchar[0] = (unsigned char)**buf;
  fchar[1] = 0;
  i = 1;
#endif
  goto ok;

 ok2:
#ifndef FRIENDS_USE_UTF8_INTERNAL
  mrk = *buf;
  t = ((unsigned char)*mrk++) & 0x1f;
  t = (t << 6) | (((unsigned char)*mrk) & 0x3f);
  fchar[0] = t;
  fchar[1] = 0;
  i = 1;
#endif
  goto ok;

 ok3:
#ifndef FRIENDS_USE_UTF8_INTERNAL
  mrk = *buf;
  t = ((unsigned char)*mrk++) & 0x0f;
  t = (t << 6) | (((unsigned char)*mrk++) & 0x3f);
  t = (t << 6) | (((unsigned char)*mrk  ) & 0x3f);
  fchar[0] = t;
  fchar[1] = 0;
  i = 1;
#endif
  goto ok;

 ok4:
#ifndef FRIENDS_USE_UTF8_INTERNAL
  mrk = *buf;
  t = ((unsigned char)*mrk++) & 0x07;
  t = (t << 6) | (((unsigned char)*mrk++) & 0x3f);
  t = (t << 6) | (((unsigned char)*mrk++) & 0x3f);
  fchar[0] = 0xd800 | (((t >> 2) - 0x100) >> 2);
  t = t & 0x0f;
  t = (t << 6) | (((unsigned char)*mrk  ) & 0x3f);
  fchar[1] = 0xdc00 | t;
  i = 2;
#endif
  goto ok;

 ok:
#ifdef FRIENDS_USE_UTF8_INTERNAL /* Copy UTF-8. */
  mrk = *buf;
  for (i = 0; i < FRIENDS_MAX_CHAR; ++i) {
    fchar[i] = *mrk;
    mrk++;
    if (mrk > cur) {
      fchar[i] = 0;
      break;
    }
  }
#endif
  *buf = cur;
  return i;

 end:
  return 0;

 error:
  return -1;
}

int friendsUtf8ToFChar(friendsChar **ret, const char *buf, friendsError *err)
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
      i = friendsOneUtf8ToFChar(fchar, &cur, err);
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
