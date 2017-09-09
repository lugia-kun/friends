
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

friendsData *friendsSetData(friendsData *dest, friendsType type,
                            void *data, friendsPointerDeleter *data_deleter,
                            friendsDataCompareFunc *comp_func,
                            friendsChar *text,
                            friendsPointerDeleter *text_deleter,
                            friendsHash hash,
                            friendsBool allow_replace,
                            friendsError *err)
{
  friendsAssert(dest);
  friendsAssert(dest->park);

  if (type <= friendsInvalidType || type >= friendsLastType) {
    friendsSetError(err, InvalidType);
    return NULL;
  }
  if (dest->type != friendsInvalidType) {
    if (allow_replace == friendsTrue && type != dest->type) {
      friendsSetError(err, ValidType);
      return NULL;
    }
    if (dest->data != data) {
      if (dest->deleter) {
        dest->deleter(dest->data);
      }
      if (dest->txt_deleter) {
        dest->txt_deleter(dest->txt);
      }
    }
  }

  dest->type = type;
  dest->data = data;
  dest->deleter = data_deleter;
  dest->comp_func = comp_func;
  dest->hash = hash;
  dest->txt = text;
  dest->txt_deleter = text_deleter;

  return dest;
}

const friendsChar *friendsDataToText(friendsData *d)
{
  if (!d) return NULL;

  return d->txt;
}

friendsDataCompareResult friendsDataCompare(const friendsData *a,
                                            const friendsData *b)
{
  friendsDataCompareResult t;
  friendsDataCompareFunc *cfa;

  t = 0;

  if (a->park != b->park) {
    t |= friendsDataInAnotherPark;
  }

  if (a->type != b->type) {
    t |= friendsDataDifferentType;
    return t;
  }

  cfa = a->comp_func;
  if (!cfa) {
    cfa = b->comp_func;
    if (!cfa) {
      t |= friendsDataNotComparable;
      return t;
    }
  }

  t |= cfa(a->data, b->data);
  return t;
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

int friendsDataCompareIsEqual(friendsDataCompareResult x)
{
  return (x == friendsDataEqual);
}

int friendsDataCompareIsNotEqual(friendsDataCompareResult x)
{
  return (x != friendsDataEqual);
}

int friendsDataCompareIsSetEqual(friendsDataCompareResult x)
{
  return friendsDataCompareIsEqual(x) || ((x & friendsDataSetEqual) > 0);
}
