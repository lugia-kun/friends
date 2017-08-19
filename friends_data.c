
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>

#include "friends_park.h"
#include "friends_data.h"
#include "friends_struct_data.h"
#include "friends_error.h"
#include "friends_core.h"
#include "friends_string.h"
#include "friends_list.h"
#include "friends_set.h"

friendsData *friendsNewData(friendsPark *park, friendsError *e)
{
  friendsData *p;
  if (!park) {
    friendsSetError(e, INVAL);
    return NULL;
  }

  p = (friendsData *)friendsMalloc(sizeof(friendsData), e);
  if (!p) {
    return NULL;
  }

  if (friendsAddPointer(park, p,
                        (friendsPointerDeleter *)friendsDeleteData,
                        e) == NULL) {
    free(p);
    return NULL;
  }

  p->data = NULL;
  p->type = friendsInvalidType;
  p->hash = 0;
  p->park = park;
  p->deleter = NULL;
  p->txt = NULL;
  p->txt_deleter = NULL;
  return p;
}

void friendsDeleteData(friendsData *d)
{
  if (!d) return;

  friendsDeletePointer(d->park, d);

  if (d->deleter)     d->deleter(d->data);
  if (d->txt_deleter) d->txt_deleter(d->txt);
  free(d);
}

friendsPark *friendsGetPark(friendsData *d)
{
  if (!d) return NULL;
  return d->park;
}

friendsType friendsGetType(friendsData *d)
{
  if (!d) return friendsInvalidType;
  return d->type;
}

friendsHash friendsGetHash(friendsData *d)
{
  if (!d) return 0;
  return d->hash;
}

const friendsVariableData *friendsGetVariable(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) != friendsVariable) return NULL;

  return (friendsVariableData *)d->data;
}

const friendsChar *friendsDataToText(friendsData *d)
{
  if (!d) return NULL;

  return d->txt;
}

friendsData *friendsSetVariable(friendsData *data, const friendsChar *text,
                                int offset, friendsError *e)
{
  friendsAssert(data);
  friendsAssert(data->park);
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

  return data;
}

static friendsHash friendsTopNbit(friendsHash h, unsigned int n)
{
  return h << (sizeof(friendsHash) * CHAR_BIT - n);
}

static friendsHash friendsHasher(const char *s, const char *e)
{
  int off;
  const char *p;
  friendsHash h = 0x1afe21c5;
  friendsHash f = h;
  size_t n = sizeof(friendsHash) * CHAR_BIT;

  for (p = s; p < e; ++p) {
    off = ((p - s) * 5) % n;
    f = *p * h;
    h = h ^ ((f << off) | (f >> off));
  }
  return h;
}

friendsHash friendsHashNumeral(int a)
{
  char *s, *e;
  friendsHash h, t;

  h = (friendsHash) a;
  h = h << 4;
  h = h >> 4;

  if (h == a) {
    h = friendsTopNbit(0x1, 4);
    return h | a;
  }

  s = (char *)  &a;
  e = (char *) (&a + 1);
  h = friendsHasher(s, e);
  t = friendsTopNbit(0x1, 3);
  h = h & (t - 1);
  return h | t;
}

friendsHash friendsHashString(const friendsChar *s, const friendsChar *e)
{
  friendsHash h, t;

  friendsAssert(s);

  if (!e) e = s + friendsStringArrayLength(s);

  h = friendsHasher((const char *)s, (const char *)e);
  t = friendsTopNbit(0x1, 1);
  return h | t;
}

friendsHash friendsHashBinary(const void *s, const void *e)
{
  friendsHash h, t;

  friendsAssert(s);
  friendsAssert(e);

  h = friendsHasher(s, e);
  t = friendsTopNbit(0x1, 2);
  h = h & (t - 1);
  return h | t;
}
