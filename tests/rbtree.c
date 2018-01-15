
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_rbtree.h"
#include "friends_list.h"

struct rbtree_test
{
  friendsRbTree t;
  int data;
};
#define test_entry(ptr) friendsRbTreeContainer(ptr, struct rbtree_test, t)

int test_comp(friendsRbTree *x, friendsRbTree *y)
{
  struct rbtree_test *xx;
  struct rbtree_test *yy;
  xx = test_entry(x);
  yy = test_entry(y);
  return xx->data - yy->data;
}

struct print_stack
{
  friendsList l;
  int after_l;
  friendsRbTree *tree;
};
#define print_stk_entry(ptr) friendsListContainer(ptr, struct print_stack, l)

void print_tree(struct print_stack *stk, friendsRbTree *tree,
                friendsBool printNils)
{
  struct print_stack *head;
  struct print_stack lstk;
  struct print_stack *ep;
  friendsList *lp;
  struct rbtree_test *tp;
  char cc;
  static const char *lloop = "[Link loop detected!]";
  static const char *invp  = "[Invalid parent!]";

  friendsListInit(&lstk.l);
  lstk.after_l = 0;
  lstk.tree = tree;
  if (stk) {
    if (stk->tree == tree) {
      tp = test_entry(tree);
      printf(" ,-[-] %d: %s\n", tp->data, lloop);
      return;
    }
    for (lp = stk->l.next; lp != &stk->l; lp = lp->next) {
      struct print_stack *p;
      p = print_stk_entry(lp);
      if (p->tree == tree) {
        for (lp = &stk->l; lp != &p->l; lp = lp->next) {
          struct print_stack *pp;
          pp = print_stk_entry(lp);
          if (pp->after_l) {
            printf(" | ");
          } else {
            printf("   ");
          }
        }
        if (p->after_l) {
          printf(" | ");
        } else {
          printf("   ");
        }
        tp = test_entry(tree);
        printf(" ,-[-] %d: %s\n", tp->data, lloop);
        return;
      }
    }
  }

  if (tree) {
    if (stk) {
      head = stk;
      friendsListInsertPrev(&head->l, &lstk.l);
    } else {
      head = &lstk;
    }
    print_tree(head, tree->left, printNils);
    if (stk) {
      friendsListDelete(&lstk.l);
    }
  }

  ep = stk;
  if (stk) {
    lp = stk->l.prev;
    if (lp == &stk->l) {
      lp = &stk->l;
      ep = print_stk_entry(lp);
    } else {
      ep = print_stk_entry(lp);
    }
  }
  if (tree || printNils == friendsTrue || !ep) {
    if (ep) {
      for (lp = &stk->l; lp != &ep->l; lp = lp->next) {
        struct print_stack *p;
        p = print_stk_entry(lp);
        if (p->after_l) {
          printf(" | ");
        } else {
          printf("   ");
        }
      }
    }
    if (ep) {
      if (ep->after_l) {
        printf(" `-");
      } else {
        printf(" ,-");
      }
    }
    if (tree) {
      switch(tree->color) {
      case friendsRbTreeRed:
        cc = 'R';
        break;
      case friendsRbTreeBlack:
        cc = 'B';
        break;
      default:
        cc = '?';
        break;
      }
      tp = test_entry(tree);
      printf("[%c] %d%s%s\n", cc, tp->data,
             (ep && tree->parent != ep->tree) ? ": " : "",
             (ep && tree->parent != ep->tree) ? invp : "");
    } else {
      printf("[B] nil\n");
    }
  }
  if (ep) {
    if (ep->after_l) {
      ep->after_l = 0;
    } else {
      ep->after_l = 1;
    }
  }
  if (tree) {
    if (stk) {
      head = stk;
      friendsListInsertPrev(&head->l, &lstk.l);
    } else {
      head = &lstk;
    }
    print_tree(head, tree->right, printNils);
    if (stk) {
      friendsListDelete(&lstk.l);
    }
  }
}

int main(int argc, char **argv)
{
  struct rbtree_test data[1000];
  friendsRbTree *root;
  struct rbtree_test *p;
  int ret;
  int i, j;

  ret = EXIT_SUCCESS;
  i = 0;

  for (j = 0; j < 1000; ++j) {
    data[j].data = j;
  }

  root = friendsRbTreeInsert(NULL, &data[100].t, test_comp);
  root = friendsRbTreeInsert(root, &data[  2].t, test_comp);
  root = friendsRbTreeInsert(root, &data[  6].t, test_comp);
  root = friendsRbTreeInsert(root, &data[ 11].t, test_comp);
  root = friendsRbTreeInsert(root, &data[ 99].t, test_comp);
  friendsRbTreeInsert(root, &data[  2].t, test_comp);
  print_tree(NULL, root, friendsTrue);

  root = friendsRbTreeDelete(root, &data[ 11].t);
  printf("-------------\n");
  print_tree(NULL, root, friendsTrue);

  friendsRbTreeInsert(root, &data[100].t, test_comp);
  root = friendsRbTreeInsert(root, &data[ 22].t, test_comp);
  root = friendsRbTreeInsert(root, &data[699].t, test_comp);
  printf("-------------\n");
  print_tree(NULL, root, friendsFalse);

  for (j = 50; j < 800; ++j) {
    friendsRbTree *nr;
    nr = friendsRbTreeInsert(root, &data[j].t, test_comp);
    if (nr) root = nr;
  }

  p = test_entry(root);
  printf("--\nroot: %d\n", p->data); /* 303 */
  root = friendsRbTreeDelete(root, root);
  root = friendsRbTreeDelete(root, &data[584].t);
  root = friendsRbTreeDelete(root, &data[ 93].t);
  root = friendsRbTreeDelete(root, &data[ 92].t);
  root = friendsRbTreeDelete(root, &data[103].t);
  for (j = 110; j < 535; j += 3) {
    root = friendsRbTreeDelete(root, &data[j].t);
  }
  root = friendsRbTreeInsert(root, &data[224].t, test_comp);
  root = friendsRbTreeDelete(root, &data[226].t);
  root = friendsRbTreeDelete(root, &data[224].t);
  root = friendsRbTreeInsert(root, &data[226].t, test_comp);
  root = friendsRbTreeInsert(root, &data[303].t, test_comp);
  for (j = 111; j < 800; j += 3) {
    root = friendsRbTreeDelete(root, &data[j].t);
  }
  root = friendsRbTreeDelete(root, &data[109].t);
  root = friendsRbTreeDelete(root, &data[775].t);
  printf("-------------\n");
  print_tree(NULL, root, friendsFalse);

  /* print_tree(NULL, root, friendsFalse); */

  root = friendsRbTreeInsert(NULL, &data[41].t, test_comp);
  root = friendsRbTreeInsert(root, &data[38].t, test_comp);
  root = friendsRbTreeInsert(root, &data[31].t, test_comp);
  root = friendsRbTreeInsert(root, &data[12].t, test_comp);
  root = friendsRbTreeInsert(root, &data[19].t, test_comp);
  root = friendsRbTreeInsert(root, &data[ 8].t, test_comp);
  printf("-------------\n");
  print_tree(NULL, root, friendsTrue);

  root = friendsRbTreeDelete(root, &data[ 8].t);
  printf("-------------\n");
  print_tree(NULL, root, friendsTrue);

  root = friendsRbTreeDelete(root, &data[12].t);
  root = friendsRbTreeDelete(root, &data[19].t);
  root = friendsRbTreeDelete(root, &data[31].t);
  root = friendsRbTreeDelete(root, &data[38].t);
  root = friendsRbTreeDelete(root, &data[41].t);
  printf("-------------\n");
  print_tree(NULL, root, friendsTrue);

  friendsRbTreeInit(&data[811].t);
  root = &data[811].t;
  printf("-------------\n");
  print_tree(NULL, root, friendsTrue);

  printf("-------------\n");
  print_tree(NULL, NULL, friendsTrue);

 clean:
  if (i > 0) ret = EXIT_FAILURE;
  return ret;

 error:
  ret = EXIT_FAILURE;
  goto clean;
}
