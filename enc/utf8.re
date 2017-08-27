/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdlib.h>
#include <stdio.h>

#include "enc/utf8.h"
#include "friends_error.h"

/*!re2c
  re2c:define:YYCTYPE = "friendsChar";
  re2c:yyfill:enable = 0;
  re2c:flags:8 = 0;
  re2c:flags:x = 1;
*/

#define friendsTestOverFlow(var)                \
  do {                                          \
    if (var < 0) {                              \
      friendsSetError(err, 2BIG);               \
      goto error;                               \
    }                                           \
  } while(0)

int friendsFCharToUtf8(char **buf, const friendsChar *b, friendsError *err)
{
#ifndef FRIENDS_USE_UTF8_INTERNAL
  const friendsChar *tok;
#endif
  const friendsChar *cur;
  char *t, *p;
  int step;
  int n, limit;

  friendsAssert(b);
  friendsAssert(buf);

  limit = 80;
  t = (char*)calloc(sizeof(char), limit);
  if (!t) { friendsSetError(err, NOMEM); return 2; }

  n = 0;
  for(step = 0; step < 2; ++step) {
    if (step) {
      free(t);
      t = (char*)calloc(sizeof(char), n);
      if (!t) { friendsSetError(err, NOMEM); return 2; }
      limit = n;
      n = 0;
    }

    p = t;
    cur = b;
#ifdef FRIENDS_USE_UTF8_INTERNAL
    while(*cur) {
      if (n < limit) {
        *p++ = *cur;
      }
      cur++;
      n++;
      friendsTestOverFlow(n);
    }
    if (n < limit) {
      *p++ = '\0';
    }
    n++;

#else
    for (;;) {
      tok = cur;
      /*!re2c
        re2c:define:YYCURSOR = "cur";
        re2c:indent:top = 3;
        re2c:indent:string = "  ";

        * { friendsSetError(err, ILSEQ); goto error; }
        "\x00" {
          if (n < limit) {
            *p = '\x00';
          }
          n++;
          friendsTestOverFlow(n);
          break;
        }

        [\U00010000-\U0010ffff] {
          if (n + 3 < limit) {
            friendsChar t = *tok;
            t &= 0x03ff;
            t += 0x0040;
            *p++ = (unsigned char)(0xf0 | ( t >> 8));
            *p++ = (unsigned char)(0x80 | ((t >> 2) & 0x3f));
            t = (t & 0x3) << 10;
            t = t | ((*(tok + 1)) & 0x03ff);
            *p++ = (unsigned char)(0x80 | ((t >> 6) & 0x3f));
            *p++ = (unsigned char)(0x80 | ( t       & 0x3f));
          }
          n += 4;
          friendsTestOverFlow(n);
          continue;
        }
        [\u0800-\uffff] {
          if (n + 2 < limit) {
            *p++ = (unsigned char)(0xe0 | ( (*tok) >> 12));
            *p++ = (unsigned char)(0x80 | (((*tok) >>  6) & 0x3f));
            *p++ = (unsigned char)(0x80 | ( (*tok)        & 0x3f));
          }
          n += 3;
          friendsTestOverFlow(n);
          continue;
        }
        [\u0080-\u07ff] {
          if (n + 1 < limit) {
            *p++ = (unsigned char)(0xc0 | ((*tok) >> 6));
            *p++ = (unsigned char)(0x80 | ((*tok) & 0x3f));
          }
          n += 2;
          continue;
        }
        [\u0001-\u007f] {
          if (n < limit) {
            *p++ = *tok;
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

  *buf = t;
  return n - 1;

 error:
  free(t);
  return -1;
}

int friendsUtf8MaxChar(void) { return 4; }
const char *friendsUtf8ICUName(void) { return NULL; }
const char *friendsUtf8IANAName(void) { return "UTF-8"; }

static friendsCodeSet friendsUtf8CodeSetData = {
  friendsOneUtf8ToFChar, friendsUtf8ToFChar, friendsFCharToUtf8,
  friendsUtf8MaxChar, "UTF-8"
};

const friendsCodeSet *friendsUtf8Set(void)
{
  return &friendsUtf8CodeSetData;
}
