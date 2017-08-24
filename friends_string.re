/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "friends_string.h"
#include "friends_error.h"

/*!re2c
  re2c:define:YYCTYPE = "friendsChar";
  re2c:indent:string = "  ";
  re2c:yyfill:enable = 0;
 */

size_t friendsCopyString(friendsChar *output, const friendsChar *input)
{
  size_t c;

  friendsAssert(input);
  friendsAssert(output);

  c = 0;
  while((*output++ = *input++)) c++;
  return c;
}

size_t friendsStringArrayLength(const friendsChar *str)
{
  size_t c;
  c = 0;

  friendsAssert(str);

  while (*str++) c++;
  return c;
}

size_t friendsStringDuplicate(friendsChar **output,
                              const friendsChar *fstp,
                              const friendsChar *endp,
                              friendsError *err)
{
  friendsChar *s;
  size_t n;
  size_t i;

  friendsAssert(output);
  friendsAssert(fstp);

  if (!endp) {
    endp = fstp + friendsStringArrayLength(fstp);
  }

  friendsAssert(fstp < endp);

  n = endp - fstp + 1;
  s = (friendsChar *)calloc(sizeof(friendsChar), n);
  if (!s) {
    friendsSetError(err, NOMEM);
    return -1;
  }
  *output = s;
  n--;

  for (i = 0; i < n; ++i) {
    *s++ = *fstp++;
  }
  *s = 0;
  return n;
}

int friendsStringCompare(const friendsChar *a, const friendsChar *b)
{
  friendsAssert(a);
  friendsAssert(b);

  while (*a && *b) {
    if (*a != *b) return *b - *a;
    a++;
    b++;
  }
  if (*a == *b) {
    return 0;
  } else {
    return *b - *a;
  }
}

size_t friendsStringCharCount(const friendsChar *start,
                              const friendsChar *end,
                              friendsError *err)
{
  size_t c;
  const friendsChar *YYCURSOR;
  const friendsChar *YYLIMIT;
  const friendsChar *YYMARKER;

  /* UTF-8 モードでは MARKER を使うのです。 */
  friendsUnUsed(YYMARKER);

  friendsAssert(start);

  if (start == end) return 0;
  if (end && end <= start) return 0;

  c = 0;
  YYCURSOR = start;
  YYLIMIT = end ? end : (start + friendsStringArrayLength(start));

  for(; YYCURSOR < YYLIMIT; ) {
    /*!re2c
      re2c:indent:top = 2;

      * { friendsSetError(err, ILSEQ); break; }
      "\x00" { break; }
      [\u0001-\U0010ffff] { c++; continue; }
    */

    friendsUnreachable();
  }

  return c;
}

/* Interaction with normal char */
/*!re2c
  re2c:define:YYCTYPE = "unsigned char";
  re2c:flags:x = 0;
  re2c:flags:8 = 0;
 */

static int toHexDigit(const char *input)
{
  const char *YYCURSOR = input;
  /*!re2c
    re2c:indent:top = 1;

    *   { return  -1; }
    '0' { return 0x0; }
    '1' { return 0x1; }
    '2' { return 0x2; }
    '3' { return 0x3; }
    '4' { return 0x4; }
    '5' { return 0x5; }
    '6' { return 0x6; }
    '7' { return 0x7; }
    '8' { return 0x8; }
    '9' { return 0x9; }
    'a' { return 0xA; }
    'b' { return 0xB; }
    'c' { return 0xC; }
    'd' { return 0xD; }
    'e' { return 0xE; }
    'f' { return 0xF; }
  */

  friendsUnreachable();
  return -1;
}

int friendsUnescapeStringLiteral(friendsChar **output, const char *input,
                                     friendsError *err)
{
  int n;
  uint_least32_t m;
  int step;
  friendsChar *tmp;
  const char *YYCURSOR;
  const char *token;
  const char *ptoken;
  friendsChar *dstCursor;
  friendsChar copy_char;

  friendsAssert(output);
  friendsAssert(input);

  tmp = NULL;
  n = 0;
  for (step = 0; step < 2; ++step) {
    int i;

    if (step) {
      n = (n / 8 + 1) * 8;
      tmp = (friendsChar *)malloc(sizeof(friendsChar) * n);
      if (!tmp) {
        friendsSetError(err, NOMEM);
        return -1;
      }
    }

    dstCursor = tmp;
    YYCURSOR = input;
    for (;;) {
      token = YYCURSOR;
      /*!re2c
        re2c:indent:top = 3;
        re2c:define:YYFILL = "{ friendsUnreachable(); }";

        *           { friendsSetError(err, ILSEQ);
                      copy_char = *token; goto copychar; }
        "\x00"      { break; }
        "\\"        { goto escaping; }
        [\x01-\x7f] { copy_char = *token; goto copychar; }
       */
      friendsUnreachable();

    copychar:
      if (step) {
        *dstCursor++ = copy_char;
      } else {
        n++;
      }
      continue;

    escaping:
      ptoken = token;
      token = YYCURSOR;
      /*!re2c
        * { friendsSetError(err, INVAL); n = -1; break; }
        "\x00" { YYCURSOR = token; token = ptoken; goto copychar; }
        "a"    { copy_char = 0x07; goto copychar; }
        "b"    { copy_char = 0x08; goto copychar; }
        "t"    { copy_char = 0x09; goto copychar; }
        "n"    { copy_char = 0x0a; goto copychar; }
        "v"    { copy_char = 0x0b; goto copychar; }
        "f"    { copy_char = 0x0c; goto copychar; }
        "r"    { copy_char = 0x0d; goto copychar; }
        "e"    { copy_char = 0x1b; goto copychar; }
        "\""   { copy_char = 0x22; goto copychar; }
        "'"    { copy_char = 0x27; goto copychar; }
        "?"    { copy_char = 0x3f; goto copychar; }
        "\\"   { copy_char = 0x5c; goto copychar; }
        "x"    { goto hexadecimal; }
        "u"    { goto four_unicode; }
        "U"    { goto eight_unicode; }
        [0-7]  { YYCURSOR = token; goto octal; }
       */
      friendsUnreachable();

    hexadecimal:
      m = 0;
      for (i = 0; i < 2; ++i) {
        token = YYCURSOR;
        /*!re2c
          *      { friendsSetError(err, INVAL); n = -1; break; }
          "\x00" { YYCURSOR = token; token = ptoken; goto copychar; }
          [0-9a-fA-F] { m <<= 4; m |= toHexDigit(token); continue; }
        */
        friendsUnreachable();
      }
      copy_char = m & 0xff;
      goto copychar;

    octal:
      m = 0;
      for (i = 0; i < 3; ++i) {
        token = YYCURSOR;
        /*!re2c
          re2c:indent:top = 4;
          *      { friendsSetError(err, INVAL); n = -1; break; }
          "\x00" { YYCURSOR = token; token = ptoken; goto copychar; }
          [0-7]  { m <<= 3; m |= toHexDigit(token); continue; }
         */
        friendsUnreachable();
      }
      copy_char = m & 0xff;
      goto copychar;

    four_unicode:
      m = 0;
      for (i = 0; i < 4; ++i) {
        token = YYCURSOR;
        /*!re2c
          *      { friendsSetError(err, INVAL); n = -1; break; }
          "\x00" { YYCURSOR = token; token = ptoken; goto copychar; }
          [0-9a-fA-F] { m <<= 4; m |= toHexDigit(token); continue; }
         */
        friendsUnreachable();
      }
      goto copyunichar;

    eight_unicode:
      m = 0;
      for (i = 0; i < 8; ++i) {
        token = YYCURSOR;
        /*!re2c
          *      { friendsSetError(err, INVAL); n = -1; break; }
          "\x00" { YYCURSOR = token; token = ptoken; goto copychar; }
          [0-9a-fA-F] { m <<= 4; m |= toHexDigit(token); continue; }
        */
        friendsUnreachable();
      }
      goto copyunichar;

    copyunichar:
#ifdef FRIENDS_USE_UTF8_INTERNAL
      if (!step) {
        if (m <= 0x7f) {
          n += 1;
        } else if (m <= 0x07ff) {
          n += 2;
        } else if (m <= 0xffff) {
          if (m >= 0xd800 && m <= 0xdfff) {
            friendsSetError(err, INVAL);
            n = -1;
            break;
          }
          n += 3;
        } else if (m <= 0x0010ffff) {
          n += 4;
        } else {
          friendsSetError(err, INVAL);
          n = -1;
          break;
        }
      } else {
        if (m <= 0x7f) {
          *dstCursor++ = m;
        } else if (m <= 0x07ff) {
          *dstCursor++ = 0xc0 | (m >> 6);
          *dstCursor++ = 0x80 | (m & 0x3f);
        } else if (m <= 0xffff) {
          *dstCursor++ = 0xe0 | (m >> 12);
          *dstCursor++ = 0x80 | ((m >>  6) & 0x3f);
          *dstCursor++ = 0x80 | (m & 0x3f);
        } else if (m <= 0x0010ffff) {
          *dstCursor++ = 0xf0 | (m >> 18);
          *dstCursor++ = 0x80 | ((m >> 12) & 0x3f);
          *dstCursor++ = 0x80 | ((m >>  6) & 0x3f);
          *dstCursor++ = 0x80 | (m & 0x3f);
        } else {
          friendsUnreachable();
        }
      }
#else
      /* UTF-16 */
      if (!step) {
        if (m <= 0xffff) {
          if (m >= 0xd800 && m <= 0xdfff) {
            friendsSetError(err, INVAL);
            n = -1;
            break;
          }
          n += 1;
        } else if (m <= 0x0010ffff) {
          n += 2;
        } else {
          friendsSetError(err, INVAL);
          n = -1;
          break;
        }
      } else {
        if (m <= 0xffff) {
          *dstCursor++ = (friendsChar)m;
        } else if (m <= 0x0010ffff) {
          m -= 0x00010000;
          *dstCursor++ = 0xd800 + ((m >> 10) & 0x03ff);
          *dstCursor++ = 0xdc00 + (m & 0x3ff);
        } else {
          friendsUnreachable();
        }
      }
#endif
      continue;
    }
    if (n < 0) break;
  }
  if (n >= 0) {
    *dstCursor++ = '\0';
    *output = tmp;
  } else {
    *output = NULL;
  }

  return n;
}


/*!re2c
  re2c:define:YYCTYPE = "unsigned char";
  re2c:flags:x = 0;
  re2c:flags:8 = 0;
  re2c:yyfill:enable = 0;
*/

const char *friendsSourceFile(const char *name)
{
  const char *YYCURSOR;
  const char *lps;

  if (!name) return NULL;

  lps = name;
  YYCURSOR = name;

  for (;;) {
    /*!re2c
      re2c:indent:top = 2;

      *      { continue; }
      "\x00" { break; }
      "/"    { lps = YYCURSOR; continue; }
      "\\"   { goto bslash; }
      ":"    { goto colon; }
    */

    friendsUnreachable();

  bslash:
#ifdef WIN32
    lps = YYCURSOR;
#endif
    continue;

  colon:
    continue;
  }

  if (*lps == '\0') {
    return NULL;
  }
  return lps;
}
