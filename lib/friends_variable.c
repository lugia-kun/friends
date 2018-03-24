
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_variable.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_core.h"
#include "friends_error.h"
#include "friends_string.h"
#include "friends_data_private.h"

static
friendsVariableData *friendsGetVariableData(void *p)
{
  return (friendsVariableData *)p;
}

const friendsVariableData *friendsGetVariable(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) != friendsVariable) return NULL;

  return friendsGetVariableData(friends_DataGetTypeData(d));
}

static void *friendsVariableDeleter(void *p)
{
  friendsVariableData *d;

  friendsAssert(p);

  d = friendsGetVariableData(p);
  free(d->text);

  return p;
}

static friendsDataCompareResult
friendsVariableCompare(void *a, void *b)
{
  friendsVariableData *va, *vb;
  int r;

  if (a == b) return friendsDataEqual;

  va = friendsGetVariableData(a);
  vb = friendsGetVariableData(b);

  if (va->tail != vb->tail) {
    return friendsDataNotEqual;
  }

  r = friendsStringCompare(va->text, vb->text);
  if (r == 0) {
    return friendsDataEqual;
  } else {
    return friendsDataNotEqual;
  }
}

static friendsError
friendsVariableTextCreator(friendsChar **output, void *p)
{
  friendsVariableData *data;

  data = friendsGetVariableData(p);
  *output = data->text;
  return friendsNoError;
}

static const friendsDataFunctions friendsVariableFuncs =
{
  .comparator = friendsVariableCompare,
  .deleter = friendsVariableDeleter,
  .txt_creator = friendsVariableTextCreator,
  .txt_deleter = NULL,
};

friendsData *friendsSetVariable(friendsData *dest, const friendsChar *text,
                                friendsBool tail, friendsError *e)
{
  friendsError err;
  friendsVariableData *data;
  friendsChar *buf;
  friendsSize sz;

  friendsAssert(dest);
  friendsAssert(text);

  sz = friendsStringDuplicate(&buf, text, NULL, e);
  if (sz < 0) {
    return NULL;
  }

  err = friends_DataSet(dest, friendsVariable, sizeof(friendsVariableData),
                        &friendsVariableFuncs, friendsHashString(text, NULL));
  if (friendsAnyError(err)) {
    if (e) *e = err;
    free(buf);
    return NULL;
  }

  data = friendsGetVariableData(friends_DataGetTypeData(dest));
  data->text = buf;
  data->tail = tail;

  return dest;
}

const friendsChar *friendsVariableText(const friendsVariableData *d)
{
  friendsAssert(d);

  return d->text;
}
