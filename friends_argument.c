
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_list.h"
#include "friends_error.h"
#include "friends_io.h"

static void friendsArgumentDeleter(void *a)
{
  friendsArgumentData *d;
  d = (friendsArgumentData *)a;

  /* friendsDeleteData(d->variable); */
  free(d->particle);
  free(d);
}

const friendsArgumentData *friendsGetArgument(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) != friendsArgument) return NULL;

  return (friendsArgumentData *)d->data;
}

typedef struct
{
  friendsBool r;
  friendsBool nested;
  friendsError *err;
} friendsArgumentListTestData;

static friendsBool friendsArgumentListTest(friendsData *d, void *a)
{
  friendsArgumentListTestData *p;
  friendsType t;
  friendsDataList *l;

  t = friendsGetType(d);
  p = (friendsArgumentListTestData *)a;

  switch(t) {
  case friendsAtom:
  case friendsVariable:
    break;
  case friendsList:
    if (p->nested == friendsTrue) {
      p->r = friendsFalse;
      friendsSetError(p->err, ListNested);
    } else {
      p->nested = friendsTrue;
      l = friendsGetList(d);
      friendsListEach(l, friendsArgumentListTest, p);
    }
    break;
  default:
    friendsSetError(p->err, InvalidType);
    p->r = friendsFalse;
    break;
  }

  return p->r;
}

void friendsSetArgument(friendsData *dest, friendsData *d,
                        friendsChar *particle, friendsError *err)
{
  friendsArgumentListTestData test_data;
  friendsArgumentData *a;
  friendsChar *ch;
  int x;

  friendsAssert(dest);
  friendsAssert(d);
  friendsAssert(particle);
  friendsAssert(dest != d);

  test_data.err = err;
  test_data.nested = friendsFalse;
  test_data.r = friendsTrue;

  if (friendsArgumentListTest(d, &test_data) == friendsFalse) {
    return;
  }

  a = (friendsArgumentData *)calloc(sizeof(friendsArgumentData *), 1);
  if (!a) {
    friendsSetError(err, NOMEM);
    return;
  }

  if (friendsGetType(d) != friendsList) {
    x = friendsAsprintCF(&ch, err, "%ls %ls",
                         friendsDataToText(d), particle);
  } else {
    x = friendsAsprintCF(&ch, err, "%s %ls",
                         "\\uff08\\u30ea\\u30b9\\u30c8\\uff09" /* （リスト） */,
                         particle);
  }
  if (x < 0) {
    free(a);
    return;
  }

  a->variable = d;
  a->particle = particle;

  dest->type = friendsArgument;
  dest->data = a;
  dest->deleter = friendsArgumentDeleter;
  dest->txt = ch;
  dest->txt_deleter = free;
}

const friendsChar *friendsArgumentParticle(const friendsArgumentData *d)
{
  friendsAssert(d);

  return d->particle;
}

friendsData *friendsArgumentVariable(const friendsArgumentData *d)
{
  friendsAssert(d);

  return d->variable;
}
