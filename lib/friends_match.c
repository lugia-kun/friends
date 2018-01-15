
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

#if 0
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
#endif
