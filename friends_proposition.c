
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_proposition.h"
#include "friends_list.h"
#include "friends_error.h"
#include "friends_argument.h"

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

void friendsSetProposition(friendsData *dest, friendsChar *verb,
                           friendsDataList *arguments,
                           friendsDataList *conditions,
                           friendsBool stop,
                           friendsError *err)
{
  friendsDataList *l;
  friendsPropositionData *d;
  friendsData *dd;
  const friendsPropositionData *pd;
  const friendsArgumentData *ad;

  friendsAssert(dest);
  friendsAssert(verb);

  if (!arguments) {
    friendsSetError(err, NoArgument);
    return;
  }
  if (friendsListSize(arguments) == 0) {
    friendsSetError(err, NoArgument);
    return;
  }

  arguments = friendsListParent(arguments);
  l = friendsListBegin(arguments);
  for (; l; l = friendsListNext(l)) {
    dd = friendsListData(l);
    ad = friendsGetArgument(dd);
    if (!ad) {
      friendsSetError(err, InvalidType);
      return;
    }
  }

  if (conditions) {
    conditions = friendsListParent(conditions);
    l = friendsListBegin(conditions);
    for (; l; l = friendsListNext(l)) {
      dd = friendsListData(l);
      pd = friendsGetProposition(dd);
      if (!pd) {
        friendsSetError(err, InvalidType);
        return;
      }
    }
  }

  d = (friendsPropositionData *)calloc(sizeof(friendsPropositionData), 1);
  if (!d) {
    friendsSetError(err, NOMEM);
    return;
  }

  d->arguments = arguments;
  d->conditions = conditions;
  d->stop = stop;
  d->verb = verb;

  dest->data = d;
  dest->deleter = friendsPropositionDeleter;
  dest->hash = friendsHashString(verb, NULL);
  dest->txt = verb;
  dest->txt_deleter = NULL;
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

  return p->stop;
}

const friendsChar *friendsPropositionVerb(const friendsPropositionData *p)
{
  friendsAssert(p);

  return p->verb;
}
