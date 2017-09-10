
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_parser.h"
#include "friends_data.h"
#include "friends_error.h"
#include "friends_grammer.h"
#include "friends_struct_data.h"
#include "friends_list.h"

/* provided by lemon paraser generator. */
void *friendsParserCoreAlloc(void *(*allocator)(size_t));
void friendsParserCoreFree(void *parser, void (*deallocator)(void *));
void friendsParserCore(void *parser, int token, friendsParser *p);

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

  p->buffer_list = friendsNewList(err);
  p->tokens = friendsNewList(err);
  p->parsed_data = friendsNewList(err);
  p->parserData = friendsParserCoreAlloc(malloc);
  p->park = NULL;

  if (!p->buffer_list || !p->tokens || !p->parsed_data || !p->parserData) {
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
  p->listcur = NULL;

  p->curpos.column = 0;
  p->curpos.line = 1;
  p->tokpos.column = 0;
  p->tokpos.line = 1;

  return p;
}

void friendsDeleteParser(friendsParser *parser)
{
  friendsDataList *l;

  if (!parser) return;

  friendsAssert(parser->park);

  if (parser->park) {
    parser->park->parser = NULL;
  }
  l = friendsListBegin(parser->buffer_list);
  for (; l; l = friendsListNext(l)) {
    
  }

  friendsDeleteList(parser->buffer_list);
  friendsDeleteList(parser->tokens);
  friendsDeleteList(parser->parsed_data);
  friendsParserCoreFree(parser->parserData, free);
  free(parser);
}

int friendsAppendParseText(friendsParser *parser, friendsChar *text)
{
  
}
