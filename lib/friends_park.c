
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_park.h"
#include "friends_set.h"
#include "friends_error.h"
#include "friends_list.h"
#include "friends_data.h"
#include "friends_atom.h"
#include "friends_parser.h"

friendsPark *friendsNewPark(friendsError *e)
{
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
  p->parser = NULL;

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
  friendsDeleteParser(park->parser);
  free(park);
}

friendsBool friendsParkIsDeleting(friendsPark *park)
{
  friendsAssert(park);

  return park->deleting;
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

friendsBool friendsAddDataToPark(friendsPark *park, friendsData *d,
                                 friendsError *e)
{
  friendsError ex;
  friendsDataSet *set;
  friendsType t;
  const friendsChar *ch;
  friendsDataList *ll;
  const friendsAtomData *at;
  int n;

  friendsAssert(park);
  friendsAssert(d);

  friendsAssert(friendsGetPark(d) == park);

  if (!e) {
    ex = friendsNoError;
    e = &ex;
  }

  t = friendsGetType(d);
  ch = friendsDataToText(d);

  switch(t) {
  case friendsAtom:
    set = park->atoms;
    if (!ch) {
      at = friendsGetAtom(d);
      n = friendsGetAtomNumber(at, e);
      if (friendsAnyError(*e)) {
        return friendsFalse;
      }
      ll = friendsSetFindNumericAtom(set, n);
    } else {
      ll = friendsSetFindText(set, t, ch);
    }
    /* このアトムはすでに登録されている。 */
    if (ll && friendsListSize(ll) > 0) return friendsTrue;
    break;
  case friendsProposition:
    set = park->friends;
    break;
  default:
    friendsSetError(e, InvalidType);
    return friendsFalse;
  }

  friendsSetInsert(set, d, e);
  if (friendsAnyError(*e)) {
    return friendsFalse;
  }

  return friendsTrue;
}


friendsDataList *friendsPropositionListInPark(friendsPark *park,
                                              const friendsChar *verb)
{
  friendsAssert(park);
  friendsAssert(verb);

  return friendsSetFindText(park->friends, friendsProposition, verb);
}


friendsParser *friendsGetParser(friendsPark *park, friendsError *err)
{
  friendsAssert(park);

  if (park->parser) return park->parser;

  park->parser = friendsNewParser(park, err);
  return park->parser;
}

friendsParser *friendsResetParser(friendsPark *park, friendsError *err)
{
  friendsAssert(park);

  if (!park->parser) return NULL;

  friendsDeleteParser(park->parser);
  park->parser = friendsNewParser(park, err);
  return park->parser;
}
