
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_common.h"

#include "friends_lexer.h"
#include "friends_park.h"
#include "friends_string.h"
#include "friends_io.h"
#include "friends_global.h"
#include "friends_parser.h"
#include "enc/utf8.h"

int main(int argc, char **argv)
{
  friendsPark *park;
  friendsError e, perr;
  friendsChar *t;

  friendsParser *parser;
  friendsFile *fp;
  const char *fn;

  int r;

  parser = NULL;
  fp = NULL;
  park = NULL;

  e = friendsNoError;
  if (argc < 2) {
    friendsPrintCF(stdout, NULL, "Usage: %s [Input]\n",  argv[0]);
    return EXIT_FAILURE;
  }

  friendsInit(argc, argv);

  fp = NULL;
  park = friendsNewPark(&e);
  if (friendsAnyError(e)) goto error;

  parser = friendsGetParser(park, &e);

  /* テスト用の入力データは UTF-8 に限定するのです。 */
  if (strcmp(argv[1], "-") == 0) {
    fp = friendsFileForFILE(stdin, friendsUtf8Set(), &e);
  } else {
    fp = friendsOpenFile(argv[1], "rb", friendsUtf8Set(), &e);
  }
  if (!fp) goto error;
  friendsSetParserFileName(parser, argv[1]);

  parser = friendsGetParser(park, &e);
  if (friendsAnyError(e)) goto error;

  while (friendsFileEOF(fp) != friendsTrue) {
    t = NULL;
    friendsGetLine(&t, fp, &e);
    if (friendsAnyError(e)) break;
    /* EOF */
    if (!t) {
      /* 改行で終わっていなかった場合の対応をするのです */
      friendsUnescapeStringLiteral(&t, "\n", NULL);
      if (t) {
        friendsAppendParseText(parser, t);
        friendsLex(parser);
        free(t);
      }
      break;
    }

    friendsAppendParseText(parser, t);
    free(t);

    friendsLex(parser);
    perr = friendsGetParserError(parser);
    if (friendsIsError(perr, NOMEM)) break;
    friendsClearParserError(parser);
  }

  friendsPrintTokens(parser);
  if (friendsAnyError(e)) goto error;

  e = friendsGetParserError(parser);

  friendsCloseFile(fp);
  fp = NULL;

  friendsDeletePark(park);
  friendsFinalize();

  if (friendsAnyError(e)) {
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }

 error:
  if (friendsAnyError(e)) {
    const char *fn;
    long l;

    fn = NULL;
    l = 0;

    if (parser) fn = friendsParserFileName(parser);
    if (parser) l = friendsParserCurrentLine(parser);
    friendsPrintError(friendsErrorLevelError, fn, l, 0, NULL, e);

    if (fp) friendsCloseFile(fp);
  }
  if (park) friendsDeletePark(park);
  friendsFinalize();
  return EXIT_FAILURE;
}
