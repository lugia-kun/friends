
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "friends_error.h"
#include "friends_io.h"
#include "friends_global.h"
#include "friends_string.h"
#include "friends_config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

void friendsPrintErrorV(friendsErrorLevel elvl,
                        const char *filename, long int line, long int column,
                        const char *format, va_list ap)
{
  friendsError e;
  friendsChar *b, *t;
  friendsChar *fc;
  friendsStaticThreadLocal friendsErrorLevel recurs_detector = 0;
  friendsStaticThreadLocal const char *ofile;
  friendsStaticThreadLocal long int oline;
  friendsErrorLevel ret;
  const char *edesc;

  e = friendsNoError;

  ret = 0;
  if (recurs_detector) {
    if (elvl == friendsErrorLevelAssert) {
      fprintf(stderr, "fatal: %s(%ld): Assertion failed here (and recursively raised errors)\n", filename, line);
      fprintf(stderr, "note: %s(%ld): Original error location was here\n", ofile, oline);
      return;
    }
    if (elvl == friendsErrorLevelFatal) {
      fprintf(stderr, "fatal: %s(%ld): Error occured here (and recursively raised errors)\n", filename, line);
      fprintf(stderr, "note: %s(%ld): Original error location was here\n", ofile, oline);
      return;
    }
    ret = recurs_detector;
  }

  recurs_detector = elvl;
  if (ret == 0) {
    ofile = filename;
    oline = line;
  }

  friendsAsprintCV(&b, &e, format, ap);
  if (friendsAnyError(e)) goto error;

  switch (elvl) {
  case friendsErrorLevelAssert:
    edesc = "\\u30bb\\u30eb\\u30ea\\u30a2\\u30f3\\u306b"     /* セルリアンに */
      ""    "\\u98df\\u3079\\u3089\\u308c\\u305f"            /* 食べられた */
      ""    "\\u306e\\u3067\\u3059";                         /* のです */
    break;
  case friendsErrorLevelFatal:
    edesc = "\\u30d1\\u30fc\\u30af\\u306e\\u5371\\u6a5f"     /* パークの危機 */
      ""    "\\u306a\\u306e\\u3067\\u3059";                  /* なのです */
    break;
  case friendsErrorLevelWarning:
    edesc = "\\u6ce8\\u610f\\u306a\\u306e\\u3067\\u3059";    /* 注意なのです */
    break;
  case friendsErrorLevelInfo:
    edesc = "\\u60c5\\u5831\\u306a\\u306e\\u3067\\u3059";    /* 情報なのです */
    break;
  case friendsErrorLevelDebug:
    edesc = "\\u866b\\u53d6\\u308a\\u30e1\\u30e2"; /* 虫取りメモ */
    break;
  case friendsErrorLevelError:
  default:
    edesc = "\\u30a8\\u30e9\\u30fc\\u306a\\u306e\\u3067\\u3059";
    /* エラーなのです */
    break;
  }

  if (filename) {
    friendsAsprintCF(&t, &e, "%%s%s%s%s: %%ls",
                     (line > 0) ? "(%ld" : "",
                     (line > 0 && column > 0) ? ".%ld" : "",
                     (line > 0) ? ")" : "");
    if (friendsAnyError(e)) { free(b); goto error; }

    if (line > 0) {
      if (column > 0) {
        friendsAsprintF(&fc, &e, t, filename, line, column, b);
      } else {
        friendsAsprintF(&fc, &e, t, filename, line, b);
      }
    } else {
      friendsAsprintF(&fc, &e, t, filename, b);
    }
    free(t);
  } else {
    friendsAsprintCF(&fc, &e, "%ls", b);
  }
  free(b);
  if (friendsAnyError(e)) goto error;

  friendsPrintCF(stderr, &e, "%s: %ls\n", edesc, fc);
  free(fc);
  if (friendsAnyError(e)) goto error;

  recurs_detector = 0;
  return;

 error:
  switch(e) {
  case friendsErrorNOMEM:
    fprintf(stderr, "warning: %s(%ld): Could not allocate memory "
            "to print mesage\n",
            filename, line);
    break;
  case friendsErrorILSEQ:
    fprintf(stderr, "%s(%ld): warning: Illegal data sequence "
            "for message\n",
            filename, line);
    break;
  case friendsErrorPIPE:
    fprintf(stderr, "%s(%ld): warning: Pipe error occured\n",
            filename, line);
    break;
  case friendsErrorIO:
    fprintf(stderr, "%s(%ld): warning: Input/Output error occured\n",
            filename, line);
    break;
  default:
    fprintf(stderr, "%s(%ld): warning: Unknown error occurd while "
            "printing message.\n",
            filename, line);
    break;
  }
  recurs_detector = ret;
  return;
}

void friendsPrintErrorF(friendsErrorLevel elvl,
                        const char *filename, long int line, long int column,
                        const char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  friendsPrintErrorV(elvl, filename, line, column, format, ap);
  va_end(ap);
}

void friendsPrintError(friendsErrorLevel elvl,
                       const char *filename, long int line, long int column,
                       const char *ainfo, friendsError e)
{
  int r;
  friendsChar *b;

  r = friendsErrorString(&b, e);
  if (r < 0) {
    fprintf(stderr, "error: %s(%ld): error while format error message\n",
            filename, line);
    return;
  }

  if (ainfo) {
    friendsPrintErrorF(elvl, filename, line, column, "%s: %ls", ainfo, b);
  } else {
    friendsPrintErrorF(elvl, filename, line, column, "%ls", b);
  }

  free(b);
}

int friendsErrorString(friendsChar **buf, friendsError ecnv)
{
  const char *strp;
  strp = NULL;

  switch(ecnv) {
  case friendsNoError:
    strp =
      "\\u30a8\\u30e9\\u30fc\\u306f\\u306a"  /* エラーはな */
      "\\u3044\\u306e\\u3067\\u3059\\u3002"; /* いのです。 */
    break;
  case friendsErrorInvalidType:
    strp =
      "\\u578b\\u304c\\u6b63\\u3057\\u304f"  /* 型が正しく */
      "\\u306a\\u3044\\u306e\\u3067\\u3059"  /* ないのです */
      "\\u3002";                             /* 。 */
    break;
  case friendsErrorValidType:
    strp =
      "\\u578b\\u304c\\u3059\\u3067\\u306b"  /* 型がすでに */
      "\\u6c7a\\u307e\\u3063\\u3066\\u3044"  /* 決まってい */
      "\\u308b\\u306e\\u3067\\u3059\\u3002"; /* るのです。 */
    break;
  case friendsErrorListIndex:
    strp =
      "\\u30a4\\u30f3\\u30c7\\u30c3\\u30af"  /* インデック */
      "\\u30b9\\u304c\\u30ea\\u30b9\\u30c8"  /* スがリスト */
      "\\u306e\\u7bc4\\u56f2\\u5916\\u306a"  /* の範囲外な */
      "\\u306e\\u3067\\u3059\\u3002";        /* のです。 */
    break;
  case friendsErrorNoArgument:
    strp =
      "\\u30d5\\u30ec\\u30f3\\u30ba\\u306e"  /* フレンズの */
      "\\u5f15\\u6570\\u304c\\u306a\\u3044"  /* 引数がない */
      "\\u306e\\u3067\\u3059\\u3002";        /* のです。 */
    break;

  case friendsError2BIG:
  case friendsErrorOVERFLOW:
    strp =
      "\\u30d5\\u30a1\\u30a4\\u30eb\\u304c"  /* ファイルが */
      "\\u3067\\u304b\\u3059\\u304e\\u308b"  /* でかすぎる */
      "\\u306e\\u3067\\u3059\\u3002";        /* のです。 */
    break;
  case friendsErrorACCES:
  case friendsErrorPERM:
    strp =
      "\\u8a31\\u53ef\\u304c\\u306a\\u3044"  /* 許可がない */
      "\\u306e\\u3067\\u3059\\u3002";        /* のです。 */
    break;
  case friendsErrorAGAIN:
  case friendsErrorWOULDBLOCK:
    strp =
      "\\u3042\\u3068\\u3067\\u3084\\u308a"  /* あとでやり */
      "\\u76f4\\u3059\\u306e\\u3067\\u3059"  /* 直すのです */
      "\\u3002";                             /* 。 */
    break;
  case friendsErrorILSEQ:
    strp =
      "\\u6587\\u5b57\\u306e\\u4e26\\u3073"  /* 文字の並び */
      "\\u304c\\u304a\\u304b\\u3057\\u3044"  /* がおかしい */
      "\\u306e\\u3067\\u3059\\u3002";        /* のです。 */
    break;
  case friendsErrorISDIR:
    strp =
      "\\u30c7\\u30a3\\u30ec\\u30af\\u30c8"  /* ディレクト */
      "\\u30ea\\u306a\\u306e\\u3067\\u3059"  /* リなのです */
      "\\u3002";                             /* 。 */
    break;
  case friendsErrorINVAL:
    strp =
      "\\u5f15\\u6570\\u304c\\u6b63\\u3057"  /* 引数が正し */
      "\\u304f\\u306a\\u3044\\u306e\\u3067"  /* くないので */
      "\\u3059\\u3002";                      /* す。 */
    break;
  case friendsErrorNOENT:
    strp =
      "\\u305d\\u3093\\u306a\\u30d5\\u30a1"  /* そんなファ */
      "\\u30a4\\u30eb\\u3084\\u30c7\\u30a3"  /* イルやディ */
      "\\u30ec\\u30af\\u30c8\\u30ea\\u306f"  /* レクトリは */
      "\\u306a\\u3044\\u306e\\u3067\\u3059"  /* ないのです */
      "\\u3002";                             /* 。 */
    break;
  case friendsErrorNOMEM:
    strp =
      "\\u30e1\\u30e2\\u30ea\\u304c\\u8db3"  /* メモリが足 */
      "\\u308a\\u306a\\u3044\\u306e\\u3067"  /* りないので */
      "\\u3059\\u3002";                      /* す。 */
    break;
  case friendsErrorNOTDIR:
    strp =
      "\\u30c7\\u30a3\\u30ec\\u30af\\u30c8"  /* ディレクト */
      "\\u30ea\\u3067\\u306f\\u306a\\u3044"  /* リではない */
      "\\u306e\\u3067\\u3059\\u3002";        /* のです。 */
    break;
  case friendsErrorPIPE:
    strp =
      "\\u30d1\\u30a4\\u30d7\\u304c\\u58ca"  /* パイプが壊 */
      "\\u308c\\u3066\\u3044\\u308b\\u306e"  /* れているの */
      "\\u3067\\u3059\\u3002";               /* です。 */
    break;
    /* case friendsErrorPERM: */
  case friendsErrorSPIPE:
    strp =
      "\\u5dfb\\u623b\\u3057\\u3068\\u65e9"  /* 巻戻しと早 */
      "\\u9001\\u308a\\u306f\\u3067\\u304d"  /* 送りはでき */
      "\\u306a\\u3044\\u306e\\u3067\\u3059"  /* ないのです */
      "\\u3002";                             /* 。 */
    break;
  case friendsErrorRANGE:
    strp =
      "\\u7d50\\u679c\\u304c\\u8868\\u73fe"  /* 結果が表現 */
      "\\u3067\\u304d\\u308b\\u7bc4\\u56f2"  /* できる範囲 */
      "\\u3092\\u8d85\\u3048\\u3066\\u3044"  /* を超えてい */
      "\\u308b\\u306e\\u3067\\u3059\\u3002"; /* るのです。 */
    break;
    /* case friendsErrorWOULDBLOCK: */

  case friendsErrorUnknown:
  case friendsLastError:
    break;
  }
  if (strp) {
    return friendsUnescapeStringLiteral(buf, strp, NULL);
  } else {
    return friendsAsprintCF(buf, NULL,
                            "\\u5168\\u7136\\u308f\\u304b"  /* 全然わか */
                            "\\u3089\\u3093\\u3002 (%d)",   /* らん。 */
                            (int)ecnv);
  }
}

void friendsAssertImplement(const char *conditional, const char *fname,
                            long int lineno, const char *funcname)
{
  friendsPrintErrorF(friendsErrorLevelAssert, fname, lineno, 0,
                     "%s: %s", funcname, conditional);

#ifdef __GNUC__
  __builtin_trap();
#else
#if defined(POSIX_KILL) && defined(HAVE_UNISTD_H)
  kill(getpid(), SIGILL);
#else
  do {
    volatile char *p = NULL;
    *p = 1;
  } while(0);
#endif
#endif

  exit(EXIT_FAILURE);
}

void friendsUnreachableImplement(const char *fname,
                                 long int lineno, const char *funcname)
{
  friendsAssertImplement("(\\u3053\\u3053\\u304d\\u3061\\u3083" /* ここきちゃ */
                          "\\u30c0\\u30e1\\u3060\\u3063\\u3066" /* ダメだって */
                          "\\u8a00\\u3063\\u305f\\u306e\\u3067" /* 言ったので */
                          "\\u3059)",                           /* す */
                         fname, lineno, funcname);
}

void friendsSetErrorImplement(friendsError *errpvar, friendsError ev)
{
  if (errpvar) {
    *errpvar = ev;
  }
}

void friendsSetErrorFromErrno(friendsError *errpvar, int errn)
{
  friendsAssert(errpvar);

#define SET_FROM_ERRNO(esym) E##esym: do { *errpvar = friendsError##esym; } while(0)

  switch(errn) {
  case 0:  *errpvar = friendsNoError; break;
#ifdef E2BIG
  case SET_FROM_ERRNO(2BIG); break;
#endif
#ifdef EACCES
  case SET_FROM_ERRNO(ACCES); break;
#endif
#ifdef EADDRINUSE
  case SET_FROM_ERRNO(ADDRINUSE); break;
#endif
#ifdef EADDRNOTAVAIL
  case SET_FROM_ERRNO(ADDRNOTAVAIL); break;
#endif
#ifdef EAFNOSUPPORT
  case SET_FROM_ERRNO(AFNOSUPPORT); break;
#endif
#ifdef EAGAIN
#if defined(EWOULDBLOCK) && EAGAIN != EWOULDBLOCK
  case SET_FROM_ERRNO(AGAIN); break;
#endif
#endif
#ifdef EALREADY
  case SET_FROM_ERRNO(ALREADY); break;
#endif
#ifdef EBADE
  case SET_FROM_ERRNO(BADE); break;
#endif
#ifdef EBADF
  case SET_FROM_ERRNO(BADF); break;
#endif
#ifdef EBADFD
  case SET_FROM_ERRNO(BADFD); break;
#endif
#ifdef EBADMSG
  case SET_FROM_ERRNO(BADMSG); break;
#endif
#ifdef EBADR
  case SET_FROM_ERRNO(BADR); break;
#endif
#ifdef EBADRQC
  case SET_FROM_ERRNO(BADRQC); break;
#endif
#ifdef EBADSLT
  case SET_FROM_ERRNO(BADSLT); break;
#endif
#ifdef EBUSY
  case SET_FROM_ERRNO(BUSY); break;
#endif
#ifdef ECANCELED
  case SET_FROM_ERRNO(CANCELED); break;
#endif
#ifdef ECHILD
  case SET_FROM_ERRNO(CHILD); break;
#endif
#ifdef ECHRNG
  case SET_FROM_ERRNO(CHRNG); break;
#endif
#ifdef ECOMM
  case SET_FROM_ERRNO(COMM); break;
#endif
#ifdef ECONNABORTED
  case SET_FROM_ERRNO(CONNABORTED); break;
#endif
#ifdef ECONNREFUSED
  case SET_FROM_ERRNO(CONNREFUSED); break;
#endif
#ifdef ECONNRESET
  case SET_FROM_ERRNO(CONNRESET); break;
#endif
#ifdef EDEADLK
#if defined(EDEADLOCK) && EDEADLK != EDEADLOCK
  case SET_FROM_ERRNO(DEADLK); break;
#endif
#endif
#ifdef EDEADLOCK
  case SET_FROM_ERRNO(DEADLOCK); break;
#endif
#ifdef EDESTADDRREQ
  case SET_FROM_ERRNO(DESTADDRREQ); break;
#endif
#ifdef EDOM
  case SET_FROM_ERRNO(DOM); break;
#endif
#ifdef EDQUOT
  case SET_FROM_ERRNO(DQUOT); break;
#endif
#ifdef EEXIST
  case SET_FROM_ERRNO(EXIST); break;
#endif
#ifdef EFAULT
  case SET_FROM_ERRNO(FAULT); break;
#endif
#ifdef EFBIG
  case SET_FROM_ERRNO(FBIG); break;
#endif
#ifdef EHOSTDOWN
  case SET_FROM_ERRNO(HOSTDOWN); break;
#endif
#ifdef EHOSTUNREACH
  case SET_FROM_ERRNO(HOSTUNREACH); break;
#endif
#ifdef EIDRM
  case SET_FROM_ERRNO(IDRM); break;
#endif
#ifdef EILSEQ
  case SET_FROM_ERRNO(ILSEQ); break;
#endif
#ifdef EINPROGRESS
  case SET_FROM_ERRNO(INPROGRESS); break;
#endif
#ifdef EINTR
  case SET_FROM_ERRNO(INTR); break;
#endif
#ifdef EINVAL
  case SET_FROM_ERRNO(INVAL); break;
#endif
#ifdef EIO
  case SET_FROM_ERRNO(IO); break;
#endif
#ifdef EISCONN
  case SET_FROM_ERRNO(ISCONN); break;
#endif
#ifdef EISDIR
  case SET_FROM_ERRNO(ISDIR); break;
#endif
#ifdef EISNAM
  case SET_FROM_ERRNO(ISNAM); break;
#endif
#ifdef EL2HLT
  case SET_FROM_ERRNO(L2HLT); break;
#endif
#ifdef EL2NSYNC
  case SET_FROM_ERRNO(L2NSYNC); break;
#endif
#ifdef EL3HLT
  case SET_FROM_ERRNO(L3HLT); break;
#endif
#ifdef EL3RST
  case SET_FROM_ERRNO(L3RST); break;
#endif
#ifdef ELIBACC
  case SET_FROM_ERRNO(LIBACC); break;
#endif
#ifdef ELIBBAD
  case SET_FROM_ERRNO(LIBBAD); break;
#endif
#ifdef ELIBMAX
  case SET_FROM_ERRNO(LIBMAX); break;
#endif
#ifdef ELIBSCN
  case SET_FROM_ERRNO(LIBSCN); break;
#endif
#ifdef ELIBEXEC
  case SET_FROM_ERRNO(LIBEXEC); break;
#endif
#ifdef ELOOP
  case SET_FROM_ERRNO(LOOP); break;
#endif
#ifdef EMFILE
  case SET_FROM_ERRNO(MFILE); break;
#endif
#ifdef EMLINK
  case SET_FROM_ERRNO(MLINK); break;
#endif
#ifdef EMSGSIZE
  case SET_FROM_ERRNO(MSGSIZE); break;
#endif
#ifdef EMULTIHOP
  case SET_FROM_ERRNO(MULTIHOP); break;
#endif
#ifdef ENAMETOOLONG
  case SET_FROM_ERRNO(NAMETOOLONG); break;
#endif
#ifdef ENETDOWN
  case SET_FROM_ERRNO(NETDOWN); break;
#endif
#ifdef ENETRESET
  case SET_FROM_ERRNO(NETRESET); break;
#endif
#ifdef ENETUNREACH
  case SET_FROM_ERRNO(NETUNREACH); break;
#endif
#ifdef ENFILE
  case SET_FROM_ERRNO(NFILE); break;
#endif
#ifdef ENOBUFS
  case SET_FROM_ERRNO(NOBUFS); break;
#endif
#ifdef ENODATA
  case SET_FROM_ERRNO(NODATA); break;
#endif
#ifdef ENODEV
  case SET_FROM_ERRNO(NODEV); break;
#endif
#ifdef ENOENT
  case SET_FROM_ERRNO(NOENT); break;
#endif
#ifdef ENOEXEC
  case SET_FROM_ERRNO(NOEXEC); break;
#endif
#ifdef ENOKEY
  case SET_FROM_ERRNO(NOKEY); break;
#endif
#ifdef ENOLCK
  case SET_FROM_ERRNO(NOLCK); break;
#endif
#ifdef ENOLINK
  case SET_FROM_ERRNO(NOLINK); break;
#endif
#ifdef ENOMEM
  case SET_FROM_ERRNO(NOMEM); break;
#endif
#ifdef ENOMSG
  case SET_FROM_ERRNO(NOMSG); break;
#endif
#ifdef ENONET
  case SET_FROM_ERRNO(NONET); break;
#endif
#ifdef ENOPROTOOPT
  case SET_FROM_ERRNO(NOPROTOOPT); break;
#endif
#ifdef ENOSPC
  case SET_FROM_ERRNO(NOSPC); break;
#endif
#ifdef ENOSR
  case SET_FROM_ERRNO(NOSR); break;
#endif
#ifdef ENOSTR
  case SET_FROM_ERRNO(NOSTR); break;
#endif
#ifdef ENOSYS
  case SET_FROM_ERRNO(NOSYS); break;
#endif
#ifdef ENOTBLK
  case SET_FROM_ERRNO(NOTBLK); break;
#endif
#ifdef ENOTCONN
  case SET_FROM_ERRNO(NOTCONN); break;
#endif
#ifdef ENOTDIR
  case SET_FROM_ERRNO(NOTDIR); break;
#endif
#ifdef ENOTEMPTY
  case SET_FROM_ERRNO(NOTEMPTY); break;
#endif
#ifdef ENOTSOCK
  case SET_FROM_ERRNO(NOTSOCK); break;
#endif
#ifdef ENOTSUP
#if defined(EOPNOTSUPP) && ENOTSUP != EOPNOTSUPP
  case SET_FROM_ERRNO(NOTSUP); break;
#endif
#endif
#ifdef ENOTTY
  case SET_FROM_ERRNO(NOTTY); break;
#endif
#ifdef ENOTUNIQ
  case SET_FROM_ERRNO(NOTUNIQ); break;
#endif
#ifdef ENXIO
  case SET_FROM_ERRNO(NXIO); break;
#endif
#ifdef EOPNOTSUPP
  case SET_FROM_ERRNO(OPNOTSUPP); break;
#endif

#ifdef EOVERFLOW
  case SET_FROM_ERRNO(OVERFLOW); break;
#endif
#ifdef EPERM
  case SET_FROM_ERRNO(PERM); break;
#endif
#ifdef EPFNOSUPPORT
  case SET_FROM_ERRNO(PFNOSUPPORT); break;
#endif
#ifdef EPIPE
  case SET_FROM_ERRNO(PIPE); break;
#endif
#ifdef EPROTO
  case SET_FROM_ERRNO(PROTO); break;
#endif
#ifdef EPROTONOSUPPORT
  case SET_FROM_ERRNO(PROTONOSUPPORT); break;
#endif
#ifdef EPROTOTYPE
  case SET_FROM_ERRNO(PROTOTYPE); break;
#endif
#ifdef ERANGE
  case SET_FROM_ERRNO(RANGE); break;
#endif
#ifdef EREMCHG
  case SET_FROM_ERRNO(REMCHG); break;
#endif
#ifdef EREMOTE
  case SET_FROM_ERRNO(REMOTE); break;
#endif
#ifdef EREMOTEIO
  case SET_FROM_ERRNO(REMOTEIO); break;
#endif
#ifdef ERESTART
  case SET_FROM_ERRNO(RESTART); break;
#endif
#ifdef EROFS
  case SET_FROM_ERRNO(ROFS); break;
#endif
#ifdef ESHUTDOWN
  case SET_FROM_ERRNO(SHUTDOWN); break;
#endif
#ifdef ESPIPE
  case SET_FROM_ERRNO(SPIPE); break;
#endif
#ifdef ESOCKTNOSUPPORT
  case SET_FROM_ERRNO(SOCKTNOSUPPORT); break;
#endif
#ifdef ESRCH
  case SET_FROM_ERRNO(SRCH); break;
#endif
#ifdef ESTALE
  case SET_FROM_ERRNO(STALE); break;
#endif

#ifdef ESTRPIPE
  case SET_FROM_ERRNO(STRPIPE); break;
#endif
#ifdef ETIME
  case SET_FROM_ERRNO(TIME); break;
#endif

#ifdef ETIMEDOUT
  case SET_FROM_ERRNO(TIMEDOUT); break;
#endif
#ifdef ETXTBSY
  case SET_FROM_ERRNO(TXTBSY); break;
#endif
#ifdef EUCLEAN
  case SET_FROM_ERRNO(UCLEAN); break;
#endif
#ifdef EUNATCH
  case SET_FROM_ERRNO(UNATCH); break;
#endif
#ifdef EUSERS
  case SET_FROM_ERRNO(USERS); break;
#endif
#ifdef EWOULDBLOCK
  case SET_FROM_ERRNO(WOULDBLOCK); break;
#endif
#ifdef EXDEV
  case SET_FROM_ERRNO(XDEV); break;
#endif
#ifdef EXFULL
  case SET_FROM_ERRNO(XFULL); break;
#endif

  default: *errpvar = friendsErrorUnknown; break;
  }
}

void friendsClearError(friendsError *e)
{
  if (!e) return;

  *e = friendsNoError;
}
