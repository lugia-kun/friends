/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "friends_string.h"
#include "friends_error.h"
#include "friends_pathname.h"

/*!re2c
  re2c:define:YYCTYPE = "friendsChar";
  re2c:indent:string = "  ";
  re2c:yyfill:enable = 0;
*/

static const friendsChar *lastSeparator(const friendsChar *input)
{
  const friendsChar *lastS;
  const friendsChar *token;
  const friendsChar *YYCURSOR;
  const friendsChar *YYMARKER;

  YYCURSOR = input;
  lastS = NULL;
  while (1) {
    token = YYCURSOR;
    /*!re2c
      re2c:indent:top = 2;

      *       { continue; }
      "\x00"  { break; }
      "/"+    { goto slash; }
      "\\"+   { goto backslash; }
    */
    friendsUnreachable();

  backslash:
#ifdef WIN32
    goto slash;
#else
    continue;
#endif

  slash:
    if (token == input) {
      lastS = YYCURSOR;
    } else {
      lastS = token;
    }
    continue;
  }
  return lastS;
}

size_t friendsDirname(friendsChar **output,
                      const friendsChar *input,
                      friendsError *err)
{
  size_t c;
  friendsChar *retp;
  const friendsChar *lastS;

  friendsAssert(input);
  friendsAssert(output);

  lastS = lastSeparator(input);
  if (lastS) {
    c = lastS - input + 1;
    retp = (friendsChar *)malloc(sizeof(friendsChar) * c);
    if (!retp) {
      friendsSetError(err, NOMEM);
      return -1;
    }
    memcpy(retp, input, sizeof(friendsChar) * (c - 1));
    retp[c - 1] = 0;
    *output = retp;
  } else {
    c = friendsUnescapeStringLiteral(output, ".", err);
  }
  return c;
}

size_t friendsBasename(friendsChar **output,
                       const friendsChar *input,
                       friendsError *err)
{
  size_t c;
  friendsChar *retp;
  const friendsChar *lastS;

  friendsAssert(input);
  friendsAssert(output);

  lastS = lastSeparator(input);
  if (lastS) {
    const friendsChar *YYCURSOR;
    YYCURSOR = lastS;
    while (1) {
      /*!re2c
        re2c:indent:top = 3;

        *    { break; }
        "/"  { continue; }
        "\\" {
#ifdef WIN32
          continue;
#else
          break;
#endif
        }
      */
    }
  } else {
    lastS = input;
  }
  c = friendsStringArrayLength(lastS);
  if (c > 0) {
    retp = (friendsChar *)malloc(sizeof(friendsChar) * (c + 1));
    if (!retp) {
      friendsSetError(err, NOMEM);
      return 0;
    }
    friendsCopyString(retp, input);
    *output = retp;
  } else {
    *output = NULL;
  }
  return c;
}

/* Interaction with normal char */
/*!re2c
  re2c:define:YYCTYPE = "unsigned char";
  re2c:flags:x = 0;
  re2c:flags:8 = 0;
*/
