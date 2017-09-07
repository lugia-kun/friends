
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "friends_io.h"
#include "friends_string.h"
#include "friends_error.h"
#include "friends_global.h"

struct friendsFileT
{
  FILE *file;
  const friendsCodeSet *encoding;
};

int friendsAsprintCV(friendsChar **buf, friendsError *err,
                     const char *format, va_list ap)
{
  friendsChar *fmt;
  int r;

  r = friendsUnescapeStringLiteral(&fmt, format, err);
  if (r < 0) {
    return -1;
  }

  r = friendsAsprintV(buf, err, fmt, ap);
  free(fmt);

  return r;
}

int friendsAsprintCF(friendsChar **buf, friendsError *err,
                     const char *format, ...)
{
  va_list ap;
  int r;
  va_start(ap, format);
  r = friendsAsprintCV(buf, err, format, ap);
  va_end(ap);
  return r;
}

int friendsAsprintF(friendsChar **buf, friendsError *err,
                    const friendsChar *format, ...)
{
  va_list ap;
  int r;
  va_start(ap, format);
  r = friendsAsprintV(buf, err, format, ap);
  va_end(ap);
  return r;
}

int friendsFprintV(friendsFile *fp, friendsError *err,
                   const friendsChar *format, va_list ap)
{
  int r;
  char *cbuf;
  friendsChar *buf;
  friendsError e;
  friendsAssert(fp);
  friendsAssert(fp->encoding);

  if (!err) {
    e = friendsNoError;
    err = &e;
  }

  r = friendsAsprintV(&buf, err, format, ap);
  if (friendsAnyError(*err)) {
    return -1;
  }
  r = fp->encoding->dec(&cbuf, buf, err);
  free(buf);
  if (friendsAnyError(*err)) {
    return -1;
  }

  r = fprintf(fp->file, "%s", cbuf);
  friendsSetErrorFromErrno(err, errno);
  free(cbuf);
  return r;
}

int friendsFprintF(friendsFile *fp, friendsError *err,
                   const friendsChar *format, ...)
{
  int r;
  va_list ap;
  va_start(ap, format);
  r = friendsFprintV(fp, err, format, ap);
  va_end(ap);
  return r;
}


int friendsFprintCV(friendsFile *fp, friendsError *err,
                    const char *format, va_list ap)
{
  int r;
  char *cbuf;
  friendsChar *buf;
  friendsError e;
  friendsAssert(fp);
  friendsAssert(fp->encoding);

  if (!err) {
    e = friendsNoError;
    err = &e;
  }

  r = friendsAsprintCV(&buf, err, format, ap);
  if (r < 0) {
    return -1;
  }
  r = fp->encoding->dec(&cbuf, buf, err);
  free(buf);
  if (r < 0) {
    return -1;
  }

  r = fprintf(fp->file, "%s", cbuf);
  friendsSetErrorFromErrno(err, errno);
  free(cbuf);
  return r;
}

int friendsFprintCF(friendsFile *fp, friendsError *err,
                    const char *format, ...)
{
  int r;
  va_list ap;
  va_start(ap, format);
  r = friendsFprintCV(fp, err, format, ap);
  va_end(ap);
  return r;
}

int friendsPrintV(FILE *fp, friendsError *err,
                  const friendsChar *format, va_list ap)
{
  int r;
  char *cbuf;
  friendsChar *buf;
  friendsError e;
  const friendsCodeSet *set;
  friendsAssert(fp);

  if (!err) {
    e = friendsNoError;
    err = &e;
  }

  r = friendsAsprintV(&buf, err, format, ap);
  if (r < 0) {
    return -1;
  }

  set = friendsGetTerminalEncoding();
  r = set->dec(&cbuf, buf, err);
  free(buf);
  if (r < 0) {
    return -1;
  }

  r = fprintf(fp, "%s", cbuf);
  friendsSetErrorFromErrno(err, errno);
  free(cbuf);
  return r;
}

int friendsPrintF(FILE *fp, friendsError *err,
                  const friendsChar *format, ...)
{
  int r;
  va_list ap;
  va_start(ap, format);
  r = friendsPrintV(fp, err, format, ap);
  va_end(ap);
  return r;
}


int friendsPrintCV(FILE *fp, friendsError *err,
                   const char *format, va_list ap)
{
  int r;
  char *cbuf;
  friendsChar *buf;
  friendsError e;
  const friendsCodeSet *set;
  friendsAssert(fp);

  if (!err) {
    e = friendsNoError;
    err = &e;
  }

  r = friendsAsprintCV(&buf, err, format, ap);
  if (r < 0) {
    return -1;
  }
  set = friendsGetTerminalEncoding();
  r = set->dec(&cbuf, buf, err);

  free(buf);
  if (r < 0) {
    return -1;
  }

  errno = 0;
  r = fprintf(fp, "%s", cbuf);
  friendsSetErrorFromErrno(err, errno);
  free(cbuf);
  return r;
}

int friendsPrintCF(FILE *fp, friendsError *err,
                   const char *format, ...)
{
  int r;
  va_list ap;
  va_start(ap, format);
  r = friendsPrintCV(fp, err, format, ap);
  va_end(ap);
  return r;
}

static void *friendsExtendBuffer(friendsChar **tbuf,
                                 friendsChar **wptr,
                                 friendsChar **limp,
                                 size_t bufsz)
{
  friendsChar *tmp;
  friendsChar *obuf;

  friendsAssert(*tbuf);
  friendsAssert(*wptr);
  friendsAssert(*limp);

  obuf = *tbuf;
  tmp = (friendsChar *)realloc(obuf, sizeof(friendsChar) * bufsz);
  if (!tmp) {
    return NULL;
  }
  *wptr = *wptr - obuf + tmp;
  *tbuf = tmp;
  *limp = tmp + bufsz;
  return tmp;
}

static int friendsGetLineCore(friendsChar **buf, FILE *fp,
                              const friendsCodeSet *ccode,
                              friendsError *err)
{
  enum { bufsz_increment_size = 16 / sizeof(friendsChar) };
  size_t bufsz;
  size_t offset;
  friendsChar *tbuf, *tmp;
  friendsChar *wptr;
  friendsChar *limp;
  friendsChar  tcmp[FRIENDS_MAX_CHAR];
  char *cbuf;
  char *ret;
  const char *cur;
  const char *cend;
  int iret;
  int i;
  int cmax;
  friendsError e;

  friendsAssert(buf);
  friendsAssert(fp);
  friendsAssert(ccode);

  if (!err) {
    e = friendsNoError;
    err = &e;
  }

  cmax = ccode->max();
  cbuf = (char *)calloc(sizeof(char), cmax + 1);
  if (!cbuf) {
    friendsSetError(err, NOMEM);
    return -1;
  }
  offset = 0;
  cend = cbuf + cmax;

  bufsz = 256 / sizeof(friendsChar);
  tbuf = (friendsChar *)calloc(sizeof(friendsChar), bufsz);
  if (!tbuf) {
    free(cbuf);
    friendsSetError(err, NOMEM);
    return -1;
  }
  wptr = tbuf;
  limp = tbuf + bufsz;

  while(1) {
    for (i = 0; i < offset; ++i) {
      if (cbuf[i] == '\n') break;
    }
    if (i == offset) {
      errno = 0;
      ret = fgets(cbuf + offset, cmax - offset + 1, fp);
      friendsSetErrorFromErrno(err, errno);
      if (ret == NULL || friendsAnyError(*err)) {
        if (ferror(fp)) {
          free(cbuf);
          free(tbuf);
          return -1;
        }
        if (feof(fp) && cbuf[0] == 0) {
          break;
        }
      }
    }

    cur = cbuf;
    while(cur < cend) {
      if (limp - wptr < FRIENDS_MAX_CHAR) {
        bufsz += bufsz_increment_size;
        tmp = friendsExtendBuffer(&tbuf, &wptr, &limp, bufsz);
        if (!tmp) {
          friendsSetError(err, NOMEM);
          free(cbuf);
          free(tbuf);
          return -1;
        }
      }

      e = friendsNoError;
      iret = ccode->one_enc(wptr, &cur, &e);
      if (iret < 0) {
        if (!friendsIsError(e, ILSEQ)) {
          if (err) *err = e;
          free(cbuf);
          free(tbuf);
          return -1;
        }
        if (cur == cbuf) {
          iret = friendsUnescapeStringLiteral(&tmp, "\\ufffd", NULL);
          if (iret < 0) {
            if (err) *err = e;
            free(cbuf);
            free(tbuf);
            return -1;
          }
          for (i = 0; tmp[i] != 0; ++i) {
            *wptr++ = tmp[i];
          }
          free(tmp);
          cur++;
        } else {
          offset = cmax - (cur - cbuf);
          break;
        }
      } else {
        offset = cmax - (cur - cbuf);
        wptr += iret;
        if (iret == 0 || wptr[-1] == 0x0a) {
          wptr[0] = 0;
          goto end;
        }
      }
    }
    for (i = 0; i < cmax; ++i) {
      if (cur - cbuf >= cmax) break;
      cbuf[i] = *cur++;
    }
    for (; i < cmax; ++i) {
      cbuf[i] = 0;
    }
  }

 end:
  *buf = tbuf;
  iret = wptr - tbuf;

  free(cbuf);
  return iret;
}

int friendsGetLineF(friendsChar **buf, FILE *fp, friendsError *err)
{
  const friendsCodeSet *ccode;
  ccode = friendsGetTerminalEncoding();
  return friendsGetLineCore(buf, fp, ccode, err);
}

int friendsGetLine(friendsChar **buf, friendsFile *fp, friendsError *err)
{
  return friendsGetLineCore(buf, fp->file, fp->encoding, err);
}
