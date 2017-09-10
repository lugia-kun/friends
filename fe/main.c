
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


int main(int argc, char **argv)
{
  friendsPark *park;
  friendsParser *parser;
  friendsChar *p;
  friendsChar *t;
  friendsError e;

  setlocale(LC_CTYPE, "");

#if 0
  t = NULL;
  p = NULL;
  e = friendsNoError;

  friendsUnescapeStringLiteral(&p,
                               "\\u30d5\\u30ec\\u30f3\\u30ba> "  /* フレンズ> */,
                               &e);
  friendsPrompt(&t, p, &e);
  if (friendsAnyError(e)) {
    friendsPrintError(friendsErrorLevelError, NULL, 0, 0, NULL, e);
  } else {
    friendsPrintCF(stdout, &e, "%ls", t);
  }

  free(p);
  free(t);
#endif

  park = friendsNewPark(&e);
  if (!park) return 1;

  parser = friendsGetParser(park, &e);
  

  friendsDeletePark(park);
  
  return 0;
}
