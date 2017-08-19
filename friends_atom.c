
#include <stdlib.h>

#include "friends_atom.h"
#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_error.h"
#include "friends_core.h"
#include "friends_data.h"

static void friendsAtomDeleter(void *p)
{
  friendsAtomData *d;

  if (!p) return;

  d = (friendsAtomData *)p;
  if (d->type == friendsTextAtom) {
    free((void *)d->data.t);
  }

  friendsFree(p);
}

const friendsAtomData *friendsGetAtom(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) == friendsAtom) {
    return (friendsAtomData *)d->data;
  }
  return NULL;
}

friendsData *friendsSetNumeralAtom(friendsData *data, int a, friendsError *e)
{
  friendsAssert(data);
  friendsAssert(data->park);

  if (a < 0) {
    friendsSetError(e, INVAL);
    return NULL;
  }

  if (data->type != friendsInvalidType) {
    friendsSetError(e, ValidType);
    return NULL;
  }

  if (data->data) {
    friendsSetError(e, INVAL);
    return NULL;
  }

  friendsAtomData *d;
  d = friendsMalloc(sizeof(friendsAtomData), e);
  if (!d) {
    return NULL;
  }

  d->data.n = a;
  d->type = friendsNumricAtom;

  data->data = d;
  data->type = friendsAtom;
  data->hash = friendsHashNumeral(a);
  data->deleter = friendsAtomDeleter;
  data->txt = NULL;
  data->txt_deleter = NULL;

  return data;
}

friendsData *friendsSetTextAtom(friendsData *data, friendsChar *text,
                                friendsError *e)
{
  friendsAtomData *d;

  friendsAssert(data);
  friendsAssert(data->park);
  friendsAssert(text);

  if (data->type != friendsInvalidType) {
    friendsSetError(e, ValidType);
    return NULL;
  }

  if (data->data) {
    friendsSetError(e, INVAL);
    return NULL;
  }

  d = friendsMalloc(sizeof(friendsAtomData), e);
  if (!d) {
    return NULL;
  }

  d->data.t = text;
  d->type = friendsTextAtom;

  data->data = d;
  data->type = friendsAtom;
  data->hash = friendsHashString(text, NULL);
  data->deleter = friendsAtomDeleter;
  data->txt = text;
  data->txt_deleter = NULL;

  return data;
}

int friendsGetAtomNumber(const friendsAtomData *a, friendsError *e)
{
  friendsAssert(a);

  if (a->type != friendsNumricAtom) {
    friendsSetError(e, InvalidType);
    return -1;
  }
  return a->data.n;
}

const friendsChar *friendsGetAtomText(const friendsAtomData *a, friendsError *e)
{
  friendsAssert(a);

  if (a->type != friendsTextAtom) {
    friendsSetError(e, InvalidType);
    return NULL;
  }

  return a->data.t;
}
