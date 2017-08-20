
#include "friends_match.h"
#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_core.h"
#include "friends_park.h"
#include "friends_data.h"
#include "friends_error.h"
#include "friends_list.h"
#include "friends_set.h"
#include "friends_proposition.h"
#include "friends_atom.h"
#include "friends_variable.h"

friendsMatchData *friendsGetMatch(friendsData *data)
{
  friendsAssert(data);

  if (friendsGetType(data) != friendsMatch) return NULL;

  return (friendsMatchData *)data->data;
}

static void friendsMatchDeleter(void *p)
{
  friendsMatchData *a;

  if (!p) return;

  friendsFree(p);
}

friendsData *friendsSetMatch(friendsData *dest, friendsMatchData *m,
                             friendsError *err)
{
  friendsType t;

  friendsAssert(dest);
  friendsAssert(m);

  if (m->park != friendsGetPark(dest)) {
    friendsSetError(err, INVAL);
    return NULL;
  }

  return friendsSetData(dest, friendsMatch, m, friendsMatchDeleter, NULL,
                        NULL, NULL, 0, friendsTrue, err);
}

friendsMatchData *friendsNewMatch(friendsPark *park)
{

}

friendsDataList *friendsBuildMatchList(friendsPark *park,
                                       friendsDataList *a,
                                       friendsDataList *b,
                                       friendsError *e)
{
  friendsDataList *pa, *pb;

  pa = friendsListBegin(a);
  pb = friendsListBegin(b);

}

friendsBool friendsPropositionMatch(friendsPark *park,
                                    friendsData *query,
                                    friendsData *target,
                                    friendsDataList **match_list,
                                    friendsError *e)
{
  const friendsPropositionData *qp, *tp;
  friendsDataList *qargs, *lq;
  friendsDataList *targs, *lt;
  friendsDataList *retl;
  friendsMatchData *md;
  friendsData *qd, *td, *dd;
  friendsBool result;
  friendsType qt, tt;

  friendsAssert(park);
  friendsAssert(query);
  friendsAssert(target);
  friendsAssert(match_list);

  qp = friendsGetProposition(query);
  tp = friendsGetProposition(target);
  if (!qp || !tp) {
    friendsSetError(e, InvalidType);
    return friendsFalse;
  }

  qargs = friendsPropositionArguments(query);
  targs = friendsPropositionArguments(target);

  if (!qargs || !targs) return friendsFalse;

  retl = friendsNewList(e);
  if (!retl) return friendsFalse;

  /* ---- goto error if error below here ---- */
  lq = friendsListBegin(qargs);
  lt = friendsListBegin(targs);
  while (1) {
    qd = friendsListData(lq);
    td = friendsListData(lt);
    qt = friendsGetType(qd);
    tt = friendsGetType(td);
    if (qt == friendsEtcetra && tt == friendsEtcetra) {
      dd = friendsNewData(park, e);
      if (!dd) goto error;
    }
  lend:
    if (lq) lq = friendsListNext(lq);
    if (lt) lt = friendsListNext(lt);
    if (!lq && !lt) break;
  }

  return result;

 error:
  friendsDeleteList(retl);
  return friendsFalse;
}

friendsBool friendsQuery(friendsPark *park,
                         friendsDataList **match_list,
                         const friendsPropositionData *query,
                         friendsError *e)
{
  friendsDataList *l;
  friendsDataList *list;
  friendsDataList *conds;
  friendsDataList *props;
  friendsData *dt;
  friendsBool result;
  friendsError err;
  const friendsChar *verb;

  friendsAssert(query);
  friendsAssert(park);

  if (!e) {
    err = friendsNoError;
    e = &err;
  }

  result = friendsTrue;
  list = friendsNewList(e);
  if (!list) return friendsFalse;

  verb = friendsPropositionVerb(query);
  props = friendsPropositionListInPark(park, verb);
  if (friendsListSize(props) == 0) {
    result = friendsFalse;
    goto end;
  }

  l = friendsListBegin(props);
  for (; l; l = friendsListNext(l)) {

  }

 end:
  if (match_list) {
    *match_list = list;
  } else {
    friendsDeleteList(list);
  }
  return result;

 error:
  friendsDeleteList(list);
  return friendsFalse;
}
