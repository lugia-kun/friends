
#include <stdlib.h>

#include "friends_error.h"
#include "friends_defs.h"
#include "friends_list.h"
#include "friends_data.h"
#include "friends_struct_data.h"

friendsDataList *friendsNewList(friendsError *e)
{
  friendsDataList *l;
  l = (friendsDataList *)calloc(sizeof(friendsDataList), 1);
  if (!l) {
    friendsSetError(e, NOMEM);
    return NULL;
  }

  l->data = NULL;
  l->next = NULL;
  l->prev = NULL;
  l->begin = NULL;
  l->size = 0;
  return l;
}

void friendsDeleteList(friendsDataList *l)
{
  friendsDataList *nex;
  friendsDataList *p;

  if (!l) return;

  p = friendsListParent(l);
  l = friendsListBegin(l);
  do {
    nex = friendsListNext(l);
    free(l);
    l = nex;
  } while(nex);
  free(p);
}

friendsDataList *friendsListNext(friendsDataList *l)
{
  if (!l) return NULL;
  return l->next;
}

friendsDataList *friendsListPrev(friendsDataList *l)
{
  if (!l) return NULL;
  return l->prev;
}

size_t friendsListSize(friendsDataList *l)
{
  if (!l) return 0;

  l = friendsListParent(l);
  return l->size;
}

friendsData *friendsListData(friendsDataList *l)
{
  if (!l) return NULL;
  return l->data;
}

void friendsSetListData(friendsDataList *l, friendsData *d)
{
  friendsAssert(l);
  friendsAssert(l->begin);

  l->data = d;
}

friendsBool friendsListIsBegin(friendsDataList *l)
{
  if (!l) return friendsFalse;
  if (!l->prev) return friendsTrue;
  return friendsFalse;
}

friendsBool friendsListIsEnd(friendsDataList *l)
{
  if (!l) return friendsFalse;
  if (!l->next) return friendsTrue;
  return friendsFalse;
}

friendsBool friendsListIsParent(friendsDataList *l)
{
  if (!l) return friendsFalse;
  if (l->begin) return friendsFalse;
  return friendsTrue;
}

friendsDataList *friendsListBegin(friendsDataList *l)
{
  if (!l) return NULL;
  if (!l->begin) return l->next;
  return l->begin->next;
}

friendsDataList *friendsListEnd(friendsDataList *l)
{
  if (!l) return NULL;
  if (!l->begin) return l->prev;
  return l->begin->prev;
}

friendsDataList *friendsListParent(friendsDataList *l)
{
  if (!l) return NULL;
  if (!l->begin) return l;
  return l->begin;
}

friendsDataList *friendsListInsert(friendsDataList *l,
                                   friendsData *d, friendsError *e)
{
  friendsDataList *n, *p, *b;

  friendsAssert(l);

  n = friendsNewList(e);
  if (!n) return NULL;

  p = friendsListPrev(l);

  b = friendsListParent(l);
  friendsAssert(b);

  if (b->size + 1 == 0) {
    friendsSetError(e, 2BIG);
    free(n);
    return NULL;
  }

  n->data = d;
  if (l == b) {
    n->next = b->next;
    if (b->next) {
      b->next->prev = n;
    }
    b->next = n;
  } else {
    if (p) {
      p->next = n;
      n->prev = p;

      l->prev = n;
      n->next = l;
    } else {
      b->next = n;

      n->next = l;
      l->prev = n;
    }
  }

  n->begin = b;
  if (!b->prev) {
    b->prev = n;
  }

  b->size += 1;
  return n;
}

friendsDataList *friendsListAppend(friendsDataList *l,
                                   friendsData *d, friendsError *e)
{
  friendsDataList *n, *p, *b;

  friendsAssert(l);

  n = friendsNewList(e);
  if (!n) return NULL;

  p = friendsListEnd(l);

  b = friendsListParent(l);
  friendsAssert(b);

  if (b->size + 1 == 0) {
    friendsSetError(e, 2BIG);
    free(n);
    return NULL;
  }

  n->data = d;
  if (p) {
    p->next = n;
    n->prev = p;
  }

  if (!b->next) {
    b->next = n;
  }
  b->prev = n;
  n->begin = b;
  b->size += 1;

  return n;
}

friendsDataList *friendsListFind(friendsDataList *l, friendsData *d)
{
  friendsAssert(l);

  while (l) {
    if (friendsListData(l) == d) return l;
    l = friendsListNext(l);
  }
  return NULL;
}

friendsDataList *friendsListFindIf(friendsDataList *l,
                                   friendsBool (*f)(friendsData *d, void *a),
                                   void *a)
{
  friendsData *d;

  friendsAssert(l);

  while (l) {
    d = friendsListData(l);
    if (d) {
      if (f(d, a) == friendsTrue) return l;
    }
    l = friendsListNext(l);
  }
  return NULL;
}

void friendsListEach(friendsDataList *l,
                     friendsBool (*f)(friendsData *d, void *a),
                     void *a)
{
  friendsData *d;

  friendsAssert(l);

  l = friendsListBegin(l);
  while (l) {
    d = friendsListData(l);
    if (d) {
      if (f(d, a) == friendsFalse) break;
    }
    l = friendsListNext(l);
  }
}

void friendsListEachItem(friendsDataList *l,
                         friendsBool (*f)(friendsDataList *l, void *a),
                         void *a)
{
  friendsAssert(l);

  l = friendsListBegin(l);
  while (l) {
    if (f(l, a) == friendsFalse) break;
    l = friendsListNext(l);
  }
}

void friendsListRemove(friendsDataList *l)
{
  friendsDataList *n, *p;

  friendsAssert(l);
  friendsAssert(l->begin);

  p = l->prev;
  n = l->next;

  if (p) {  /* If not the first data */
    p->next = n;
  } else {
    l->begin->next = n;
  }
  if (n) {  /* If not the last data */
    n->prev = p;
  } else {
    l->begin->prev = p;
  }

  l->begin->size -= 1;
  free(l);
}

void friendsListRemoveAt(friendsDataList *l,
                         int index, friendsError *e)
{
  friendsAssert(l);

  if (friendsListIsParent(l) == friendsTrue) {
    if (index >= 0) {
      l = friendsListBegin(l);
    }
  }
  if (index > 0) {
    while (l && index > 0) {
      l = friendsListNext(l);
      index--;
    }
  } else if (index < 0) {
    while (l && index < 0) {
      l = friendsListPrev(l);
      index++;
    }
  }

  if (!l) {
    friendsSetError(e, ListIndex);
    return;
  }

  friendsListRemove(l);
}

friendsData *friendsSetList(friendsData *dest, friendsDataList *l,
                            friendsError *err)
{
  friendsAssert(dest);
  friendsAssert(l);

  l = friendsListParent(l);

  return friendsSetData(dest, friendsList, l,
                        (friendsPointerDeleter *)friendsDeleteList,
                        NULL, NULL, NULL, 0, friendsFalse, err);
}

friendsDataList *friendsGetList(friendsData *d)
{
  friendsAssert(d);

  if (friendsGetType(d) != friendsList) return NULL;
  return d->data;
}
