
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_error.h"
#include "friends_set.h"
#include "friends_struct_data.h"
#include "friends_list.h"
#include "friends_core.h"
#include "friends_data.h"
#include "friends_set.h"
#include "friends_string.h"
#include "friends_atom.h"

enum {
  friendsCharMax     = UCHAR_MAX,
  friendsDataSetSize = friendsCharMax + 1,
};

friendsDataSetHash friendsSmallHash(friendsHash h)
{
  friendsDataSetHash o = 0xc6;
  int i;
  enum {
    ASIZE = ((sizeof(friendsHash) - 1) / sizeof(friendsDataSetHash) + 1)
  };
  union {
    friendsHash h;
    friendsDataSetHash d[ASIZE];
  } t;

  memset(t.d, 0, sizeof(friendsDataSetHash) * ASIZE);
  t.h = h;
  for (i = 0; i < ASIZE; ++i) {
    o = o ^ (t.d[i] * 11 * (i + 1) + 6);
  }
  return o;
}

friendsDataSet *friendsNewSet(friendsError *e)
{
  friendsDataSet *set;
  friendsDataSetNode **table;

  set = (friendsDataSet *)calloc(sizeof(friendsDataSet), 1);
  if (!set) {
    friendsSetError(e, NOMEM);
    return NULL;
  }

  table = (friendsDataSetNode **)calloc(sizeof(friendsDataSetNode *),
                                        friendsDataSetSize);
  if (!table) {
    friendsSetError(e, NOMEM);
    free(set);
    return NULL;
  }

  set->table = table;
  return set;
}

void friendsDeleteSet(friendsDataSet *set)
{
  friendsDataSetNode *n, *p;
  int i;

  if (!set) return;

  if (set->table) {
    for (i = 0; i < friendsDataSetSize; ++i) {
      n = set->table[i];
      while (n) {
        if (n->list) friendsDeleteList(n->list);
        p = n->next;
        free(n);
        n = p;
      }
    }
    free(set->table);
  }

  free(set);
}

static friendsData *friendsSetGetData(friendsDataSetNode *n,
                                      friendsDataList **l)
{
  friendsDataList *ll;

  if (!n) return NULL;
  ll = n->list;
  if (!ll) return NULL;
  if (l) *l = ll;
  if (friendsListIsParent(ll)) ll = friendsListNext(ll);
  return friendsListData(ll);
}

friendsBool  friendsSetContains(friendsDataSet *set, friendsData *d)
{
  friendsDataSetHash h;
  friendsDataSetNode *n;
  friendsDataList *l;
  friendsData *c;

  friendsAssert(set);
  friendsAssert(set->table);
  friendsAssert(d);

  h = friendsSmallHash(d->hash);

  n = set->table[h];
  if (!n) return friendsFalse;

  for (; n; n = n->next) {
    c = friendsSetGetData(n, &l);
    if (!c) continue;
    if (c == d) return friendsTrue;
    if (c->hash != d->hash) continue;
    l = friendsListNext(l);
    for (; l; l = friendsListNext(l)) {
      c = friendsListData(l);
      if (c == d) return friendsTrue;
    }
  }
  return friendsFalse;
}

friendsDataList *friendsSetFindText(friendsDataSet *set,
                                    friendsType type, const friendsChar *text)
{
  friendsDataSetNode *n;
  friendsDataSetHash h;
  friendsDataList *l;
  friendsData *c;
  friendsHash hsh;
  const friendsChar *tx;

  friendsAssert(set);
  friendsAssert(set->table);
  friendsAssert(text);

  hsh = friendsHashString(text, NULL);
  h = friendsSmallHash(hsh);

  n = set->table[h];
  if (!n) return NULL;

  for (; n; n = n->next) {
    c = friendsSetGetData(n, &l);
    if (!c) continue;
    if (c->hash != hsh) continue;
    if (c->type != type) continue;
    tx = friendsDataToText(c);
    if (!tx) continue;
    if (friendsStringCompare(tx, text) == 0) {
      return l;
    }
  }
  return NULL;
}

friendsDataList *friendsSetFindNumericAtom(friendsDataSet *set, int a)
{
  friendsDataSetNode *n;
  friendsDataSetHash h;
  friendsDataList *l;
  friendsData *c;
  const friendsAtomData *at;
  friendsHash hsh;
  friendsError e;
  int r;

  friendsAssert(set);
  friendsAssert(set->table);

  hsh = friendsHashNumeral(a);
  h = friendsSmallHash(hsh);

  n = set->table[h];
  if (!n) return NULL;

  for (; n; n = n->next) {
    c = friendsSetGetData(n, &l);
    if (!c) continue;
    at = friendsGetAtom(c);
    if (!at) continue;
    e = friendsNoError;
    r = friendsGetAtomNumber(at, &e);
    if (friendsAnyError(e)) continue;
    if (r == a) return l;
  }
  return NULL;
}

void friendsSetInsert(friendsDataSet *set, friendsData *d, friendsError *e)
{
  friendsHash hsh;
  friendsDataSetNode *n;
  friendsDataSetHash h;
  friendsDataList *l;
  friendsData *c;
  const friendsChar *ct, *dt;
  const friendsAtomData *ca, *da;
  int ci, di;
  friendsError ce, de;

  friendsAssert(d);
  friendsAssert(set);
  friendsAssert(set->table);

  hsh = friendsGetHash(d);
  h = friendsSmallHash(hsh);
  n = set->table[h];
  for (; n; n = n->next) {
    c = friendsSetGetData(n, &l);
    if (!c) continue;
    if (friendsGetHash(c) != hsh) continue;
    if (friendsGetType(c) != friendsGetType(d)) continue;
    ct = friendsDataToText(c);
    dt = friendsDataToText(d);
    if (ct && dt) {
      if (ct == dt) break;
      if (friendsStringCompare(ct, dt) == 0) break;
    } else {
      ca = friendsGetAtom(c);
      da = friendsGetAtom(d);
      if (ca && ca == da) break;
      if (ca && da) {
        ce = de = friendsNoError;
        ci = friendsGetAtomNumber(ca, &ce);
        di = friendsGetAtomNumber(da, &de);
        if (friendsAnyError(ce) || friendsAnyError(de)) {
        } else if (ci == di) {
          break;
        }
      }
    }
  }
  if (!n) {
    n = (friendsDataSetNode *)calloc(sizeof(friendsDataSetNode), 1);
    if (!n) {
      friendsSetError(e, NOMEM);
      return;
    }
    l = friendsNewList(e);
    if (!l) {
      free(n);
      return;
    }
    n->list = l;
    n->root = set;
    n->next = set->table[h];
    set->table[h] = n;
  }
  friendsAssert(l);
  friendsListAppend(l, d, e);
}

void friendsSetRemove(friendsDataSet *set, friendsData *d, friendsError *e)
{
  friendsDataList *l;
  friendsHash hsh;
  friendsDataSetHash h;
  friendsDataSetNode *n;
  friendsData *c;

  friendsAssert(set);
  friendsAssert(d);

  hsh = friendsGetHash(d);
  h = friendsSmallHash(hsh);
  n = set->table[h];
  for (; n; n = n->next) {
    c = friendsSetGetData(n, &l);
    if (!c) continue;
    if (friendsGetHash(c) != hsh) continue;
    l = friendsListFind(l, d);
    if (l) {
      friendsListRemove(l);
    }
    break;
  }
}

void friendsSetRemoveAll(friendsDataSet *set, friendsData *d, friendsError *e)
{
  friendsDataList *l, *p;
  friendsDataSetNode *n;
  int i;

  friendsAssert(set);
  friendsAssert(d);

  for (i = 0; i < friendsDataSetSize; ++i) {
    n = set->table[i];
    for (; n; n = n->next) {
      l = n->list;
      if (!l) continue;
      while ((l = friendsListFind(l, d))) {
        p = friendsListNext(l);
        friendsListRemove(l);
        l = p;
      }
    }
  }
}

void friendsSetEach(friendsDataSet *set,
                    friendsBool (*f)(friendsDataList *list, void *a),
                    void *a)
{
  friendsDataList *l;
  friendsDataSetNode *n;
  int i;

  friendsAssert(set);
  friendsAssert(f);

  for (i = 0; i < friendsDataSetSize; ++i) {
    n = set->table[i];
    for (; n; n = n->next) {
      l = n->list;
      if (!l) continue;
      if (f(l, a) == friendsFalse) return;
    }
  }
}
