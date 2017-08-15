
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_park.h"
#include "friends_set.h"
#include "friends_error.h"
#include "friends_list.h"

friendsPark *friendsNewPark(friendsError *e)
{
  int i;
  friendsPark *p;

  p = (friendsPark *)calloc(sizeof(friendsPark), 1);
  if (!p) {
    friendsSetError(e, NOMEM);
    return NULL;
  }

  p->alloc_table = friendsNewList(e);
  p->atoms   = friendsNewSet(e);
  p->friends = friendsNewSet(e);
  p->deleting = friendsFalse;

  if (!(p->atoms && p->friends)) {
    friendsDeleteSet(p->atoms);
    friendsDeleteSet(p->friends);
    free(p);
    return NULL;
  }

  return p;
}

void friendsDeletePark(friendsPark *park)
{
  friendsDataList *l;
  friendsMemory *n;

  if (!park) return;

  park->deleting = friendsTrue;

  l = park->alloc_table;
  for (; l; l = friendsListNext(l)) {
    n = (friendsMemory *)friendsListData(l);
    if (n) {
      if (n->del) n->del(n->p);
    }
    free(n);
  }

  friendsDeleteList(park->alloc_table);
  friendsDeleteSet(park->atoms);
  friendsDeleteSet(park->friends);
  free(park);
}

void *friendsAddPointer(friendsPark *park, void *p,
                        friendsPointerDeleter *deleter,
                        friendsError *e)
{
  friendsError ee;
  friendsMemory *m;

  friendsAssert(park);
  friendsAssert(p);
  friendsAssert(deleter);
  friendsAssert(park->alloc_table);

  if (!e) {
    ee = friendsNoError;
    e = &ee;
  }

  m = (friendsMemory *)calloc(sizeof(friendsMemory), 1);
  if (!m) {
    friendsSetError(e, NOMEM);
    return NULL;
  }

  m->p = p;
  m->del = deleter;

  friendsListAppend(park->alloc_table, (friendsData *)m, e);
  if (friendsAnyError(*e)) {
    free(m);
    return NULL;
  }

  return p;
}

static friendsBool friendsMemoryFinder(friendsData *d, void *p)
{
  friendsMemory *m;
  m = (friendsMemory *)d;

  if (m->p == p) {
    return friendsTrue;
  }
  return friendsFalse;
}

void friendsDeletePointer(friendsPark *park, void *p)
{
  friendsDataList *l;
  friendsMemory *m;

  if (!park) return;
  if (!p) return;

  friendsAssert(park->alloc_table);

  if (park->deleting == friendsTrue) return;

  l = friendsListFindIf(park->alloc_table, friendsMemoryFinder, p);
  if (!l) return;

  m = (friendsMemory *)friendsListData(l);

  friendsListRemove(l);
  free(m);
}
