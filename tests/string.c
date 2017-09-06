
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "test_common.h"

#include "friends_defs.h"
#include "friends_config.h"

#include "friends_io.h"
#include "friends_string.h"
#include "friends_test.h"

int main(int argc, char **argv)
{
  const friendsChar test_data[] = {
#ifdef FRIENDS_USE_UTF8_INTERNAL
    0xe3, 0x80, 0x80, 0x00,
#else
    0x3000, 0x0000,
#endif
  };

  const friendsChar *ep;
  friendsChar *buf;
  friendsError e;
  int ret;
  long int li;
  size_t sret;

  e = friendsNoError;
  buf = NULL;
  ret = friendsUnescapeStringLiteral(&buf, "\\u3000", &e);

#ifdef FRIENDS_USE_UTF8_INTERNAL
  friendsTestExpect(ret, 4, "%d", friendsTestEqual);
  if (ret == 4) {
    friendsTestExpect(buf[0], 0xe3, "%02x", friendsTestEqual);
    friendsTestExpect(buf[1], 0x80, "%02x", friendsTestEqual);
    friendsTestExpect(buf[2], 0x80, "%02x", friendsTestEqual);
    friendsTestExpect(buf[3], 0x00, "%02x", friendsTestEqual);
  }
#else
  friendsTestExpect(ret, 2, "%d", friendsTestEqual);
  if (ret == 2) {
    friendsTestExpect(buf[0], 0x3000, "%02x", friendsTestEqual);
    friendsTestExpect(buf[1], 0x0000, "%02x", friendsTestEqual);
  }
#endif
  if_error_goto_error(e, "friendsUnescapeStringLiteral", error);

  ret = friendsStringCompare(buf, test_data);
  friendsTestExpect(ret, 0, "%d", friendsTestEqual);

  free(buf);
  buf = NULL;

  ret = friendsUnescapeStringLiteral(&buf, "1234567890", &e);
  if_error_goto_error(e, "friendsUnescapeStringLiteral", error);

  li = friendsStringToLong(buf, &ep, 10, &e);
#if LONG_MAX < 1234567890L
  friendsTestExpect(e, friendsErrorRANGE, "%d", friendsTestEqual);
  friendsPrintErrorF(friendsErrorLevelWarning, FRIENDS_SOURCE_FILE,
                     __LINE__, 0,
                     "1234567890 \\u306f\\u3042"       /* 1234567890 はあ */
                     "\\u306a\\u305f\\u306e\\u30b3\\u30f3"  /* なたのコン */
                     "\\u30d4\\u30e5\\u30fc\\u30bf\\u3067"  /* ピュータで */
                     "\\u306f\\u8868\\u73fe\\u3067\\u304d"  /* は表現でき */
                     "\\u306a\\u3044\\u307f\\u305f\\u3044"  /* ないみたい */
                     "\\u306a\\u306e\\u3067\\u3059\\u3002"  /* なのです。 */);
#else
  if_error_goto_error(e, "friendsUnescapeStringLiteral", error);
#endif

  if (ep) {
    friendsTestExpectText(ep, "");
  }
  friendsTestExpect(li, 1234567890L, "%ld", friendsTestEqual);

  li = friendsStringToLong(buf, &ep, 0, &e);
  if (ep) {
    friendsTestExpectText(ep, "");
  }
  friendsTestExpect(li, 1234567890L, "%ld", friendsTestEqual);

  li = friendsStringToLong(buf, &ep, 8, &e);
  if (ep) {
    friendsTestExpectText(ep, "890");
  }
  friendsTestExpect(li, 01234567L, "%ld", friendsTestEqual);

  free(buf);
  buf = NULL;

  ret = friendsUnescapeStringLiteral(&buf, " 0\\uff380\\uff26f" /* 0x0ff */,
                                     &e);
  if_error_goto_error(e, "friendsUnescapeStringLiteral", error);

  li = friendsStringToLong(buf, &ep, 0, &e);
  friendsTestExpect(li, 0x0ffL, "%#lx", friendsTestEqual);

  free(buf);
  buf = NULL;

  ret = friendsUnescapeStringLiteral(&buf, "\\uff5azZ\\uff3a" /* zzzz */,
                                     &e);
  if_error_goto_error(e, "friendsUnescapeStringLiteral", error);

  li = friendsStringToLong(buf, &ep, 36, &e);
  friendsTestExpect(li, 1679615L, "%ld", friendsTestEqual);

  free(buf);
  buf = NULL;

  ret = friendsUnescapeStringLiteral(&buf, "ffffffffffffffff",
                                     &e);
  if_error_goto_error(e, "friendsUnescapeStringLiteral", error);

  li = friendsStringToLong(buf, &ep, 16, &e);
  friendsTestExpect(e, friendsErrorRANGE, "%d", friendsTestEqual);

  free(buf);
  buf = NULL;

  return 0;

 error:
  if (ret == 0) return 1;
  free(buf);
  return ret;
}
