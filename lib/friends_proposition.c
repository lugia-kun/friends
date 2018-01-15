
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_proposition.h"
#include "friends_error.h"
#include "friends_string.h"

static
friendsPropositionData *friendsGetPropositionData(void *p)
{
  return (friendsPropositionData *)p;
}

const friendsPropositionData *friendsGetProposition(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) != friendsProposition) return NULL;

  return friendsGetPropositionData(friends_DataGetTypeData(d));
}

static void *friendsPropositionDeleter(void *p)
{
  friendsPropositionData *a;

  a = friendsGetPropositionData(p);

  friendsDataListDeleteAll(a->arguments);
  friendsDataListDeleteAll(a->conditions);
  free(a->verb);
  return a;
}

static friendsDataCompareResult
friendsPropositionListCompare(friendsDataList *la, friendsDataList *lb)
{
  friendsDataCompareResult t, u;
  friendsData *da, *db;
  friendsDataList *sla, *slb;

  if (!la || !lb) { return friendsDataNotComparable; }

  la = friendsDataListHead(la);
  lb = friendsDataListHead(lb);
  if (la == lb) { return friendsDataEqual; }
  if (!la || !lb) { return friendsDataNotComparable; }

  if (friendsDataListIsEmpty(la) &&
      friendsDataListIsEmpty(lb)) {
    return friendsDataEqual;
  }

  t = friendsDataEqual;
  la = friendsDataListBegin(la);
  lb = friendsDataListBegin(lb);
  for (; la && lb;
       la = friendsDataListNext(la), lb = friendsDataListNext(lb)) {
    da = friendsDataListGetData(la);
    db = friendsDataListGetData(lb);
    if (da == db) continue;
    u = friendsDataCompare(da, db);
    if (u == friendsDataDifferentType) { t = u; break; }
    if (friendsDataCompareIsNotEqual(u)) {
      if (friendsGetType(da) == friendsListType) {
        sla = friendsGetList(da, NULL);
        slb = friendsGetList(db, NULL);
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
  if (la || lb) {
    return friendsDataNotEqual;
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

static friendsDataCompareResult
friendsPropositionCompareV(void *a, void *b)
{
  return friendsPropositionCompare(friendsGetPropositionData(a),
                                   friendsGetPropositionData(b));
}

static friendsBool friendsPropositionArgsChecker(friendsDataList *arguments,
                                                 friendsError *err,
                                                 int nest)
{
  friendsData *d;
  friendsType t;
  friendsDataList *l;
  friendsBool b;

  arguments = friendsDataListBegin(arguments);
  for (; arguments; arguments = friendsDataListNext(arguments)) {
    d = friendsDataListGetData(arguments);
    if (!d) continue;
    t = friendsGetType(d);
    switch (t) {
    case friendsAtom:
    case friendsVariable:
      break;
    case friendsListType:
      if (nest > 10968) {
        friendsSetError(err, TooNested);
        return friendsFalse;
      }
      l = friendsGetList(d, NULL);
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

const static friendsDataFunctions friendsPropositionFuncs = {
  .comparator = friendsPropositionCompareV,
  .deleter = friendsPropositionDeleter,
  .txt_creator = NULL,
  .txt_deleter = NULL,
};

friendsData *
friendsSetProposition(friendsData *dest, const friendsChar *verb,
                      friendsDataList *arguments, friendsDataList *conditions,
                      friendsPropositionMode mode, friendsError *e)
{
  friendsError err;
  friendsDataList *l;
  friendsPropositionData *d;
  const friendsPropositionData *pd;
  friendsData *dd;
  friendsChar *vv;
  friendsSize n;

  friendsAssert(dest);
  friendsAssert(verb);
  friendsAssert(arguments);

  if (friendsDataListIsEmpty(arguments)) {
    friendsSetError(e, NoArgument);
    return NULL;
  }

  arguments = friendsDataListHead(arguments);
  if (friendsPropositionArgsChecker(arguments, e, 0) == friendsFalse) {
    friendsDataListDeleteAll(arguments);
    return NULL;
  }

  if (conditions) {
    l = friendsDataListBegin(conditions);
    for (; l; l = friendsDataListNext(l)) {
      dd = friendsDataListGetData(l);
      pd = friendsGetProposition(dd);
      if (!pd) {
        friendsSetError(e, InvalidType);
        return NULL;
      }
    }
    conditions = friendsDataListDuplicate(conditions, e);
    if (!conditions) {
      return NULL;
    }
  }

  arguments = friendsDataListDuplicate(arguments, e);
  if (!arguments) {
    if (conditions) {
      friendsDataListDeleteAll(conditions);
    }
    return NULL;
  }

  n = friendsStringDuplicate(&vv, verb, NULL, e);
  if (n == -1) {
    if (conditions) {
      friendsDataListDeleteAll(conditions);
    }
    friendsDataListDeleteAll(arguments);
    return NULL;
  }

  err = friends_DataSet(dest, friendsProposition,
                       sizeof(friendsPropositionData),
                       &friendsPropositionFuncs);
  if (friendsAnyError(err)) {
    if (e) *e = err;
    friendsDataListDeleteAll(conditions);
    friendsDataListDeleteAll(arguments);
    free(vv);
    return NULL;
  }

  d = friendsGetPropositionData(friends_DataGetTypeData(dest));
  d->arguments = arguments;
  d->conditions = conditions;
  d->mode = mode;
  d->verb = vv;

  return dest;
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
