
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

int friendsGetLine(friendsChar **buf, FILE *fp, friendsError *err)
{
  size_t bufsz;
  size_t offset;
  size_t offdiff;
  const friendsCodeSet *ccode;
  char *cbuf;
  char *ret;
  int iret;
  friendsError e;

  friendsAssert(buf);
  friendsAssert(fp);

  ccode = friendsGetTerminalEncoding();
  friendsAssert(ccode);

  if (!err) {
    e = friendsNoError;
    err = &e;
  }

  bufsz = 256;
  offdiff = bufsz - 1;
  offset = 0;
  cbuf = (char *)calloc(sizeof(char), bufsz);
  if (!cbuf) {
    friendsSetError(err, NOMEM);
    return -1;
  }

  while(1) {
    errno = 0;
    cbuf[bufsz - 2] = '\0';
    ret = fgets(cbuf + offset, bufsz - offset, fp);
    friendsSetErrorFromErrno(err, errno);
    if (ret == NULL || friendsAnyError(*err)) {
      if (feof(fp)) {
        break;
      }
      free(cbuf);
      return -1;
    }
    cbuf[bufsz - 1] = '\0';
    if (cbuf[bufsz - 2] == '\0' ||
        cbuf[bufsz - 2] == '\n') {
      break;
    }
    bufsz  += offdiff;
    offset += offdiff;
    ret = realloc(cbuf, bufsz);
    if (!ret) {
      friendsSetError(err, NOMEM);
      free(cbuf);
      return -1;
    }
    cbuf = ret;
  }

  iret = ccode->enc(buf, cbuf, err);
  free(cbuf);
  if (friendsAnyError(*err)) {
    return -1;
  }

  return iret;
}
