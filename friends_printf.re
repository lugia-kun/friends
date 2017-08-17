/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "friends_io.h"
#include "friends_error.h"
#include "friends_string.h"

/*!re2c
  re2c:define:YYCTYPE = "friendsChar";
  re2c:indent:string = "  ";
  re2c:yyfill:enable = 0;
*/

static unsigned int toDigit(const friendsChar *c)
{
  const friendsChar *YYCURSOR;
  YYCURSOR = c;

  /*!re2c
    re2c:indent:top = 1;

    *    { friendsUnreachable(); return -1; }
    '0'  { return  0; }
    '1'  { return  1; }
    '2'  { return  2; }
    '3'  { return  3; }
    '4'  { return  4; }
    '5'  { return  5; }
    '6'  { return  6; }
    '7'  { return  7; }
    '8'  { return  8; }
    '9'  { return  9; }
   */

  friendsUnreachable();
  return -1;
}

static size_t copyChars(friendsChar **dst, const friendsChar *from,
                     friendsChar *dstLimit, size_t copy_count)
{
  size_t r;

  friendsAssert(dst);
  friendsAssert(from);
  friendsAssert(dstLimit);

  r = 0;
  if (copy_count == 0) {
    while (*from) {
      if (*dst < dstLimit) {
        *((*dst)++) = *from++;
      }
      r++;
    }
  } else {
    while (*from && copy_count > 0) {
      if (*dst < dstLimit) {
        *((*dst)++) = *from++;
      }
      r++;
      copy_count--;
    }
  }
  return r;
}

static int friendsUltoa(friendsChar **buf, unsigned long long int v,
                        int base, int flag)
{
  int c;
  int r;
  unsigned long long int l;
  friendsChar *t;
  friendsAssert(buf);
  friendsAssert(base > 0);
  friendsAssert(base <= 36);

  l = v;
  c = 0;
  if (l == 0) {
    c = 1;
  } else {
    while (l > 0) {
      c++;
      l = l / base;
    }
  }
  t = (friendsChar *)malloc(sizeof(friendsChar) * (c + 1));
  if (!t) {
    return -1;
  }
  *buf = t;
  t = t + c;
  *t-- = 0;
  if (v == 0) {
    *t-- = '0';
  } else {
    while (v > 0) {
      r = v % base;
      if (r >= 10) {
        if (flag) {
          *t-- = 'A' + r - 10;
        } else {
          *t-- = 'a' + r - 10;
        }
      } else {
        *t-- = '0' + r;
      }
      v /= base;
    }
  }
  return c;
}

static int friendsUltoJ(friendsChar **buf, unsigned long long int v)
{
  static const char *numerals[] = {
    "\\u3007", "\\u4e00", "\\u4e8c", "\\u4e09", /* 〇一二三 */
    "\\u56db", "\\u4e94", "\\u516d", "\\u4e03", /* 四五六七 */
    "\\u516b", "\\u4e5d"                        /* 八九    */
  };
  static const char *smalldigits[] = {
    "\\u5341", "\\u767e", "\\u5343"             /* 十百千 */
  };
  /* 実のところ lli が 64bit でも 京 までしか使わない */
  static const char *largedigits[] = {
    "\\u4e07", "\\u5104", "\\u5146", "\\u4eac", /* 万億兆京 */
    "\\u5793", "\\U00025771", "\\u7a63",        /* 垓𥝱穣 */
    "\\u6e9d", "\\u6f97", "\\u6b63", "\\u8f09", /* 溝澗正載 */
    "\\u6975", "\\u6052\\u6cb3\\u6c99",         /* 極 恒河沙 */
    "\\u963f\\u50e7\\u7947",                    /* 阿僧祇 */
    "\\u90a3\\u7531\\u4ed6",                    /* 那由他 */
    "\\u4e0d\\u53ef\\u601d\\u8b70",             /* 不可思議 */
    "\\u7121\\u91cf\\u5927\\u6570"              /* 無量大数 */
  };

  int p;
  int c;
  int r;
  unsigned long long int l;
  char *t;
  char *x;
  friendsAssert(buf);

  l = v;
  c = 0;
  p = 0;
  if (l == 0) {
    c = strlen(numerals[0]);
  } else {
    while (l > 0) {
      r = l % 10;
      if (p > 0) {
        if (p % 4 == 0) {
          c += strlen(largedigits[p / 4 - 1]);
        } else {
          if (r > 0) {
            c += strlen(smalldigits[p % 4 - 1]);
          }
        }
      }
      if (r == 1) {
        if (p % 4 == 0) {
          c += strlen(numerals[r]);
        }
      } else if (r > 0) {
        c += strlen(numerals[r]);
      }
      p++;
      l = l / 10;
    }
  }
  t = (char *)malloc(sizeof(char) * (c + 1));
  if (!t) {
    return -1;
  }
  if (v == 0) {
    strcpy(t, numerals[0]);
  } else {
    x = t;
    l = 1;
    p--;
    r = p;
    while (r > 0) {
      l *= 10;
      r--;
    }
    while (v > 0) {
      r = v / l;
      if (r == 1) {
        if (p % 4 == 0) {
          strcpy(x, numerals[r]);
          x += strlen(numerals[r]);
        }
      } else if (r > 0) {
        strcpy(x, numerals[r]);
        x += strlen(numerals[r]);
      }
      if (p > 0) {
        if (p % 4 == 0) {
          strcpy(x, largedigits[p / 4 - 1]);
          x += strlen(largedigits[p / 4 - 1]);
        } else {
          if (r > 0) {
            strcpy(x, smalldigits[p % 4 - 1]);
            x += strlen(smalldigits[p % 4 - 1]);
          }
        }
      }

      v = v - r * l;
      l = l / 10;
      p--;
    }
  }
  r = friendsUnescapeStringLiteral(buf, t, NULL);
  free(t);
  return r;
}

int friendsAsprintV(friendsChar **buf, friendsError *err,
                    const friendsChar *format, va_list ap)
{
  const friendsChar *YYCURSOR;
  const friendsChar *YYMARKER;
  const friendsChar *token;
  friendsChar *outp;
  friendsChar *dstc;
  friendsChar *wlim;
  friendsChar *pstr;
  friendsChar *tmp;
  char sign;
  int width;
  int precision;
  unsigned long long int uintval;
  long long int intval;
  double fltval;
  int base;
  char *ctmp;
  const friendsChar *fchrp;
  const char *chrp;
  va_list aq;
  int i;
  static const int sbuf_size = 20;
  char sbuf[20];
  friendsError e;

  enum pflags {
    ALTERNATE = 0x0001,
    HEXPREFIX = 0x0002,
    LADJUST   = 0x0004,
    LONGINT   = 0x0008,
    ZEROPAD   = 0x0010,

    QUARTER   = 0x0020,
    HALF      = 0x0040,
    LONG      = 0x0080,
    QUADRUPLE = 0x0100,
    PTRDIFF   = 0x0200,
    SIZET     = 0x0400,

    FPT       = 0x1000,
    CAPITAL   = 0x2000,
  } flags;

  static const size_t init_size = 128;
  size_t szcnt;
  int stp;

  outp = (friendsChar*)malloc(sizeof(friendsChar) * init_size);
  if (!outp) {
    friendsSetError(err, NOMEM);
    return -1;
  }

  if (!err) {
    e = friendsNoError;
    err = &e;
  }

  va_copy(aq, ap);
  wlim = outp + init_size - 1;

  szcnt = 0;
  for (stp = 0; stp < 2; ++stp) {
    YYCURSOR = format;
    if (stp) {
      free(outp);
      outp = (friendsChar*)malloc(sizeof(friendsChar) * (szcnt + 1));
      wlim = outp + szcnt + 1;
      va_copy(ap, aq);
      va_end(aq);

      if (!outp) {
        friendsSetError(err, NOMEM);
        return -1;
      }
      szcnt = 0;
    }
    dstc = outp;

    for (;;) {
      token = YYCURSOR;
      /*!re2c
        re2c:indent:top = 3;

        percent = "%";
        modifiers   = [hl];
        conversions = [diouxXeEgGaAfFcsp];

        *      { goto copy_char; }
        "\x00" { break; }
        "%%"   { goto copy_char; }
        "%"    { goto format_start; }
       */
      friendsUnreachable();

    copy_char:
      szcnt += copyChars(&dstc, token, wlim, 1);
      continue;

    format_start:
      width = 0;
      precision = -1;
      flags = 0;
      chrp = NULL;
      fchrp = NULL;
      sign = ' ';

    rtags:
      token = YYCURSOR;
      /*!re2c
        re2c:indent:top = 3;

        *           { friendsSetError(err, INVAL); goto error; }
        " "         { sign = ' ';         goto rtags; }
        "+"         { sign = '+';         goto rtags; }
        "-"         { flags |= LADJUST;   goto rtags; }
        "#"         { flags |= ALTERNATE; goto rtags; }
        "0"         { flags |= ZEROPAD;   goto rtags; }
        modifiers   { YYCURSOR = token;   goto modifier; }
        conversions { YYCURSOR = token;   goto convert;  }
        [1-9]       { YYCURSOR = token;   goto width;    }
        ".*"        { goto get_precision; }
        "."         { goto precision; }
        "*"         { goto get_width; }
       */

      friendsUnreachable();

    get_width:
      width = va_arg(ap, int);
      if (width < 0) {
        width = -width;
        flags |= LADJUST;
      }
      token = YYCURSOR;
      /*!re2c
        re2c:indent:top = 3;

        *    { friendsSetError(err, INVAL); goto error; }
        modifiers   { YYCURSOR = token; goto modifier; }
        conversions { YYCURSOR = token; goto convert;  }
        ".*"        { goto get_precision; }
        "."         { goto precision; }
       */

      friendsUnreachable();

    width:
      width = 0;
      for (;;) {
        token = YYCURSOR;
        /*!re2c
          re2c:indent:top = 4;

          *     { friendsSetError(err, INVAL); goto error; }
          [0-9] { width = width * 10 + toDigit(token); continue; }
          modifiers   { YYCURSOR = token; goto modifier; }
          conversions { YYCURSOR = token; goto convert;  }
          ".*"        { goto get_precision; }
          "."         { goto precision; }
         */

        friendsUnreachable();
      }

    get_precision:
      precision = va_arg(ap, int);
      if (precision < 0) {
        precision = -1;
      }
      token = YYCURSOR;
      /*!re2c
        re2c:indent:top = 3;

        *    { friendsSetError(err, INVAL); goto error; }
        modifiers   { YYCURSOR = token; goto modifier; }
        conversions { YYCURSOR = token; goto convert;  }
       */

      friendsUnreachable();

    precision:
      precision = 0;
      for (;;) {
        token = YYCURSOR;
        /*!re2c
          re2c:indent:top = 4;

          *     { friendsSetError(err, INVAL); goto error; }
          [0-9] { precision = precision * 10 + toDigit(token); continue; }
          modifiers   { YYCURSOR = token; goto modifier; }
          conversions { YYCURSOR = token; goto convert;  }
        */

        friendsUnreachable();
      }

    modifier:
      token = YYCURSOR;
      /*!re2c
        re2c:indent:top = 3;

        *    { friendsSetError(err, INVAL); goto error; }
        "hh" { flags |= QUARTER;   goto convert; }
        "h"  { flags |= HALF;      goto convert; }
        "ll" { flags |= QUADRUPLE; goto convert; }
        "l"  { flags |= LONG;      goto convert; }
        "t"  { flags |= SIZET;     goto convert; }
        conversions { YYCURSOR = token; goto convert; }
       */

      friendsUnreachable();

    convert:
      token = YYCURSOR;
      for(;;) {
        /*!re2c
          re2c:indent:top = 4;

          *       { break; }
          [XEFGA] { flags |= CAPITAL; break; }
        */
        friendsUnreachable();
      }

      YYCURSOR = token;
      /*!re2c
        re2c:indent:top = 3;

        * { friendsSetError(err, INVAL); goto error; }
        [di]  { goto int_conversion; }
        "o"   { goto octal_conversion; }
        "u"   { goto uint_conversion; }
        [xX]  { goto hex_conversion; }
        [eE]  { goto exponent_conversion; }
        [fF]  { goto fixpoint_conversion; }
        [aA]  { goto hexfloat_conversion; }
        [gG]  { goto shortflt_conversion; }
        "p"   { goto pointer_conversion; }
        "C"   { flags |= LONG; goto char_conversion; }
        "c"   { goto char_conversion; }
        "S"   { flags |= LONG; goto strs_conversion; }
        "s"   { goto strs_conversion; }
       */

      friendsUnreachable();

    int_conversion:
      if (flags & LONG) {
        intval = va_arg(ap, long);
      } else if (flags & QUADRUPLE) {
        intval = va_arg(ap, long long);
      } else {
        intval = va_arg(ap, int);
      }

      if (intval < 0) {
        uintval = -intval;
        sign = '-';
      } else {
        uintval =  intval;
      }

      base = 10;
      goto number;

    uint_conversion:
      base = 10;
      sign = ' ';
      goto load_unumber;

    octal_conversion:
      base =  8;
      sign = ' ';
      goto load_unumber;

    pointer_conversion:
      base = 16;
      sign = ' ';
      flags |= ZEROPAD;
      /*
      if (width == 0) {
        width = sizeof(void *) * 2 + 2;
      }
      */
      friendsUnescapeStringLiteral(&tmp, "0x", err);
      width -= 2;
      if (width < 0) width = 0;
      if (friendsAnyError(*err)) goto error;
      szcnt += copyChars(&dstc, tmp, wlim, 0);
      free(tmp);

      uintval = (unsigned long long)va_arg(ap, void *);
      goto number;

    hex_conversion:
      base = 16;
      sign = ' ';
      if (flags & ALTERNATE) {
        if (flags & CAPITAL) {
          friendsUnescapeStringLiteral(&tmp, "0X", err);
        } else {
          friendsUnescapeStringLiteral(&tmp, "0x", err);
        }
        width -= 2;
        if (friendsAnyError(*err)) goto error;
        szcnt += copyChars(&dstc, tmp, wlim, 0);
        free(tmp);
      }
      goto load_unumber;

    load_unumber:
      if (flags & LONG) {
        uintval = va_arg(ap, unsigned long);
      } else if (flags & QUADRUPLE) {
        uintval = va_arg(ap, unsigned long long);
      } else {
        uintval = va_arg(ap, unsigned int);
      }

    number:
      if (base == 10 && (flags & ALTERNATE)) {
        i = friendsUltoJ(&pstr, uintval);
        if (i < 0) {
          goto error;
        }
        if (precision < i) {
          precision = i;
        }
        fchrp = pstr;
        goto dostr_conversion;
      }
      i = friendsUltoa(&pstr, uintval, base, flags & CAPITAL);
      if (i < 0) {
        goto error;
      }
      if (precision < i) {
        precision = i;
      }
      if (sign != ' ') {
        sbuf[0] = sign;
        sbuf[1] = '\0';
        width--;
      } else {
        sbuf[0] = '\0';
      }
      if (flags & ZEROPAD) {
        friendsUnescapeStringLiteral(&tmp, sbuf, err);
        if (friendsAnyError(*err)) goto error;
        szcnt += copyChars(&dstc, tmp, wlim, 0);
        free(tmp);
      }
      if ((!(flags & LADJUST)) && width > precision) {
        if (flags & ZEROPAD) {
          friendsUnescapeStringLiteral(&tmp, "0", err);
        } else {
          friendsUnescapeStringLiteral(&tmp, " ", err);
        }
        if (friendsAnyError(*err)) goto error;
        while (width > precision) {
          szcnt += copyChars(&dstc, tmp, wlim, 0);
          --width;
        }
        free(tmp);
      }
      if ((flags & ZEROPAD) == 0) {
        friendsUnescapeStringLiteral(&tmp, sbuf, err);
        if (friendsAnyError(*err)) goto error;
        szcnt += copyChars(&dstc, tmp, wlim, 0);
        free(tmp);
      }
      if (precision > i) {
        friendsUnescapeStringLiteral(&tmp, "0", err);
        if (friendsAnyError(*err)) goto error;
        while (precision > i) {
          szcnt += copyChars(&dstc, tmp, wlim, 0);
          precision--;
          width--;
        }
        free(tmp);
      }
      szcnt += copyChars(&dstc, pstr, wlim, precision);
      if (width > precision) {
        friendsUnescapeStringLiteral(&tmp, " ", err);
        if (friendsAnyError(*err)) goto error;
        while (width > precision) {
          szcnt += copyChars(&dstc, tmp, wlim, 0);
          --width;
        }
        free(tmp);
      }
      free(pstr);
      continue;

    char_conversion:
      uintval = va_arg(ap, unsigned int);
      if (flags & LONG) goto fchar_conversion;
      if ((!(flags & LADJUST)) && width > 0) {
        if (flags & ZEROPAD) {
          friendsUnescapeStringLiteral(&tmp, "0", err);
        } else {
          friendsUnescapeStringLiteral(&tmp, " ", err);
        }
        if (friendsAnyError(*err)) goto error;
        while (--width > 0) {
          szcnt += copyChars(&dstc, tmp, wlim, 0);
        }
        free(tmp);
      }
      if (dstc < wlim) {
        *dstc++ = (friendsChar)uintval;
      }
      szcnt++;
      if (width > 0) {
        friendsUnescapeStringLiteral(&tmp, " ", err);
        if (friendsAnyError(*err)) goto error;
        while (--width > 0) {
          szcnt += copyChars(&dstc, tmp, wlim, 0);
        }
        free(tmp);
      }
      continue;

    fchar_conversion:
      if (uintval > 0x10ffff) {
        uintval = 0xfffd;
      }
      sbuf[0] = '\\';
      sbuf[1] = 'U';
      snprintf(&sbuf[2], sbuf_size - 2, "%08llx", uintval & 0xffffff);
      friendsUnescapeStringLiteral(&pstr, sbuf, err);
      if (friendsAnyError(*err)) goto error;
      fchrp = pstr;
      goto dostr_conversion;

    strs_conversion:
      if (flags & LONG) goto fstr_conversion;
      chrp = va_arg(ap, const char *);
      if (chrp) {
        friendsUnescapeStringLiteral(&pstr, chrp, err);
      } else {
        friendsUnescapeStringLiteral(&pstr, "(null)", err);
      }
      if (friendsAnyError(*err)) goto error;
      fchrp = pstr;
      goto dostr_conversion;

    fstr_conversion:
      fchrp = va_arg(ap, const friendsChar *);
      pstr = NULL;
      if (!fchrp) {
        friendsUnescapeStringLiteral(&pstr, "(null)", err);
        fchrp = pstr;
      }
      goto dostr_conversion;

    dostr_conversion:
      if (precision < 0) {
        precision = friendsStringArrayLength(fchrp);
      }
      if ((!(flags & LADJUST)) && width > precision) {
        if (flags & ZEROPAD) {
          friendsUnescapeStringLiteral(&tmp, "0", err);
        } else {
          friendsUnescapeStringLiteral(&tmp, " ", err);
        }
        if (friendsAnyError(*err)) goto error;
        while (width > precision) {
          szcnt += copyChars(&dstc, tmp, wlim, 0);
          --width;
        }
        free(tmp);
      }
      szcnt += copyChars(&dstc, fchrp, wlim, precision);
      if (width > precision) {
        friendsUnescapeStringLiteral(&tmp, " ", err);
        if (friendsAnyError(*err)) goto error;
        while (width > precision) {
          szcnt += copyChars(&dstc, tmp, wlim, 0);
          --width;
        }
        free(tmp);
      }

      if (pstr) free(pstr);
      pstr = NULL;
      continue;

    hexfloat_conversion:;

    exponent_conversion:;

    fixpoint_conversion:;

    shortflt_conversion:
      friendsAssert(!"Conversion of floats is not supported yet.");
    }

    if (dstc < wlim) {
      *dstc = '\0';
      break;
    }
  }

  *buf = outp;
  return szcnt;

 error:
  free(outp);
  return -1;
}
