/**
 * @file friends_io.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_IO_H
#define FRIENDS_IO_H

#include <stdio.h>
#include <stdarg.h>

#include "friends_defs.h"

int friendsAsprintF(friendsChar **buf, friendsError *err,
                    const friendsChar *format, ...);
int friendsAsprintV(friendsChar **buf, friendsError *err,
                    const friendsChar *format, va_list ap);
int friendsAsprintCF(friendsChar **buf, friendsError *err,
                     const char *format, ...);
int friendsAsprintCV(friendsChar **buf, friendsError *err,
                     const char *format, va_list ap);

int friendsFprintF(friendsFile *fp, friendsError *err,
                   const friendsChar *format, ...);
int friendsFprintV(friendsFile *fp, friendsError *err,
                   const friendsChar *format, va_list ap);
int friendsFprintCF(friendsFile *fp, friendsError *err,
                    const char *format, ...);
int friendsFprintCV(friendsFile *fp, friendsError *err,
                    const char *format, va_list ap);

int friendsPrintF(FILE *fp, friendsError *err,
                  const friendsChar *format, ...);
int friendsPrintV(FILE *fp, friendsError *err,
                  const friendsChar *format, va_list ap);
int friendsPrintCF(FILE *fp, friendsError *err,
                   const char *format, ...);
int friendsPrintCV(FILE *fp, friendsError *err,
                   const char *format, va_list ap);

int friendsGetLine(friendsChar **buf, FILE *fp, friendsError *err);

#endif
