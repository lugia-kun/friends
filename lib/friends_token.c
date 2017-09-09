
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_data.h"
#include "friends_struct_data.h"
#include "friends_error.h"

void friendsTokenDeleter(void *p)
{
  friendsTokenData *d;
  if (!p) return;

  d = (friendsTokenData *)p;
  free(d->token);
  free(d);
}

friendsData *friendsSetToken(friendsData *data,
                             friendsChar *token_text,
                             int token_type,
                             long line, long column, friendsError *e)
{
  friendsTokenData *d;
  friendsData *dd;

  friendsAssert(data);
  friendsAssert(token_text);

  d = (friendsTokenData *)calloc(sizeof(friendsTokenData), 1);
  if (!d) {
    friendsSetError(e, NOMEM);
    return NULL;
  }

  d->token = token_text;
  d->token_type = token_type;
  d->lc.line = line;
  d->lc.column = column;

  dd = friendsSetData(data, friendsToken, d, friendsTokenDeleter,
                      NULL, token_text, NULL, 0, friendsFalse, e);
  if (!dd) {
    free(d);
    return NULL;
  }
  return data;
}

const friendsTokenData *friendsGetToken(friendsData *data)
{
  if (friendsGetType(data) != friendsToken) return NULL;

  return (const friendsTokenData *)data->data;
}

int friendsTokenType(const friendsTokenData *data)
{
  friendsAssert(data);

  return data->token_type;
}

friendsChar *friendsTokenText(const friendsTokenData *data)
{
  friendsAssert(data);

  return data->token;
}

long friendsTokenLinePosition(const friendsTokenData *data)
{
  friendsAssert(data);

  return data->lc.line;
}

long friendsTokenColumnPosition(const friendsTokenData *data)
{
  friendsAssert(data);

  return data->lc.column;
}
