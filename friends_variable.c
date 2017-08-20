
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_variable.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_core.h"
#include "friends_error.h"
#include "friends_string.h"

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

static friendsDataCompareResult friendsVariableCompare(const void *a,
                                                       const void *b)
{
  const friendsVariableData *va, *vb;
  int r;

  if (a == b) return friendsDataEqual;

  va = (const friendsVariableData *)a;
  vb = (const friendsVariableData *)b;

  if (va->tail != vb->tail) {
    return friendsDataNotEqual;
  }

  if (va->offset != vb->offset) {
    return friendsDataNotEqual;
  }

  r = friendsStringCompare(va->text, vb->text);
  if (r == 0) {
    return friendsDataEqual;
  } else {
    return friendsDataNotEqual;
  }
}


friendsData *friendsSetVariable(friendsData *dest, friendsChar *text,
                                int offset, friendsBool tail, friendsError *e)
{
  friendsVariableData *d;

  friendsAssert(dest);
  friendsAssert(text);

  if (tail == friendsTrue) {
    friendsAssert(offset == 0);
  } else {
    friendsAssert(offset >= 0);
  }

  d = friendsMalloc(sizeof(friendsVariableData), e);
  if (!d) {
    return NULL;
  }

  d->text = text;
  d->offset = offset;
  d->tail = tail;

  if (!friendsSetData(dest, friendsVariable, d, friendsVariableDeleter,
                      friendsVariableCompare, text, NULL,
                      friendsHashString(text, NULL), friendsFalse, e)) {
    free(d);
    return NULL;
  }

  return dest;
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
