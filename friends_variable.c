
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_variable.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_core.h"
#include "friends_error.h"

const friendsVariableData *friendsGetVariable(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) != friendsVariable) return NULL;

  return (friendsVariableData *)d->data;
}

static void friendsVariableDeleter(void *p)
{
  friendsVariableData *d;

  if (!p) return;

  d = (friendsVariableData *)p;
  free(d->text);
  friendsFree(d);
}

friendsData *friendsSetVariable(friendsData *data, friendsChar *text,
                                int offset, friendsError *e)
{
  friendsAssert(data);
  friendsAssert(text);
  friendsAssert(offset >= 0);

  if (data->type != friendsInvalidType) {
    friendsSetError(e, ValidType);
    return NULL;
  }

  if (data->data) {
    friendsSetError(e, INVAL);
    return NULL;
  }

  friendsVariableData *d;
  d = friendsMalloc(sizeof(friendsVariableData), e);
  if (!d) {
    return NULL;
  }

  d->text = text;
  d->offset = offset;

  data->data = d;
  data->type = friendsVariable;
  data->hash = friendsHashString(text, NULL);
  data->deleter = friendsVariableDeleter;

  return data;
}

const friendsChar *friendsVariableText(const friendsVariableData *d)
{
  friendsAssert(d);

  return d->text;
}

int friendsVariableOffset(const friendsVariableData *d)
{
  friendsAssert(d);

  return d->offset;
}
