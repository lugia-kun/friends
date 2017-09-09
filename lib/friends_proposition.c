
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_proposition.h"
#include "friends_list.h"
#include "friends_error.h"
#include "friends_string.h"

const friendsPropositionData *friendsGetProposition(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) != friendsProposition) return NULL;

  return (friendsPropositionData *)d->data;
}

static void friendsPropositionDeleter(void *p)
{
  friendsPropositionData *a;

  if (!p) return;

  a = (friendsPropositionData *)p;

  friendsDeleteList(a->arguments);
  friendsDeleteList(a->conditions);
  free(a->verb);
  free(a);
}

static friendsDataCompareResult
friendsPropositionListCompare(friendsDataList *la,
                              friendsDataList *lb)
{
  friendsDataCompareResult t, u;
  friendsData *da, *db;
  friendsDataList *sla, *slb;

  if (!la || !lb) { return friendsDataNotComparable; }

  la = friendsListParent(la);
  lb = friendsListParent(lb);
  if (la == lb) { return friendsDataEqual; }
  if (!la || !lb) { return friendsDataNotComparable; }

  if (friendsListSize(la) != friendsListSize(lb)) {
    return friendsDataNotEqual;
  }

  if (friendsListSize(la) == 0) {
    return friendsDataEqual;
  }

  t = friendsDataEqual;
  la = friendsListBegin(la);
  lb = friendsListBegin(lb);
  for (; la && lb; la = friendsListNext(la), lb = friendsListNext(lb)) {
    da = friendsListData(la);
    db = friendsListData(lb);
    if (da == db) continue;
    u = friendsDataCompare(da, db);
    if (u == friendsDataDifferentType) { t = u; break; }
    if (friendsDataCompareIsNotEqual(u)) {
      if (friendsGetType(da) == friendsList) {
        sla = friendsGetList(da);
        slb = friendsGetList(db);
        u = friendsPropositionListCompare(sla, slb);
        if (friendsDataCompareIsNotEqual(u)) {
          t = u;
          break;
        }
      } else {
        t = u;
        break;
      }
    }
  }
  return t;
}

friendsDataCompareResult
friendsPropositionCompare(const friendsPropositionData *a,
                          const friendsPropositionData *b)
{
  friendsDataCompareResult ret, tmp;

  if (!a || !b) return friendsDataNotComparable;
  if (a == b) return friendsDataEqual;

  ret = 0;
  if (friendsStringCompare(a->verb, b->verb) == 0) {
    ret = friendsDataSetEqual;
  } else {
    return friendsDataNotEqual;
  }

  tmp = friendsDataEqual;
  if (a->arguments != b->arguments) {
    tmp = friendsPropositionListCompare(a->arguments, b->arguments);
  }

  if (friendsDataCompareIsEqual(tmp)) {
    if (a->conditions != b->conditions) {
      tmp = friendsPropositionListCompare(a->conditions, b->conditions);
    }
  }

  if (friendsDataCompareIsEqual(tmp)) {
    return tmp;
  } else {
    return ret | tmp | friendsDataNotEqual;
  }
}

static friendsDataCompareResult friendsPropositionCompareV(const void *a,
                                                           const void *b)
{
  if (!a || !b) return friendsDataNotComparable;

  return friendsPropositionCompare((const friendsPropositionData *)a,
                                   (const friendsPropositionData *)b);
}

static friendsBool friendsPropositionArgsChecker(friendsDataList *arguments,
                                                 friendsError *err,
                                                 int nest)
{
  friendsData *d;
  friendsType t;
  friendsDataList *l;
  friendsBool b;

  arguments = friendsListBegin(arguments);
  for (; arguments; arguments = friendsListNext(arguments)) {
    d = friendsListData(arguments);
    if (!d) continue;
    t = friendsGetType(d);
    switch (t) {
    case friendsAtom:
    case friendsVariable:
      break;
    case friendsList:
      if (nest > 10968) {
        friendsSetError(err, TooNested);
        return friendsFalse;
      }
      l = friendsGetList(d);
      b = friendsPropositionArgsChecker(l, err, nest + 1);
      if (b == friendsFalse) return b;
      break;
    default:
      friendsSetError(err, InvalidType);
      return friendsFalse;
    }
  }
  return friendsTrue;
}

friendsData *friendsSetProposition(friendsData *dest, friendsChar *verb,
                                   friendsDataList *arguments,
                                   friendsDataList *conditions,
                                   friendsPropositionMode mode,
                                   friendsError *err)
{
  friendsDataList *l;
  friendsPropositionData *d;
  friendsData *dd;
  const friendsPropositionData *pd;

  friendsAssert(dest);
  friendsAssert(verb);

  if (!arguments) {
    friendsSetError(err, NoArgument);
    return NULL;
  }
  if (friendsListSize(arguments) == 0) {
    friendsSetError(err, NoArgument);
    return NULL;
  }

  arguments = friendsListParent(arguments);
  if (friendsPropositionArgsChecker(arguments, err, 0) == friendsFalse)
    return NULL;

  if (conditions) {
    conditions = friendsListParent(conditions);
    l = friendsListBegin(conditions);
    for (; l; l = friendsListNext(l)) {
      dd = friendsListData(l);
      pd = friendsGetProposition(dd);
      if (!pd) {
        friendsSetError(err, InvalidType);
        return NULL;
      }
    }
  }

  d = (friendsPropositionData *)calloc(sizeof(friendsPropositionData), 1);
  if (!d) {
    friendsSetError(err, NOMEM);
    return NULL;
  }

  d->arguments = arguments;
  d->conditions = conditions;
  d->mode = mode;
  d->verb = verb;

  dd = friendsSetData(dest, friendsProposition, d,
                      friendsPropositionDeleter,
                      NULL, verb, NULL,
                      friendsHashString(verb, NULL),
                      friendsFalse, err);
  if (!dd) {
    free(d);
    return NULL;
  }
  return dd;
}

friendsDataList *friendsPropositionArguments(const friendsPropositionData *p)
{
  friendsAssert(p);

  return p->arguments;
}

friendsDataList *friendsPropositionConditions(const friendsPropositionData *p)
{
  friendsAssert(p);

  return p->conditions;
}

friendsBool friendsPropositionIsStop(const friendsPropositionData *p)
{
  friendsAssert(p);

  if (p->mode == friendsPropositionStop) {
    return friendsTrue;
  } else {
    return friendsFalse;
  }
}

friendsBool friendsPropositionIsQuery(const friendsPropositionData *p)
{
  friendsAssert(p);

  if (p->mode == friendsPropositionQuery) {
    return friendsTrue;
  } else {
    return friendsFalse;
  }
}

const friendsChar *friendsPropositionVerb(const friendsPropositionData *p)
{
  friendsAssert(p);

  return p->verb;
}
