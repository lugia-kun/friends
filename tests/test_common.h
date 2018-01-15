#ifndef FRIENDS_TEST_COMMON_H
#define FRIENDS_TEST_COMMON_H

#include "friends_defs.h"
#include "friends_data.h"
#include "friends_atom.h"
#include "friends_error.h"
#include "friends_io.h"

/**
 * @brief エラーなら error に飛ぶのです。
 * @param e    エラーかどうかを確かめる変数をよこすのです。
 * @param func エラーを起こした関数名をよこすのです。
 * @param error 飛ぶ先のラベル名をよこすのです。
 *
 * このマクロは、以下の識別子を使うのです。
 */
#define if_error_goto_error(e, func, error)                             \
  do {                                                                  \
    if (friendsAnyError(e)) {                                           \
      friendsPrintError(friendsErrorLevelError, FRIENDS_SOURCE_FILE,    \
                        __LINE__, 0, func, e);                          \
      goto error;                                                       \
    }                                                                   \
  } while(0)

/**
 * @brief
 */
#if 0
static inline friendsData *
newData(friendsPark *park, friendsAtomType t, int numeral, const char *txt,
        friendsError *e, ...)
{
  va_list ap;
  friendsData *d;
  friendsError ee;
  friendsChar *ftxt;
  if (!e) {
    ee = friendsNoError;
    e = &ee;
  }

  d = friendsNewData(park, e);
  if (!d) return NULL;

  switch(t) {
  case friendsNumericAtom:
    friendsSetNumeralAtom(d, numeral, e);
    if (friendsAnyError(*e)) return NULL;
    break;
  case friendsTextAtom:
    va_start(ap, e);
    friendsAsprintCV(&ftxt, e, txt, ap);
    va_end(ap);
    if (friendsAnyError(*e)) return NULL;
    friendsSetTextAtom(d, ftxt, e);
    if (friendsAnyError(*e)) { free(ftxt); return NULL; }
    break;
  case friendsNextAtom:
    friendsSetNextAtom(d, e);
    if (friendsAnyError(*e)) return NULL;
    break;
  default:
    return NULL;
  }

  return d;
}
#endif

#endif
