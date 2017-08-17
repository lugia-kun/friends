
#include <stdlib.h>
#include <stdarg.h>

#include "friends-config.h"
#include "friends_io.h"
#include "friends_test.h"
#include "friends_error.h"
#include "friends_string.h"
#include "friends_list.h"

friendsStaticThreadLocal int friendsTestErrorCountValue = 0;

void friendsTestExpectPrint(const char *file, long line,
                            const char *data_format, ...)
{
  va_list ap;
  friendsChar *s, *t;
  friendsError e;

  friendsTestErrorCountValue += 1;

  e = friendsNoError;
  friendsAsprintCF(&s, &e,
                   "%%s \\u306f %s \\u306a"             /* %%s は %s な */
                   "\\u306e\\u3067\\u3059\\u304c "      /* のですが  */
                   "%s \\u306e\\u306f\\u305a\\u306a"    /* %s のはずな */
                   "\\u306e\\u3067\\u3059\\u3002 (%d)", /* のです。 */
                   data_format, data_format, friendsTestErrorCountValue);
  if (friendsAnyError(e)) {
    friendsPrintError(friendsErrorLevelError, file, line, 0,
                      "Could not format message", e);
    return;
  }

  va_start(ap, data_format);
  friendsAsprintV(&t, &e, s, ap);
  va_end(ap);
  free(s);

  if (friendsAnyError(e)) {
    friendsPrintError(friendsErrorLevelError, file, line, 0,
                      "Could not format message", e);
    return;
  }

  friendsPrintErrorF(friendsErrorLevelError, file, line, 0,
                     "%ls", t);
  free(t);
}

int friendsTestErrorCount(void)
{
  return friendsTestErrorCountValue;
}

void friendsTestExpectTextImplement(const friendsChar *object,
                                    const char *expect,
                                    const char *file, long line,
                                    const char *object_name)
{
  friendsChar *fs;
  friendsError e;

  friendsAssert(expect);

  e = friendsNoError;
  friendsUnescapeStringLiteral(&fs, expect, &e);
  if (friendsAnyError(e)) {
    friendsPrintError(friendsErrorLevelError, file, line, 0,
                      "Could not format message", e);
    return;
  }

  if (!object || friendsStringCompare(object, fs) != 0) {
    friendsTestExpectPrint(file, line, "%ls", object_name, object, fs);
  }

  free(fs);
}
