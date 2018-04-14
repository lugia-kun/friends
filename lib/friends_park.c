
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_park.h"
#include "friends_set.h"
#include "friends_error.h"
#include "friends_data.h"
#include "friends_atom.h"
#include "friends_parser.h"
#include "friends_memory.h"

friendsPark *friendsNewPark(friendsError *e)
{
  friendsPark *p;

  p = (friendsPark *)calloc(sizeof(friendsPark), 1);
  if (!p) {
    friendsSetError(e, NOMEM);
    return NULL;
  }

  p->allocs = friendsNewMemory(e);
  p->atoms   = friendsNewSet(e);
  p->friends = friendsNewSet(e);
  p->deleting = friendsFalse;
  p->parser = NULL;

  if (!(p->atoms && p->friends && p->allocs)) {
    friendsDeletePark(p);
    free(p);
    return NULL;
  }

  return p;
}

void friendsDeletePark(friendsPark *park)
{
  if (!park) return;

  park->deleting = friendsTrue;
  friendsDeleteParser(park->parser);
  friendsDeleteSet(park->atoms);
  friendsDeleteSet(park->friends);
  friendsDeleteMemory(park->allocs);
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
  friendsAssert(park);
  friendsAssert(p);
  friendsAssert(park->allocs);

  p = friendsMemoryAddPointer(park->allocs, p, deleter, e);
  return p;
}

void *friendsDeletePointer(friendsPark *park, void *p)
{
  friendsAssert(park);
  friendsAssert(p);
  friendsAssert(park->allocs);

  p = friendsMemoryDeletePointer(park->allocs, p);
  return p;
}

friendsBool friendsAddDataToPark(friendsPark *park, friendsData *d,
                                 friendsError *e)
{
  friendsError ex;
  friendsDataSet *set;
  friendsType t;
  const friendsChar *ch;
  friendsDataSetNode *node;
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
      node = friendsSetFindNumericAtom(set, n, e);
    } else {
      node = friendsSetFindByText(set, friendsAtom, ch, e);
    }
    /* このアトムはすでに登録されている。 */
    if (node) return friendsTrue;
    if (friendsAnyError(*e)) return friendsFalse;
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
