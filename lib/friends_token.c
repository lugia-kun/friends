
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_error.h"
#include "friends_list.h"
#include "friends_string.h"
#include "friends_core.h"

struct friendsToken
{
  friendsList list;
  friendsChar *token;
  int token_type;
  friendsLineColumn lc;
  friendsBool delete;
};
#define friendsTokenContainer(ptr) \
  friendsListContainer(ptr, struct friendsToken, list)

friendsToken *friendsNewTokenList(friendsError *e)
{
  friendsToken *t;
  t = (friendsToken *)friendsMalloc(sizeof(friendsToken), e);
  if (t) {
    t->token = NULL;
    friendsListInit(&t->list);
    t->delete = friendsFalse;
    t->lc.column = 0;
    t->lc.line = 0;
    t->token_type = 0;
  }
  return t;
}

friendsToken *friendsTokenNext(friendsToken *t)
{
  friendsAssert(t);

  return friendsTokenContainer(friendsListNext(&t->list));
}

friendsToken *friendsTokenPrev(friendsToken *t)
{
  friendsAssert(t);

  return friendsTokenContainer(friendsListPrev(&t->list));
}

static void friendsTokenDeleteElement(friendsToken *t)
{
  friendsAssert(t);

  friendsListDelete(&t->list);
  free(t->token);
  friendsFree(t);
}

void friendsTokenDelete(friendsToken *t)
{
  if (!t) return;

  friendsAssert(t->token);

  friendsTokenDeleteElement(t);
}

void friendsTokenDeleteAll(friendsToken *t)
{
  friendsList *p;
  friendsList *n;
  friendsList *h;
  friendsToken *pp;

  if (!t) return;

  h = &t->list;
  friendsListForeachSafe(p, n, h) {
    pp = friendsTokenContainer(p);
    friendsTokenDeleteElement(pp);
  }
  friendsTokenDeleteElement(t);
}

friendsToken *
friendsTokenAppend(friendsToken *list, int token_type,
                   const friendsChar *start, const friendsChar *end,
                   long line, long column, friendsError *e)
{
  friendsSize sz;
  friendsToken *tok;

  friendsAssert(list);

  tok = (friendsToken *)friendsMalloc(sizeof(friendsToken), e);
  if (!tok) return NULL;

  sz = friendsStringDuplicate(&tok->token, start, end, e);
  if (sz == -1) {
    friendsFree(tok);
    return NULL;
  }

  tok->token_type = token_type;
  tok->lc.line = line;
  tok->lc.column = column;
  tok->delete = friendsFalse;
  friendsListInsertPrev(&list->list, &tok->list);
  return tok;
}

int friendsTokenType(const friendsToken *data)
{
  friendsAssert(data);

  return data->token_type;
}

void friendsSetTokenType(friendsToken *d, int type)
{
  friendsAssert(d);

  d->token_type = type;
}

const friendsChar *friendsTokenText(const friendsToken *data)
{
  friendsAssert(data);

  return data->token;
}

long friendsTokenLinePosition(const friendsToken *data)
{
  friendsAssert(data);

  return data->lc.line;
}

long friendsTokenColumnPosition(const friendsToken *data)
{
  friendsAssert(data);

  return data->lc.column;
}

void friendsTokenMarkDelete(friendsToken *data)
{
  friendsAssert(data);

  data->delete = friendsTrue;
}

friendsBool friendsTokenMarkDeleted(const friendsToken *data)
{
  friendsAssert(data);

  return data->delete;
}
