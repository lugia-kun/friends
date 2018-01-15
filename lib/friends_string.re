/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "friends_defs.h"
#include "friends_string.h"
#include "friends_error.h"
#include "friends_list.h"
#include "friends_core.h"

/*!re2c
  re2c:define:YYCTYPE = "friendsChar";
  re2c:indent:string = "  ";
  re2c:yyfill:enable = 0;
 */

/**
 * @brief 文字列をコピーするのです。
 * @param output 書き込み先をよこすのです。
 * @param sz 書き込み先の容量をよこすのです。
 * @return コピーした配列要素数を返すのです。
 *
 * NUL 文字に当たるか、sz 個コピーするのです。
 */
static friendsSize
friendsCopyStringN(friendsChar *output, friendsSize sz,
                   const friendsChar *input)
{
  friendsSize c, r;

  friendsAssert(input);
  friendsAssert(output);

  c = 0;
  while (c < sz && (*output++ = *input++)) c++;
  r = c;
  while (c < sz) {
    *output++ = 0;
    c++;
  }
  return r;
}

friendsSize friendsCopyString(friendsChar *output, const friendsChar *input)
{
  friendsSize c;

  friendsAssert(input);
  friendsAssert(output);

  c = 0;
  while ((*output++ = *input++)) c++;
  return c;
}

friendsSize friendsStringArrayLength(const friendsChar *str)
{
  friendsSize c;
  c = 0;

  friendsAssert(str);

  while (*str++) c++;
  return c;
}

friendsSize
friendsStringDuplicate(friendsChar **output,
                       const friendsChar *fstp, const friendsChar *endp,
                       friendsError *err)
{
  friendsChar *s;
  friendsSize n;
  friendsSize i;

  friendsAssert(output);
  friendsAssert(fstp);

  if (!endp) {
    endp = fstp + friendsStringArrayLength(fstp);
  }

  friendsAssert(fstp <= endp);

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

friendsSize friendsStringCharCount(const friendsChar *start,
                                   const friendsChar *end,
                                   friendsError *err)
{
  friendsSize c;
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

static int friendsCharToNumeral(int base, const friendsChar **cur)
{
  int ir;
  const friendsChar *YYCURSOR;
  const friendsChar *YYMARKER;
  const friendsChar *tok;

  friendsAssert(cur && *cur);
  friendsAssert(base >= 2 && base <= 36);

  YYCURSOR = *cur;
  for (;;) {
    /*!re2c
      re2c:indent:top = 2;

      *  { ir = -1;  break; }
      [0\uff10] { ir =  0;  break; }
      [1\uff11] { ir =  1;  break; }
      [2\uff12] { ir =  2;  break; }
      [3\uff13] { ir =  3;  break; }
      [4\uff14] { ir =  4;  break; }
      [5\uff15] { ir =  5;  break; }
      [6\uff16] { ir =  6;  break; }
      [7\uff17] { ir =  7;  break; }
      [8\uff18] { ir =  8;  break; }
      [9\uff19] { ir =  9;  break; }
      [aA\uff21\uff41] { ir = 10;  break; }
      [bB\uff22\uff42] { ir = 11;  break; }
      [cC\uff23\uff43] { ir = 12;  break; }
      [dD\uff24\uff44] { ir = 13;  break; }
      [eE\uff25\uff45] { ir = 14;  break; }
      [fF\uff26\uff46] { ir = 15;  break; }
      [gG\uff27\uff47] { ir = 16;  break; }
      [hH\uff28\uff48] { ir = 17;  break; }
      [iI\uff29\uff49] { ir = 18;  break; }
      [jJ\uff2a\uff4a] { ir = 19;  break; }
      [kK\uff2b\uff4b] { ir = 20;  break; }
      [lL\uff2c\uff4c] { ir = 21;  break; }
      [mM\uff2d\uff4d] { ir = 22;  break; }
      [nN\uff2e\uff4e] { ir = 23;  break; }
      [oO\uff2f\uff4f] { ir = 24;  break; }
      [pP\uff30\uff50] { ir = 25;  break; }
      [qQ\uff31\uff51] { ir = 26;  break; }
      [rR\uff32\uff52] { ir = 27;  break; }
      [sS\uff33\uff53] { ir = 28;  break; }
      [tT\uff34\uff54] { ir = 29;  break; }
      [uU\uff35\uff55] { ir = 30;  break; }
      [vV\uff36\uff56] { ir = 31;  break; }
      [wW\uff37\uff57] { ir = 32;  break; }
      [xX\uff38\uff58] { ir = 33;  break; }
      [yY\uff39\uff59] { ir = 34;  break; }
      [zZ\uff3a\uff5a] { ir = 35;  break; }
    */
  }
  if (ir >= base) {
    ir = -1;
  } else {
    if (cur) *cur = YYCURSOR;
  }
  return ir;
}

long int friendsStringToLong(const friendsChar *text, const friendsChar **end,
                             int base, friendsError *err)
{
  const friendsChar *YYCURSOR = text;
  const friendsChar *YYMARKER;
  long int li = 0;
  int ix;
  friendsBool neg = friendsFalse;

  friendsAssert(text);
  friendsAssert(base >= 0);

  for (;;) {
    text = YYCURSOR;
    /*!re2c
      re2c:indent:top = 2;

      *               { YYCURSOR = text; break; }
      [ \t\u3000]     { continue; }
      [-\u2212\uff0d] { neg = friendsTrue; break; }
    */
    friendsUnreachable();
  }
  text = YYCURSOR;
  if (base == 0) {
    ix = friendsCharToNumeral(34, &YYCURSOR);
    if (ix == 0) {
      text = YYCURSOR;
      ix = friendsCharToNumeral(34, &YYCURSOR);
      if (ix == 33) { /* X */
        base = 16;
      } else {
        base = 8;
        YYCURSOR = text;
      }
    } else {
      base = 10;
      YYCURSOR = text;
    }
  }
  for (;;) {
    text = YYCURSOR;
    ix = friendsCharToNumeral(base, &YYCURSOR);
    if (ix < 0) break;
    if (li >= 0) {
      li = li * base + ix;
    }
    if (li < 0) {
      friendsSetError(err, RANGE);
      li = -1;
    }
  }
  if (li >= 0) {
    if (neg == friendsTrue) {
      li = -li;
    }
    if (end) {
      *end = text;
    }
  } else {
    li = LONG_MIN;
    if (end) {
      *end = NULL;
    }
  }
  return li;
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
      n = n + 1;
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

struct friendsStringList
{
  struct friendsList list;
  friendsSize sz;
  friendsChar text[];
};
#define friendsStringListContainer(ptr) \
  friendsListContainer(ptr, struct friendsStringList, list)

friendsStringList *friendsNewStringList(friendsError *e)
{
  friendsStringList *l;
  l = (friendsStringList *)friendsMalloc(sizeof(friendsStringList), e);
  l->sz = 0;
  return l;
}

void friendsDeleteStringList(friendsStringList *head)
{
  friendsStringList *pp;
  friendsList *p, *n, *l;

  if (!head) return;

  l = &head->list;
  friendsListForeachSafe(p, n, l) {
    pp = friendsStringListContainer(p);
    friendsStringListRemove(pp);
  }
  friendsStringListRemove(pp);
}

void friendsStringListRemove(friendsStringList *item)
{
  friendsAssert(item);

  friendsListDelete(&item->list);
  friendsFree(item);
}

friendsStringList *
friendsStringListInsert(friendsStringList *list, const friendsChar *start,
                        const friendsChar *end, friendsError *e)
{
  friendsStringList *l;
  friendsSize sz;

  friendsAssert(start);
  friendsAssert(list);

  if (end) {
    friendsAssert(end >= start);
    sz = end - start;
  } else {
    sz = friendsStringArrayLength(start);
  }
  sz++;
  if (sz <= 0) {
    friendsSetError(e, OVERFLOW);
    return NULL;
  }

  l = (friendsStringList *)friendsMalloc(sizeof(friendsStringList) +
                                         sizeof(friendsChar) * sz, e);
  if (!l) {
    return NULL;
  }

  friendsCopyStringN(&l->text, sz - 1, start);
  l->sz = sz;
  l->text[sz - 1] = 0;
  return l;
}

friendsStringList *friendsStringListNext(friendsStringList *item)
{
  friendsAssert(item);
  return friendsStringListContainer(friendsListNext(&item->list));
}

friendsStringList *friendsStringListPrev(friendsStringList *item)
{
  friendsAssert(item);
  return friendsStringListContainer(friendsListPrev(&item->list));
}

const friendsChar *friendsStringListGetString(friendsStringList *item)
{
  friendsAssert(item);
  return item->text;
}
