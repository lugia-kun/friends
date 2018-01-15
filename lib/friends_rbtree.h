/**
 * @brief 赤黒木なのです。
 */

#ifndef FRIENDS_RBTREE_H
#define FRIENDS_RBTREE_H

#include "friends_defs.h"

enum friendsRbTreeColor
{
  friendsRbTreeRed, friendsRbTreeBlack
};

struct friendsRbTree
{
  struct friendsRbTree *parent;
  struct friendsRbTree *left;
  struct friendsRbTree *right;
  enum friendsRbTreeColor color;
};
typedef struct friendsRbTree friendsRbTree;

typedef int friendsRbTreeCompare(friendsRbTree *x, friendsRbTree *y);

#define friendsRbTreeContainer(ptr, type, field) \
  ((type *)((char *)ptr - offsetof(type, field)))

static inline
friendsRbTree *friendsRbTreeParent(friendsRbTree *rb)
{
  return rb->parent;
}

static inline
friendsRbTree *friendsRbTreeLeft(friendsRbTree *rb)
{
  return rb->left;
}

static inline
friendsRbTree *friendsRbTreeRight(friendsRbTree *rb)
{
  return rb->right;
}

static inline
enum friendsRbTreeColor friendsRbTreeColor(friendsRbTree *rb)
{
  if (rb) {
    if (rb->parent == rb) {
      return friendsRbTreeBlack;
    }
    return rb->color;
  }
  return friendsRbTreeRed;
}

static inline
friendsRbTree *friendsRbTreeSibling(friendsRbTree *t)
{
  if (t == t->parent->left) return t->parent->right;
  if (t == t->parent->right) return t->parent->left;
  return NULL;
}

static inline
friendsBool friendsRbTreeIsLeftChild(friendsRbTree *t)
{
  return (t == t->parent->left) ? friendsTrue : friendsFalse;
}

static inline
friendsBool friendsRbTreeIsRightChild(friendsRbTree *t)
{
  return (t == t->parent->right) ? friendsTrue : friendsFalse;
}

static inline
void friendsRbTreeInit(friendsRbTree *t)
{
  t->parent = t;
  t->left = NULL;
  t->right = NULL;
  t->color = friendsRbTreeBlack;
}

static inline friendsRbTree *
friends_RbTreeLeftRotate(friendsRbTree *root, friendsRbTree *x)
{
  friendsRbTree *y;
  y = x->right;
  if (!y) return root;

  x->right = y->left;
  if (y->left) {
    y->left->parent = x;
  }
  if (x->parent == x) {
    y->parent = y;
    root = y;
  } else {
    y->parent = x->parent;
    if (x == x->parent->left) {
      x->parent->left = y;
    } else {
      x->parent->right = y;
    }
  }
  y->left = x;
  x->parent = y;
  return root;
}

static inline friendsRbTree *
friends_RbTreeRightRotate(friendsRbTree *root, friendsRbTree *y)
{
  friendsRbTree *x;
  x = y->left;
  if (!x) return root;

  y->left = x->right;
  if (x->right) {
    x->right->parent = y;
  }
  if (y->parent == y) {
    x->parent = x;
    root = x;
  } else {
    x->parent = y->parent;
    if (y == y->parent->right) {
      y->parent->right = x;
    } else {
      y->parent->left = x;
    }
  }
  x->right = y;
  y->parent = x;
  return root;
}

static inline friendsRbTree *
friends_RbTreeInsertFix(friendsRbTree *root, friendsRbTree *z)
{
  friendsRbTree *y;
  friendsRbTree *zp;
  zp = z->parent;
  if (zp == z) return root;
  while (zp && zp->color == friendsRbTreeRed) {
    if (zp->parent->left == zp) {
      y = zp->parent->right;
      if (y && y->color == friendsRbTreeRed) {
        zp->color = friendsRbTreeBlack;
        y->color = friendsRbTreeBlack;
        zp->parent->color = friendsRbTreeRed;
        z = zp->parent;
        zp = z->parent;
        if (zp == z) break;
      } else {
        if (z == zp->right) {
          z = zp;
          root = friends_RbTreeLeftRotate(root, z);
          zp = z->parent;
          if (zp == z) break;
        }
        zp->color = friendsRbTreeBlack;
        if (zp->parent == zp) break;
        zp->parent->color = friendsRbTreeRed;
        root = friends_RbTreeRightRotate(root, zp->parent);
      }
    } else {
      y = zp->parent->left;
      if (y && y->color == friendsRbTreeRed) {
        zp->color = friendsRbTreeBlack;
        y->color = friendsRbTreeBlack;
        zp->parent->color = friendsRbTreeRed;
        z = zp->parent;
        zp = z->parent;
        if (zp == z) break;
      } else {
        if (z == zp->left) {
          z = zp;
          root = friends_RbTreeRightRotate(root, z);
          zp = z->parent;
          if (zp == z) break;
        }
        zp->color = friendsRbTreeBlack;
        if (zp->parent == zp)  break;
        zp->parent->color = friendsRbTreeRed;
        root = friends_RbTreeLeftRotate(root, zp->parent);
      }
    }
  }
  return root;
}

static inline friendsRbTree *
friendsRbTreeInsert(friendsRbTree *root, friendsRbTree *newp,
                    friendsRbTreeCompare *comp)
{
  friendsRbTree *x;
  friendsRbTree *y;
  int i;

  y = NULL;
  x = root;
  while (x) {
    y = x;
    i = comp(newp, x);
    if (i < 0) {
      x = x->left;
    } else if (i == 0) {
      return NULL;
    } else {
      x = x->right;
    }
  }
  if (y) {
    i = comp(newp, y);
    if (i < 0) {
      y->left = newp;
    } else if (i == 0) {
      return NULL;
    } else {
      y->right = newp;
    }
    newp->parent = y;
  } else {
    newp->parent = newp;
    root = newp;
  }
  newp->left = NULL;
  newp->right = NULL;
  newp->color = friendsRbTreeRed;

  root = friends_RbTreeInsertFix(root, newp);
  root->color = friendsRbTreeBlack;
  return root;
}


static inline friendsRbTree *
friendsRbTreeMinimum(friendsRbTree *item)
{
  while (item->left) {
    item = item->left;
  }
  return item;
}

static inline friendsRbTree *
friendsRbTreeMaximum(friendsRbTree *item)
{
  while (item->right) {
    item = item->right;
  }
  return item;
}

static inline friendsRbTree *
friends_RbTreeTransplant(friendsRbTree *root,
                         friendsRbTree *u, friendsRbTree *v)
{
  if (u->parent == u) {
    root = v;
    if (v) v->parent = v;
  } else {
    if (u == u->parent->left) {
      u->parent->left = v;
    } else {
      u->parent->right = v;
    }
    if (v) v->parent = u->parent;
  }
  return root;
}

static inline friendsRbTree *
friends_RbTreeDeleteFix(friendsRbTree *root, friendsRbTree *x)
{
  friendsRbTree *w;
  friendsRbTree *xp;

  while (x != root && x->color == friendsRbTreeBlack) {
    xp = x->parent;
    if (x == xp->left) {
      w = xp->right;
      if (w && w->color == friendsRbTreeRed) {
        w->color = friendsRbTreeBlack;
        xp->color = friendsRbTreeRed;
        root = friends_RbTreeLeftRotate(root, xp);
        xp = x->parent;
        w = xp->right;
      }
      if (w &&
          (!w->left || w->left->color == friendsRbTreeBlack) &&
          (!w->right || w->right->color == friendsRbTreeBlack)) {
        w->color = friendsRbTreeRed;
        x = xp;
        xp = x->parent;
      } else {
        if (w && (!w->right || w->right->color == friendsRbTreeBlack)) {
          if (w->left) w->left->color = friendsRbTreeBlack;
          w->color = friendsRbTreeRed;
          root = friends_RbTreeRightRotate(root, w);
          xp = x->parent;
          w = xp->right;
        }
        if (w) w->color = xp->color;
        xp->color = friendsRbTreeBlack;
        root = friends_RbTreeLeftRotate(root, xp);
        x = root;
      }
    } else {
      w = xp->left;
      if (w && w->color == friendsRbTreeRed) {
        w->color = friendsRbTreeBlack;
        xp->color = friendsRbTreeRed;
        root = friends_RbTreeRightRotate(root, xp);
        xp = x->parent;
        w = xp->left;
      }
      if (w &&
          (!w->left || w->left->color == friendsRbTreeBlack) &&
          (!w->right || w->right->color == friendsRbTreeBlack)) {
        w->color = friendsRbTreeRed;
        x = xp;
        xp = x->parent;
      } else {
        if (w && (!w->left || w->left->color == friendsRbTreeBlack)) {
          w->color = friendsRbTreeRed;
          root = friends_RbTreeLeftRotate(root, w);
          xp = x->parent;
          w = xp->left;
        }
        if (w) w->color = xp->color;
        xp->color = friendsRbTreeBlack;
        root = friends_RbTreeRightRotate(root, xp);
        x = root;
      }
    }
  }
  x->color = friendsRbTreeBlack;
  return root;
}

static inline friendsRbTree *
friendsRbTreeDelete(friendsRbTree *root, friendsRbTree *delp)
{
  friendsRbTree *y;
  friendsRbTree *x;
  enum friendsRbTreeColor yoc;

  y = delp;
  yoc = y->color;
  if (!delp->left) {
    x = delp->right;
    root = friends_RbTreeTransplant(root, delp, x);
  } else if (!delp->right) {
    x = delp->left;
    root = friends_RbTreeTransplant(root, delp, x);
  } else {
    y = friendsRbTreeMinimum(delp->right);
    yoc = y->color;
    x = y->right;
    if (y->parent == delp) {
      if (x) x->parent = y;
    } else {
      root = friends_RbTreeTransplant(root, y, y->right);
      y->right = delp->right;
      y->right->parent = y;
    }
    root = friends_RbTreeTransplant(root, delp, y);
    y->left = delp->left;
    y->left->parent = y;
    y->color = delp->color;
  }
  if (x && yoc == friendsRbTreeBlack) {
    root = friends_RbTreeDeleteFix(root, x);
  }
  friendsRbTreeInit(delp);
  return root;
}

static inline friendsRbTree *
friendsRbTreeFind(friendsRbTree *root, friendsRbTree *find,
                  friendsRbTreeCompare *comp)
{
  friendsRbTree *t;
  int i;

  t = root;
  while (t) {
    i = comp(find, t);
    if (i == 0) {
      return t;
    } else if (i > 0) {
      t = t->left;
    } else {
      t = t->right;
    }
  }
  return NULL;
}

#endif
