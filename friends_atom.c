
#include <stdlib.h>

#include "friends_atom.h"
#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_error.h"
#include "friends_core.h"
#include "friends_data.h"
#include "friends_string.h"

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

friendsDataCompareResult friendsAtomCompare(const friendsAtomData *a,
                                            const friendsAtomData *b)
{
  int r;

  if (!a || !b) return friendsDataNotEqual;
  if (a->type != b->type) return friendsDataDifferentType;

  if (a->type == friendsNextAtom) {
    return friendsDataEqual;
  } else if (a->type == friendsTextAtom) {
    r = friendsStringCompare(a->data.t, b->data.t);
    if (r == 0) {
      return friendsDataEqual;
    } else if (r > 0) {
      return friendsDataGreater;
    } else {
      return friendsDataLess;
    }
  } else {
    if (a->data.n == b->data.n) {
      return friendsDataEqual;
    } else if (a->data.n > b->data.n) {
      return friendsDataGreater;
    } else {
      return friendsDataLess;
    }
  }
}

static friendsDataCompareResult friendsAtomCompareV(const void *a,
                                                    const void *b)
{
  return friendsAtomCompare((const friendsAtomData *)a,
                            (const friendsAtomData *)b);
}

const friendsAtomData *friendsGetAtom(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) == friendsAtom) {
    return (friendsAtomData *)d->data;
  }
  return NULL;
}

friendsData *friendsSetNumeralAtom(friendsData *dest, int a, friendsError *e)
{
  friendsAtomData *d;

  friendsAssert(dest);

  if (a < 0) {
    friendsSetError(e, INVAL);
    return NULL;
  }

  d = friendsMalloc(sizeof(friendsAtomData), e);
  if (!d) {
    return NULL;
  }

  d->data.n = a;
  d->type = friendsNumericAtom;

  if (!friendsSetData(dest, friendsAtom, d, friendsAtomDeleter,
                      friendsAtomCompareV, NULL, NULL,
                      friendsHashNumeral(a),
                      friendsFalse, e)) {
    free(d);
    return NULL;
  }

  return dest;
}

friendsData *friendsSetTextAtom(friendsData *dest, friendsChar *text,
                                friendsError *e)
{
  friendsAtomData *d;

  friendsAssert(dest);
  friendsAssert(text);

  d = friendsMalloc(sizeof(friendsAtomData), e);
  if (!d) {
    return NULL;
  }

  d->data.t = text;
  d->type = friendsTextAtom;

  if (!friendsSetData(dest, friendsAtom, d, friendsAtomDeleter,
                      friendsAtomCompareV, text, NULL,
                      friendsHashString(text, NULL),
                      friendsFalse, e)) {
    free(d);
    return NULL;
  }

  return dest;
}

friendsData *friendsSetNextAtom(friendsData *dest, friendsError *e)
{
  friendsChar *ch;
  friendsAtomData *d;
  int r;

  friendsAssert(dest);

  d = friendsMalloc(sizeof(friendsAtomData), e);
  if (!d) {
    return NULL;
  }

  r = friendsUnescapeStringLiteral(&ch, "\\u6b21" /* 次 */, e);
  if (r < 0) {
    free(d);
    return NULL;
  }

  d->data.t = NULL;
  d->type = friendsNextAtom;

  if (!friendsSetData(dest, friendsAtom, d, friendsAtomDeleter,
                      friendsAtomCompareV, ch, free,
                      friendsHashValueNextAtom, friendsFalse, e)) {
    free(d);
    free(ch);
    return NULL;
  }

  return dest;
}

friendsAtomType friendsGetAtomType(const friendsAtomData *a)
{
  friendsAssert(a);

  return a->type;
}

int friendsGetAtomNumber(const friendsAtomData *a, friendsError *e)
{
  friendsAssert(a);

  if (a->type != friendsNumericAtom) {
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
