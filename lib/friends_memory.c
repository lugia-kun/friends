
#include "friends_defs.h"
#include "friends_rbtree.h"
#include "friends_list.h"
#include "friends_memory.h"
#include "friends_core.h"
#include "friends_error.h"

struct friendsMemoryNode
{
  friendsRbTree tree;
  void *ptr;
  friendsPointerDeleter *deleter;
};

struct friendsMemory
{
  struct friendsMemoryNode *root;
  struct friendsMemoryNode *delete_later_root;
};

#define friendsMemoryNodeTreeContainer(ptr) \
  friendsRbTreeContainer(ptr, struct friendsMemoryNode, tree)

static int friendsMemoryCompare(friendsRbTree *a, friendsRbTree *b)
{
  struct friendsMemoryNode *na, *nb;
  na = friendsMemoryNodeTreeContainer(a);
  nb = friendsMemoryNodeTreeContainer(b);
  return nb->ptr - na->ptr;
}

friendsMemory *friendsNewMemory(friendsError *e)
{
  friendsMemory *p;
  p = (friendsMemory *)friendsMalloc(sizeof(friendsMemory), e);
  if (!p) return NULL;

  p->root = NULL;
  p->delete_later_root = NULL;
  return p;
}

void friendsDeleteMemory(friendsMemory *pool)
{
  struct friendsMemoryNode *n;
  friendsRbTree *t;

  if (!pool) return;
  if (!pool->root) return;

  t = &pool->root->tree;
  while (t) {
    n = friendsMemoryNodeTreeContainer(t);
    n->deleter(n->ptr);
    t = friendsRbTreeDelete(t, t);
    friendsFree(n);
  }
  friendsFree(pool);
}

void *
friendsMemoryAddPointer(friendsMemory *pool, void *ptr,
                        friendsPointerDeleter *ptrdel, friendsError *e)
{
  friendsRbTree *tree;
  struct friendsMemoryNode *n;

  friendsAssert(pool);
  friendsAssert(ptrdel);
  friendsAssert(ptr);

  n = (struct friendsMemoryNode *)
    friendsMalloc(sizeof(struct friendsMemoryNode), e);
  if (!n) return NULL;

  n->ptr = ptr;
  n->deleter = ptrdel;

  tree = NULL;
  if (pool->root) {
    tree = &pool->root->tree;
  }

  tree = friendsRbTreeInsert(tree, &n->tree, friendsMemoryCompare);
  if (tree) {
    pool->root = friendsMemoryNodeTreeContainer(tree);
  } else {
    friendsSetError(e, MemoryHasPointer);
    friendsFree(n);
    return NULL;
  }
  return ptr;
}

friendsBool friendsMemoryHasPointer(friendsMemory *pool, void *ptr)
{
  friendsRbTree *p;
  struct friendsMemoryNode n;

  friendsAssert(pool);
  if (!pool->root) return friendsFalse;

  n.ptr = ptr;
  p = friendsRbTreeFind(&pool->root->tree, &n.tree, friendsMemoryCompare);
  if (p) {
    return friendsTrue;
  } else {
    return friendsFalse;
  }
}

void *friendsMemoryUnregister(friendsMemory *pool, void *ptr)
{
  struct friendsMemoryNode n, *pn;
  friendsRbTree *p;

  friendsAssert(pool);
  if (!pool->root) return NULL;

  n.ptr = ptr;
  p = friendsRbTreeFind(&pool->root->tree, &n.tree, friendsMemoryCompare);
  if (!p) {
    return NULL;
  }
  pn = friendsMemoryNodeTreeContainer(p);
  p = friendsRbTreeDelete(&pool->root->tree, p);
  friendsFrree(pn);
  pn = friendsMemoryNodeTreeContainer(p);
  pool->root = pn;
  return ptr;
}

void *friendsMemoryDeletePointer(friendsMemory *pool, void *ptr)
{
  struct friendsMemoryNode n, *pn;
  friendsRbTree *p;

  friendsAssert(pool);
  if (!pool->root) return ptr;

  n.ptr = ptr;
  p = friendsRbTreeFind(&pool->root->tree, &n.tree, friendsMemoryCompare);
  if (!p) {
    return ptr;
  }
  pn = friendsMemoryNodeTreeContainer(p);
  pn->deleter(pn->ptr);
  p = friendsRbTreeDelete(&pool->root->tree, p);
  friendsFree(pn);
  pn = friendsMemoryNodeTreeContainer(p);
  pool->root = pn;
  return NULL;
}
