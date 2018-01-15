
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>

#include "friends_error.h"
#include "friends_io.h"
#include "friends_list.h"
#include "friends_park.h"
#include "friends_data.h"
#include "friends_atom.h"
#include "friends_string.h"
#include "friends_parser.h"
#include "friends_lexer.h"
#include "friends_global.h"
#include "friends_lexer.h"

void friendsParserCoreTrace(FILE *traceFile, const char *prompt);

int main(int argc, char **argv)
{
  friendsPark *park;
  friendsParser *parser;
  friendsChar *t;
  friendsError e;
  friendsError perr;
  const char *prmptchar;
  int r;
  friendsLexerState ls;

  friendsParserCoreTrace(stderr, "Trace> ");
  friendsInit(argc, argv);

  t = NULL;
  e = friendsNoError;

  park = friendsNewPark(&e);
  if (!park) return 1;

  parser = friendsGetParser(park, &e);
  if (!parser) {
    friendsPrintError(friendsErrorLevelError, NULL, 0, 0,
                      NULL, e);
    friendsDeletePark(park);
    return 1;
  }

  friendsPrintCF(stdout, NULL,
                 "\\u3088\\u3046\\u3053\\u305d\\u30b8"  /* ようこそジ */
                 "\\u30e3\\u30d1\\u30ea\\u30d1\\u30fc"  /* ャパリパー */
                 "\\u30af\\u3078\\uff01\\n\\n"          /* クへ！ */);

  for (;;) {
    ls = friendsGetLexerState(parser);
    switch (ls) {
    case friendsLexerNORMAL:
      prmptchar = "> ";
      break;
    case friendsLexerPAREN:
      prmptchar = "( ";
      break;
    case friendsLexerDBRACKET:
      prmptchar = "\\u300e"; /* 『 */
      break;
    case friendsLexerUNDERSCORE:
      prmptchar = "_ ";
      break;
    default:
      friendsUnreachable();
    }

    t = NULL;
    r = friendsPromptCF(&t, &e, "%s: %03d%s",
                        "\\u30d5\\u30ec\\u30f3\\u30ba"  /* フレンズ */,
                        friendsParserCurrentLine(parser),
                        prmptchar);
    if (friendsIsError(e, NOMEM)) {
      break;
    }
    if (friendsAnyError(e)) {
      friendsPrintError(friendsErrorLevelError, NULL, 0, 0, NULL, e);
      continue;
    }
    if (r == 0) {
      friendsPrintCF(stdout, NULL, "\n");
      free(t);
      break;
    }

    friendsAppendParseText(parser, t);
    free(t);

    friendsLex(parser);

    perr = friendsGetParserError(parser);
    if (friendsIsError(perr, NOMEM)) {
      e = perr;
      break;
    }
    if (friendsAnyError(perr) && !friendsIsError(perr, Bye)) {
      friendsClearParserError(parser);
      continue;
    }

    friendsPrintTokens(parser);
    friendsParse(parser);

    if (friendsIsError(perr, Bye)) {
      break;
    }
 }

  if (friendsAnyError(e)) {
    friendsPrintError(friendsErrorLevelFatal, NULL, 0, 0, NULL, e);
  }
  friendsDeletePark(park);
  friendsFinalize();

  return 0;
}
