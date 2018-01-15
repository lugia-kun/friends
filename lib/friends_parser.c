
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_parser.h"
#include "friends_data.h"
#include "friends_error.h"
#include "friends_grammer.h"
#include "friends_struct_data.h"
#include "friends_park.h"
#include "friends_string.h"
#include "friends_grammer.h"
#include "friends_token.h"
#include "friends_io.h"

/* provided by lemon paraser generator. */
void *friendsParserCoreAlloc(void *(*allocator)(size_t));
void friendsParserCoreFree(void *parser, void (*deallocator)(void *));
void friendsParserCore(void *parser, int token, friendsToken *data,
                       friendsParser *p);

friendsParser *friendsNewParser(friendsPark *park, friendsError *err)
{
  friendsParser *p;

  friendsAssert(park);

  if (park->parser) return NULL;

  p = (friendsParser *)calloc(sizeof(friendsParser), 1);
  if (!p) {
    friendsSetError(err, NOMEM);
    return NULL;
  }

  p->tokens = friendsNewTokenList(err);
  p->parsed_data = friendsNewDataList(err);
  p->parserData = friendsParserCoreAlloc(malloc);
  p->park = NULL;

  if (!p->tokens || !p->parsed_data || !p->parserData) {
    friendsDeleteParser(p);
    return NULL;
  }

  p->bufp = NULL;
  p->bufp_size = 0;

  p->park = park;
  p->cur = NULL;
  p->mark = NULL;
  p->cmark = NULL;
  p->token = NULL;
  p->stoken = NULL;
  p->limit = NULL;

  p->last_type = 0;
  p->lexer_state = friendsLexerNORMAL;
  p->err = friendsNoError;
  p->tokencur = NULL;

  p->curpos.column = 1;
  p->curpos.line = 1;
  p->tokpos.column = 1;
  p->tokpos.line = 1;

  p->filename = "-";

  return p;
}

void friendsDeleteParser(friendsParser *parser)
{
  if (!parser) return;

  if (parser->park && parser->park->parser == parser) {
    parser->park->parser = NULL;
  }

  free(parser->bufp);
  friendsTokenDeleteAll(parser->tokens);
  friendsDataListDeleteAll(parser->parsed_data);
  friendsParserCoreFree(parser->parserData, free);
  free(parser);
}

friendsBool friendsParse(friendsParser *parser)
{
  friendsToken *tokcur;
  friendsToken *tokend;
  friendsToken *toknext;
  friendsBool ret;

  friendsAssert(parser);
  friendsAssert(parser->parserData);
  friendsAssert(parser->tokens);
  friendsAssert(parser->parsed_data);

  ret = friendsTrue;
  tokcur = parser->tokencur;
  if (!tokcur) {
    tokcur = friendsTokenNext(parser->tokens);
  } else {
    tokcur = friendsTokenNext(tokcur);
  }
  tokend = friendsTokenPrev(parser->tokens);
  for (; tokcur != parser->tokens; tokcur = friendsTokenNext(tokcur)) {
    int tt;
    tt = friendsTokenType(tokcur);
    if (tokcur == tokend) {
      if (tt == friendsEND || tt == friendsTANOSHI ||
          tt == friendsNANDAKKE ||
          tt == friendsSEPR || tt == friendsNANDANE) {
        /* NOP */
      } else {
        tokend = tokcur;
        break;
      }
    }
    friendsParserCore(parser->parserData, tt, tokcur, parser);
  }
  tokcur = friendsTokenNext(parser->tokens);
  toknext = NULL;
  for (; tokcur != tokend; tokcur = toknext) {
    toknext = friendsTokenNext(tokcur);

    if (friendsTokenMarkDeleted(tokcur) == friendsTrue) {
      if (tokcur == parser->tokencur) {
        parser->tokencur = toknext;
      }
      friendsTokenDelete(tokcur);
    }
  }
  parser->tokencur = friendsTokenPrev(parser->tokencur);
  if (friendsAnyError(parser->err) && !friendsIsError(parser->err, Bye)) {
    ret = friendsFalse;
  }
  return ret;
}

friendsBool
friendsAppendParseText(friendsParser *parser, const friendsChar *text)
{
  friendsStringList *l;

  friendsAssert(parser);
  friendsAssert(text);

  l = friendsStringListInsert(parser->buffer_list, text, NULL, &parser->err);

  if (l) {
    return friendsTrue;
  } else {
    return friendsFalse;
  }
}

friendsError friendsGetParserError(friendsParser *parser)
{
  if (!parser) return friendsErrorUnknown;

  return parser->err;
}

void friendsClearParserError(friendsParser *parser)
{
  if (!parser) return;

  friendsClearError(&parser->err);
}

long friendsParserCurrentLine(friendsParser *parser)
{
  if (!parser) return -1;

  return parser->curpos.line;
}

long friendsParserCurrentColumn(friendsParser *parser)
{
  if (!parser) return -1;

  return parser->curpos.column;
}

void friendsSetParserFileName(friendsParser *parser, const char *filename)
{
  friendsAssert(parser);
  friendsAssert(filename);

  parser->filename = filename;
  parser->curpos.line = 1;
  parser->curpos.column = 1;
}

const char *friendsParserFileName(friendsParser *parser)
{
  friendsAssert(parser);

  return parser->filename;
}

friendsLexerState friendsGetLexerState(friendsParser *parser)
{
  friendsAssert(parser);

  return parser->lexer_state;
}

void friendsPrintTokens(friendsParser *parser)
{
  friendsToken *t;
  const char *token_as_text;
  const friendsChar *token_text;
  long line;
  long col;
  friendsChar *lct;
  const char *pmark;

#define friendsTOKEN(name) \
  friends##name: token_as_text = #name; goto case##name; case##name

  friendsAssert(parser);

  t = friendsTokenNext(parser->tokens);
  if (t) {
    friendsPrintCF(stdout, NULL, "----------------------------------\n");
    for (; t != parser->tokens; t = friendsTokenNext(t)) {
      pmark = " ";
      if (t == parser->tokencur) {
        pmark = "*";
      }
      switch (friendsTokenType(t)) {
      case friendsTOKEN(END): break;
      case friendsTOKEN(SUGOI): break;
      case friendsTOKEN(NANDANE): break;
      case friendsTOKEN(TANOSHI): break;
      case friendsTOKEN(THEN): break;
      case friendsTOKEN(NANDAKKE): break;
      case friendsTOKEN(AND): break;
      case friendsTOKEN(FRIEND_NAME): break;
      case friendsTOKEN(FRIEND): break;
      case friendsTOKEN(PARTICLE): break;
      case friendsTOKEN(VARIABLE): break;
      case friendsTOKEN(ATOM): break;
      case friendsTOKEN(TSUGI): break;
      case friendsTOKEN(LKAGI): break;
      case friendsTOKEN(RKAGI): break;
      case friendsTOKEN(NO): break;
      case friendsTOKEN(SEPR): break;
      default:
        token_as_text = "???";
        break;
      }
      token_text = friendsTokenText(t);
      line = friendsTokenLinePosition(t);
      col = friendsTokenColumnPosition(t);
      if (line > 0 && col > 0) {
        line = friendsAsprintCF(&lct, NULL, "%5ld %5ld", line, col);
      } else {
        line = friendsAsprintCF(&lct, NULL, "%5s %5s", "-", "-");
      }
      if (token_text) {
        if (line > 0) {
          friendsPrintCF(stdout, NULL, "%s%11s %ls %ls\n",
                         pmark, token_as_text, lct, token_text);
        } else {
          friendsPrintCF(stdout, NULL, "%s%11s %5s %5s %ls\n",
                         pmark, token_as_text, "?", "?", token_text);
        }
      } else {
        if (line > 0) {
          friendsPrintCF(stdout, NULL, "%s%11s %ls ?????\n",
                         pmark, token_as_text, lct);
        } else {
          friendsPrintCF(stdout, NULL, "%s%11s %5s %5s ?????\n",
                         pmark, token_as_text, "-", "-");
        }
      }
      if (line > 0) {
        free(lct);
      }
    }
  }
}
