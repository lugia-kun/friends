
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "friends_park.h"
#include "friends_data.h"
#include "friends_struct_data.h"
#include "friends_error.h"
#include "friends_string.h"
#include "friends_list.h"
#include "friends_core.h"
#include "friends_set.h"
#include "friends_data_private.h"

static friendsDataObject *
friendsDataObjectResize(friendsDataObject *obj, friendsSize sz,
                        friendsError *e)
{
  if (sz < sizeof(intptr_t)) sz = sizeof(intptr_t);
  sz = (sz / sizeof(intptr_t) + 1) * sizeof(intptr_t);

  obj = realloc(obj, sizeof(friendsDataObject) + sz);
  if (!obj) {
    friendsSetError(e, NOMEM);
    return NULL;
  }

  obj->size = sz;
  return obj;
}

static friendsDataObject *
friendsDataObjectResizeIfSmall(friendsDataObject *obj, friendsSize sz,
                               friendsError *e)
{
  friendsAssert(obj);

  if (obj->size >= sz) return obj;

  return friendsDataObjectResize(obj, sz, e);
}

void friends_DataInit(friendsData *d)
{
  d->object = NULL;
  d->park = NULL;
}

friendsData *
friendsNewData(friendsPark *park, friendsError *e)
{
  friendsData *p;

  friendsAssert(park);

  p = (friendsData *)friendsMalloc(sizeof(friendsData), e);
  if (!p) {
    return NULL;
  }

  if (!friendsAddPointer(park, p,
                         (friendsPointerDeleter *)friendsDeleteData, e)) {
    free(p);
    return NULL;
  }

  friends_DataInit(p);
  p->park = park;
  return p;
}

void friendsDataDelink(friendsData *d)
{
  friendsDataObject *obj;

  friendsAssert(d);

  obj = d->object;
  if (!obj) return;

  obj->refc--;
  if (obj->refc == 0) {
    if (obj->funcs) {
      if (obj->funcs->deleter) {
        obj->funcs->deleter(obj->data);
      }
      if (obj->funcs->txt_deleter) {
        obj->funcs->txt_deleter(obj->txt, obj->data);
      }
    }
    friendsFree(obj);
  }
  d->object = NULL;
}

void friendsDeleteData(friendsData *d)
{
  if (!d) return;

  friendsDeletePointer(d->park, d);

  if (d->object) {
    friendsDataDelink(d);
  }
  friendsFree(d);
}

friendsError
friends_DataSet(friendsData *d, friendsType type, friendsSize data_size,
                const friendsDataFunctions *funcs, friendsHash hash)
{
  friendsError e;
  friendsDataObject *obj;
  friendsDataObject *nobj;

  friendsAssert(d);

  e = friendsNoError;

  obj = d->object;
  if (obj && obj->refc == 1) {
    nobj = friendsDataObjectResizeIfSmall(obj, data_size, &e);
  } else {
    if (obj) {
      friendsDataDelink(d);
    }
    nobj = friendsDataObjectResize(NULL, data_size, &e);
  }
  if (!nobj) {
    return e;
  }

  nobj->funcs = funcs;
  nobj->refc = 1;
  nobj->txt = NULL;
  nobj->type = type;
  nobj->hash = hash;
  d->object = nobj;
  return e;
}

friendsPark *friendsGetPark(const friendsData *d)
{
  friendsAssert(d);

  return d->park;
}

friendsType friendsGetType(const friendsData *d)
{
  friendsAssert(d);

  if (d->object) {
    return d->object->type;
  } else {
    return friendsInvalidType;
  }
}

friendsHash friendsGetHash(const friendsData *d)
{
  friendsAssert(d);

  if (d->object) {
    return d->object->hash;
  } else {
    return friendsHashC(0);
  }
}

void *friends_DataGetTypeData(friendsData *d)
{
  friendsAssert(d);

  if (d->object) return d->object->data;
  return NULL;
}

const friendsChar *friendsDataToText(friendsData *d)
{
  friendsDataObject *obj;

  friendsAssert(d);

  obj = d->object;
  if (!obj) return NULL;

  if (!obj->txt) {
    if (obj->funcs && obj->funcs->txt_creator) {
      friendsError e;
      e = obj->funcs->txt_creator(&obj->txt, obj->data);
      if (e != friendsNoError) {
        obj->txt = NULL;
      }
    }
  }
  return obj->txt;
}

friendsDataCompareResult
friendsDataCompare(const friendsData *a, const friendsData *b)
{
  friendsDataCompareResult t;
  friendsDataCompareFunc *cfa;
  friendsDataObject *ao, *bo;

  friendsAssert(a);
  friendsAssert(b);

  t = 0;

  if (a->park != b->park) {
    t |= friendsDataInAnotherPark;
  }

  ao = a->object;
  bo = b->object;
  if (!ao || !bo) {
    t |= friendsDataNullObject;
  }
  if (ao == bo) {
    t |= friendsDataObjectEqual;
    if (ao) t |= friendsDataEqual;
  } else if (ao && bo) {
    if (ao->type != bo->type) {
      t |= friendsDataDifferentType;
    } else {
      cfa = NULL;
      if (ao->funcs) {
        cfa = ao->funcs->comparator;
      } else if (bo->funcs) {
        cfa = bo->funcs->comparator;
      }
      if (cfa) {
        t |= cfa(ao->data, bo->data);
      } else {
        t |= friendsDataNotComparable;
      }
    }
  }
  return t;
}

friendsData *friendsDataCopy(friendsData *dest, const friendsData *src,
                             friendsError *e)
{
  friendsAssert(src);
  friendsAssert(dest);

  if (dest->park && dest->park != src->park) {
    friendsSetError(e, InvalidPark);
    return NULL;
  }

  if (dest->object) {
    friendsDataDelink(dest);
  }
  dest->object = src->object;
  dest->park = src->park;
  dest->object->refc += 1;
  return dest;
}

static friendsHash friendsTopNbit(friendsHash h, unsigned int n)
{
  return h << (sizeof(friendsHash) * CHAR_BIT - n);
}

static friendsHash friendsHasher(const char *s, const char *e)
{
  int off;
  const char *p;
  friendsHash h = 0x1afe21c5;
  friendsHash f = h;
  size_t n = sizeof(friendsHash) * CHAR_BIT;

  for (p = s; p < e; ++p) {
    off = ((p - s) * 5) % n;
    f = *p * h;
    h = h ^ ((f << off) | (f >> off));
  }
  return h;
}

friendsHash friendsHashNumeral(int a)
{
  char *s, *e;
  friendsHash h, t;

  h = (friendsHash) a;
  h = h << 4;
  h = h >> 4;

  if (h == a) {
    h = friendsTopNbit(0x1, 4);
    return h | a;
  }

  s = (char *)  &a;
  e = (char *) (&a + 1);
  h = friendsHasher(s, e);
  t = friendsTopNbit(0x1, 3);
  h = h & (t - 1);
  return h | t;
}

friendsHash friendsHashString(const friendsChar *s, const friendsChar *e)
{
  friendsHash h, t;

  friendsAssert(s);

  if (!e) e = s + friendsStringArrayLength(s);

  h = friendsHasher((const char *)s, (const char *)e);
  t = friendsTopNbit(0x1, 1);
  return h | t;
}

friendsHash friendsHashBinary(const void *s, const void *e)
{
  friendsHash h, t;

  friendsAssert(s);
  friendsAssert(e);

  h = friendsHasher(s, e);
  t = friendsTopNbit(0x1, 2);
  h = h & (t - 1);
  return h | t;
}

struct friendsDataList
{
  struct friendsList list;
  struct friendsData data;
  struct friendsDataList *head;
};
#define friendsDataListEntry(ptr) \
  friendsListContainer(ptr, friendsDataList, list)

static
void friendsDataListInit(friendsDataList *l)
{
  friendsAssert(l);

  friendsListInit(&l->list);
  l->head = l;
  friends_DataInit(&l->data);
}

friendsDataList *friendsNewDataList(friendsError *e)
{
  friendsDataList *l;
  l = (friendsDataList *)friendsMalloc(sizeof(friendsDataList), e);
  if (!l) return NULL;

  friendsDataListInit(l);
  return l;
}

friendsBool friendsDataListIsHead(friendsDataList *item)
{
  friendsAssert(item);
  return (item->head == item) ? friendsTrue : friendsFalse;
}

void friendsDataListDelete(friendsDataList *item, friendsError *e)
{
  friendsAssert(item);

  if (friendsDataListIsHead(item) == friendsTrue) {
    friendsSetError(e, INVAL);
    return;
  }

  friendsListDelete(&item->list);
  friendsDataDelink(&item->data);
  friendsFree(item);
}

void friendsDataListDeleteAll(friendsDataList *list)
{
  friendsDataList *head;
  friendsDataList *item;
  struct friendsList *headp;
  struct friendsList *it, *next;

  friendsAssert(list);

  head = list->head;
  headp = &head->list;

  friendsListForeachSafe(it, next, headp) {
    item = friendsDataListEntry(it);
    friendsDataListDelete(item, NULL);
  }
  free(head);
}

friendsBool friendsDataListIsEmpty(friendsDataList *list)
{
  friendsAssert(list);

  if (!friendsDataListIsHead(list)) {
    return friendsFalse;
  }
  if (friendsListIsEmpty(&list->list) == friendsTrue) {
    return friendsTrue;
  }
  return friendsFalse;
}

friendsDataList *
friendsDataListInsert(friendsDataList *item, const friendsData *data,
                      friendsError *e)
{
  friendsDataList *head;
  friendsDataList *new;
  friendsData *ret;

  friendsAssert(item);
  friendsAssert(data);

  head = item->head;
  friendsAssert(head);

  if (head->data.park && head->data.park != data->park) {
    friendsSetError(e, InvalidPark);
    return NULL;
  }

  new = friendsNewDataList(e);
  if (!new) return NULL;

  ret = friendsDataCopy(&new->data, data, e);
  if (!ret) {
    free(new);
    return NULL;
  }

  friendsListInsertNext(&item->list, &new->list);
  head->data.park = data->park;
  new->head = head;
  return new;
}

friendsDataList *
friendsDataListInsertAfter(friendsDataList *item, const friendsData *data,
                           friendsError *e)
{
  friendsDataList *next;
  next = friendsDataListEntry(item->list.next);

  return friendsDataListInsert(next, data, e);
}

friendsDataList *
friendsDataListAppend(friendsDataList *list, const friendsData *data,
                      friendsError *e)
{
  return friendsDataListInsert(list->head, data, e);
}

friendsDataList *
friendsDataListPrepend(friendsDataList *list, const friendsData *data,
                       friendsError *e)
{
  return friendsDataListInsertAfter(list->head, data, e);
}

friendsDataList *friendsDataListNext(friendsDataList *item)
{
  friendsDataList *next;

  friendsAssert(item);
  next = friendsDataListEntry(friendsListNext(&item->list));
  if (next->head != next) {
    return next;
  }
  return NULL;
}

friendsDataList *friendsDataListPrev(friendsDataList *item)
{
  friendsDataList *prev;

  friendsAssert(item);
  prev = friendsDataListEntry(friendsListPrev(&item->list));
  if (prev->head != prev) {
    return prev;
  }
  return NULL;
}

friendsDataList *friendsDataListHead(friendsDataList *item)
{
  friendsAssert(item);
  return item->head;
}

friendsData *friendsDataListGetData(friendsDataList *item)
{
  friendsAssert(item);
  return &item->data;
}

friendsDataList *friendsDataListBegin(friendsDataList *list)
{
  friendsDataList *head;

  friendsAssert(list);

  head = friendsDataListHead(list);
  return friendsDataListNext(head);
}

friendsDataList *friendsDataListEnd(friendsDataList *list)
{
  friendsDataList *head;

  friendsAssert(list);

  head = friendsDataListHead(list);
  return friendsDataListPrev(head);
}

static friendsDataList *friendsDataGetListData(void *p)
{
  return (friendsDataList *)p;
}

static
void *friendsDataListTypeDeleter(void *p)
{
  friendsDataList *l;
  friendsDataList *n;

  l = friendsDataGetListData(p);
  for (l = friendsDataListBegin(l), n = friendsDataListNext(l);
       l; l = n, n = (n ? friendsDataListNext(n) : NULL)) {
    friendsDataListDelete(l, NULL);
  }
  return p;
}

friendsDataList *
friendsDataListInsertList(friendsDataList *dest,
                          friendsDataList *srcst, friendsDataList *srced,
                          friendsError *e)
{
  friendsDataList *srccur;
  friendsDataList *tmphead;
  friendsDataList *head;
  const friendsData *d;

  friendsAssert(dest);
  friendsAssert(srcst);

  if (friendsDataListIsHead(srced)) {
    srced = NULL;
  }

  tmphead = friendsNewDataList(e);
  if (!tmphead) {
    return NULL;
  }

  for (srccur = srcst; srccur && srccur != srced;
       srccur = friendsDataListNext(srccur)) {
    d = friendsDataListGetData(srccur);
    if (!friendsDataListInsert(tmphead, d, e)) {
      break;
    }
  }

  if (srccur == srced) {
    head = friendsDataListHead(dest);
    srcst = dest;
    srced = friendsDataListNext(srcst);
    friendsListInsertList(&dest->list, &tmphead->list);
    for (srccur = friendsDataListNext(srcst); srccur != srced;
         srccur = friendsDataListNext(srccur)) {
      srccur->head = head;
    }
  } else {
    srcst = NULL;
  }
  friendsDataListDeleteAll(tmphead);
  return srcst;
}

friendsDataList *
friendsDataListDuplicate(friendsDataList *list, friendsError *e)
{
  friendsDataList *new;
  friendsDataList *ret;

  friendsAssert(list);

  list = friendsDataListHead(list);

  new = friendsNewDataList(e);
  if (!new) return NULL;

  ret = friendsDataListInsertList(new, list, NULL, e);
  if (!ret) {
    friendsDataListDeleteAll(list);
    return NULL;
  }

  return new;
}

static friendsDataFunctions friendsDataListFuncs = {
  .deleter = friendsDataListTypeDeleter,
  .comparator = NULL,
  .txt_creator = NULL,
  .txt_deleter = NULL,
};

friendsData *friendsSetList(friendsData *data, friendsDataList *list,
                            friendsError *e)
{
  friendsError err;
  friendsDataList *l;

  friendsAssert(data);
  friendsAssert(list);

  err = friends_DataSet(data, friendsListType, sizeof(friendsDataList),
                        &friendsDataListFuncs);
  if (friendsAnyError(err)) {
    if (e) *e = err;
    return NULL;
  }

  l = friendsDataGetListData(friends_DataGetTypeData(data));
  friendsDataListInit(l);

  if (!friendsDataListInsertList(l, list, NULL, e)) {
    friendsDataListTypeDeleter(l);
    return NULL;
  }

  return data;
}

friendsDataList *friendsGetList(friendsData *data, friendsError *e)
{
  friendsAssert(data);
  if (friendsGetType(data) != friendsListType) {
    friendsSetError(e, InvalidType);
    return NULL;
  }

  return friendsDataGetListData(friends_DataGetTypeData(data));
}
