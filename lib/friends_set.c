
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_error.h"
#include "friends_set.h"
#include "friends_struct_data.h"
#include "friends_rbtree.h"
#include "friends_list.h"
#include "friends_core.h"
#include "friends_data.h"
#include "friends_string.h"
#include "friends_atom.h"
#include "friends_variable.h"
#include "friends_proposition.h"
#include "friends_data_private.h"

struct friendsDataSet
{
  friendsDataSetNode *root;
};

struct friendsDataSetNode
{
  friendsRbTree tree; /*!< ツリー */
  friendsList list;   /*!< リスト */
  friendsData data;   /*!< データ */
};

#define friendsSetTreeContainer(ptr)                            \
  friendsRbTreeContainer(ptr, struct friendsDataSetNode, tree)

#define friendsSetListContainer(ptr)                            \
  friendsListContainer(ptr, struct friendsDataSetNode, list)

friendsDataSet *friendsNewSet(friendsError *e)
{
  friendsDataSet *set;

  set = (friendsDataSet *)friendsMalloc(sizeof(friendsDataSet), e);
  if (!set) {
    return NULL;
  }

  set->root = NULL;
  return set;
}

static friendsData *friendsSetGetData(friendsDataSetNode *n)
{
  friendsAssert(n);
  return &n->data;
}

static int
friendsSetTreeCompare(friendsRbTree *x, friendsRbTree *y)
{
  friendsDataSetNode *nx;
  friendsDataSetNode *ny;
  friendsData *dx;
  friendsData *dy;
  friendsHash hx;
  friendsHash hy;
  friendsDataCompareResult cres;

  nx = friendsSetTreeContainer(x);
  ny = friendsSetTreeContainer(y);
  dx = friendsSetGetData(nx);
  dy = friendsSetGetData(ny);
  hx = friendsGetHash(dx);
  hy = friendsGetHash(dy);

  if (hx != hy) {
    return hy - hx;
  }

  cres = friendsDataCompare(dx, dy);

  if (cres & friendsDataDifferentType) {
    friendsType tx, ty;

    tx = friendsGetType(dx);
    ty = friendsGetType(dy);
    if (tx != ty) {
      return ty - tx;
    }
  }
  if (friendsDataCompareIsSetEqual(cres)) {
    return 0;
  }
  if (cres == friendsDataGreater) {
    return -1;
  }
  if (cres == friendsDataLess) {
    return  1;
  }

  if (dx->object > dy->object) {
    return 1;
  } else if (dx->object == dy->object) {
    return 0;
  } else {
    return -1;
  }
}

void friendsSetDeleteNode(friendsDataSet *set, friendsDataSetNode *n)
{
  friendsList *ln;
  friendsRbTree *r;
  friendsDataSetNode *new_node;

  friendsAssert(set);
  friendsAssert(n);
  friendsAssert(set->root);

  new_node = NULL;
  if (!friendsListIsEmpty(&n->list)) {
    ln = friendsListNext(&n->list);
    new_node = friendsSetListContainer(ln);
  }
  if (n->tree.parent) {
    r = friendsRbTreeDelete(&set->root->tree, &n->tree);
    if (new_node) {
      r = friendsRbTreeInsert(r, &new_node->tree, friendsSetTreeCompare);
      friendsAssert(r);
    }
    set->root = friendsSetTreeContainer(r);
  }

  friendsDataDelink(&n->data);
  friendsFree(n);
}

void friendsDeleteSet(friendsDataSet *set)
{
  if (!set) return;

  while (set->root) {
    friendsSetDeleteNode(set, set->root);
  }

  friendsFree(set);
}

static void
friendsSetNodeInit(friendsDataSetNode *n)
{
  friendsAssert(n);

  friendsRbTreeInit(&n->tree);
  n->tree.parent = NULL;
  friendsListInit(&n->list);
  friends_DataInit(&n->data);
}

static void
friendsSetNodeSetData(friendsDataSetNode *n, const friendsData *d,
                      friendsError *e)
{
  friendsDataCopy(&n->data, d, e);
}

friendsDataSetNode *
friendsSetFindByNode(friendsDataSet *set, friendsDataSetNode *node)
{
  friendsRbTree *t;

  friendsAssert(set);
  friendsAssert(node);

  if (!set->root) return NULL;

  t = friendsRbTreeFind(&set->root->tree, &node->tree, friendsSetTreeCompare);
  if (!t) return NULL;

  return friendsSetTreeContainer(t);
}

friendsDataSetNode *
friendsSetFindByData(friendsDataSet *set, const friendsData *d)
{
  friendsDataSetNode n;
  friendsDataSetNode *ret;

  friendsAssert(d);

  friendsSetNodeInit(&n);
  friendsSetNodeSetData(&n, d, NULL);

  ret = friendsSetFindByNode(set, &n);

  friendsDataDelink(&n.data);

  return ret;
}

friendsDataSetNode *
friendsSetFindByText(friendsDataSet *set, friendsType type,
                     const friendsChar *text, friendsError *e)
{
  friendsData t;
  friendsData *retd;
  friendsPark *park;
  friendsDataSetNode *ret;

  friendsAssert(set);

  if (!set->root) return NULL;

  park = friendsGetPark(&set->root->data);
  if (!park) return NULL;

  friends_DataInit(&t);
  t.park = park;

  switch(type) {
  case friendsAtom:
    retd = friendsSetTextAtom(&t, text, e);
    break;
  case friendsProposition:
    retd = friendsSetProposition(&t, text, NULL, NULL,
                                 friendsPropositionNormal, e);
    break;
  case friendsVariable:
    retd = friendsSetVariable(&t, text, friendsFalse, e);
    break;
  default:
    friendsUnreachable();
    friendsSetError(e, InvalidType);
    retd = NULL;
    break;
  }
  if (!retd) {
    return NULL;
  }

  ret = friendsSetFindByData(set, &t);

  friendsDataDelink(&t);

  return ret;
}

friendsDataSetNode *
friendsSetFindNumericAtom(friendsDataSet *set, int a, friendsError *e)
{
  friendsData t;
  friendsPark *park;
  friendsDataSetNode *ret;

  friendsAssert(set);
  friendsAssert(a >= 0);

  if (!set->root) return NULL;

  park = friendsGetPark(&set->root->data);
  if (!park) return NULL;

  friends_DataInit(&t);
  t.park = park;

  if (!friendsSetNumeralAtom(&t, a, e)) {
    return NULL;
  }

  ret = friendsSetFindByData(set, &t);

  friendsDataDelink(&t);

  return ret;
}

static friendsDataSetNode *
friendsSetInsertCommon(friendsDataSet *set, friendsData *d,
                       friendsBool allow_duplicate, friendsError *e)
{
  friendsDataSetNode *n;
  friendsDataSetNode *p;
  friendsRbTree *root;

  friendsAssert(set);
  friendsAssert(d);

  n = (friendsDataSetNode *)friendsMalloc(sizeof(friendsDataSetNode), e);
  if (!n) return NULL;

  friendsSetNodeInit(n);
  friendsSetNodeSetData(n, d, NULL);

  p = friendsSetFindByNode(set, n);
  if (p) {
    if (allow_duplicate == friendsTrue) {
      friendsListInsertPrev(&p->list, &n->list);
    } else {
      friendsSetError(e, SetDuplicated);
      friendsDataDelink(&n->data);
      friendsFree(n);
      n = NULL;
    }
  } else {
    root = &set->root->tree;
    root = friendsRbTreeInsert(root, &n->tree, friendsSetTreeCompare);
    set->root = friendsSetTreeContainer(root);
  }
  return n;
}

friendsDataSetNode *
friendsSetInsert(friendsDataSet *set, friendsData *d, friendsError *e)
{
  return friendsSetInsertCommon(set, d, friendsFalse, e);
}

friendsDataSetNode *
friendsSetInsertMulti(friendsDataSet *set, friendsData *d, friendsError *e)
{
  return friendsSetInsertCommon(set, d, friendsTrue, e);
}

friendsDataSetNode *
friendsSetNodeNext(friendsDataSetNode *n)
{
  return friendsSetListContainer(friendsListNext(&n->list));
}

void friendsSetRemove(friendsDataSet *set, friendsData *d)
{
  friendsDataSetNode *n;

  friendsAssert(set);
  friendsAssert(d);

  n = friendsSetFindByData(set, d);
  if (!n) return;

  friendsSetDeleteNode(set, n);
}

void friendsSetRemoveAll(friendsDataSet *set, friendsData *d)
{
  friendsDataSetNode *n;
  friendsList *l, *ln, *lp;

  friendsAssert(set);
  friendsAssert(d);

  n = friendsSetFindByData(set, d);
  if (!n) return;

  l = &n->list;
  friendsListForeachSafe(lp, ln, l) {
    friendsDataSetNode *nn;
    nn = friendsSetListContainer(lp);
    friendsSetDeleteNode(set, nn);
  }
  friendsSetDeleteNode(set, n);
}

