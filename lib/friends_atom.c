
#include <stdlib.h>

#include "friends_atom.h"
#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_error.h"
#include "friends_core.h"
#include "friends_data.h"
#include "friends_string.h"
#include "friends_io.h"

static friendsAtomData *friendsGetAtomData(void *p)
{
  friendsAssert(p);

  return (friendsAtomData *)p;
}

static void *friendsAtomDeleter(void *p)
{
  friendsAtomData *d;

  friendsAssert(p);

  d = friendsGetAtomData(p);
  if (d->type == friendsTextAtom) {
    free(d->data.t);
  }
  return p;
}

friendsDataCompareResult
friendsAtomCompare(const friendsAtomData *a, const friendsAtomData *b)
{
  int r;

  friendsAssert(a);

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

static friendsDataCompareResult
friendsAtomCompareV(void *a, void *b)
{
  return friendsAtomCompare(friendsGetAtomData(a), friendsGetAtomData(b));
}

const friendsAtomData *
friendsGetAtom(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) == friendsAtom) {
    return friendsGetAtomData(friends_DataGetTypeData(d));
  }
  return NULL;
}

static friendsError
friendsAtomTextCreator(friendsChar **output, void *p)
{
  friendsError err;
  friendsAtomData *d;
  d = friendsGetAtomData(p);

  err = friendsNoError;
  switch(d->type) {
  case friendsNumericAtom:
    friendsAsprintCF(output, &err, "%d", d->data.n);
    break;
  case friendsNextAtom:
    friendsUnescapeStringLiteral(output, "\\u6b21" /* 次 */, &err);
    break;
  case friendsTextAtom:
    *output = d->data.t;
    break;
  default:
    friendsUnreachable();
    break;
  }
  return err;
}

static void
friendsAtomTextDeleter(friendsChar *text, void *p)
{
  friendsAtomData *d;
  d = friendsGetAtomData(p);

  switch(d->type) {
  case friendsNumericAtom:
  case friendsNextAtom:
    free(text);
    break;
  default:
    break;
  }
}

static const friendsDataFunctions friendsAtomFuncs = {
  .deleter     = friendsAtomDeleter,
  .txt_creator = friendsAtomTextCreator,
  .txt_deleter = friendsAtomTextDeleter,
  .comparator  = friendsAtomCompareV,
};

friendsData *
friendsSetNumeralAtom(friendsData *dest, int a, friendsError *e)
{
  friendsError err;
  friendsAtomData *data;

  friendsAssert(dest);

  if (a < 0) {
    friendsSetError(e, INVAL);
    return NULL;
  }

  err = friends_DataSet(dest, friendsAtom, sizeof(friendsAtomData),
                        &friendsAtomFuncs);
  if (friendsAnyError(err)) {
    if (e) *e = err;
    return NULL;
  }
  data = friendsGetAtomData(friends_DataGetTypeData(dest));

  data->data.n = a;
  data->type = friendsNumericAtom;

  return dest;
}

friendsData *friendsSetTextAtom(friendsData *dest, const friendsChar *text,
                                friendsError *e)
{
  friendsError err;
  friendsAtomData *data;
  friendsChar *buf;
  friendsSize sz;
  friendsHash hsh;

  friendsAssert(dest);
  friendsAssert(text);

  sz = friendsStringDuplicate(&buf, text, NULL, e);
  if (sz < 0) {
    return NULL;
  }

  err = friends_DataSet(dest, friendsAtom, sizeof(friendsAtomData),
                        &friendsAtomFuncs);
  if (friendsAnyError(err)) {
    if (e) *e = err;
    free(buf);
    return NULL;
  }

  data = friendsGetAtomData(friends_DataGetTypeData(dest));
  data->type = friendsTextAtom;
  data->data.t = buf;

  return dest;
}

friendsData *friendsSetNextAtom(friendsData *dest, friendsError *e)
{
  friendsError err;
  friendsAtomData *data;

  friendsAssert(dest);

  err = friends_DataSet(dest, friendsAtom, sizeof(friendsAtomData),
                        &friendsAtomFuncs);

  data = friendsGetAtomData(friends_DataGetTypeData(dest));
  data->data.t = NULL;
  data->type = friendsNextAtom;

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
