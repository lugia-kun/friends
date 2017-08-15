
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "friends_error.h"
#include "friends_io.h"
#include "friends_list.h"
#include "friends_park.h"
#include "friends_data.h"
#include "friends_test.h"
#include "friends_atom.h"

friendsBool listPrinter(friendsDataList *l, void *a)
{
  const friendsAtomData *at;
  int n;
  const friendsChar *s;
  friendsHash h;

  friendsUnUsed(a);

  friendsData *d;

  d = friendsListData(l);

  if (!d) {
    friendsPrintCF(stdout, NULL, "<null> \n");
    return friendsTrue;
  }

  at = friendsGetAtom(d);
  if (!at) {
    friendsPrintCF(stdout, NULL, "<n/a>  \n");
    return friendsTrue;
  }

  h = friendsGetHash(d);
  s = friendsGetAtomText(at, NULL);
  if (s) {
    friendsPrintCF(stdout, NULL, "<text> %ls\n", s);
  } else {
    n = friendsGetAtomNumber(at, NULL);
    friendsPrintCF(stdout, NULL, "<num>  %d\n", n);
  }
  return friendsTrue;
}

void printBorder(void)
{
  friendsPrintCF(stdout, NULL, "---------------------\n");
}

void printList(friendsDataList *l)
{
  printBorder();
  friendsListEachItem(l, listPrinter, NULL);
}

friendsData *newData(friendsPark *park, friendsAtomType t,
                     int numeral, const char *txt, friendsError *e, ...)
{
  va_list ap;
  friendsData *d;
  friendsError ee;
  friendsChar *ftxt;
  if (!e) {
    ee = friendsNoError;
    e = &ee;
  }

  d = friendsNewData(park, e);
  if (!d) return NULL;

  switch(t) {
  case friendsNumricAtom:
    friendsSetNumeralAtom(d, numeral, e);
    if (friendsAnyError(*e)) return NULL;
    break;
  case friendsTextAtom:
    va_start(ap, e);
    friendsAsprintCV(&ftxt, e, txt, ap);
    va_end(ap);
    if (friendsAnyError(*e)) return NULL;
    friendsSetTextAtom(d, ftxt, e);
    if (friendsAnyError(*e)) { free(ftxt); return NULL; }
    break;
  default:
    return NULL;
  }

  return d;
}

int main(int argc, char **argv)
{
#define if_error_goto_error(func)                                       \
  do {                                                                  \
    if (friendsAnyError(e)) {                                           \
      friendsPrintError(friendsErrorLevelFatal, __FILE__, 0, 0, func, e); \
      goto error;                                                       \
    }                                                                   \
  } while(0)

  friendsPark *park;
  friendsData *d;
  friendsDataList *l, *p;
  friendsError e;
  friendsChar *ftxt;
  const friendsChar *ctxt;
  int ret;
  int i;

  l = NULL;
  p = NULL;
  d = NULL;
  ret = 0;
  e = friendsNoError;

  park = friendsNewPark(&e);
  if_error_goto_error("friendsNewPark");

  l = friendsNewList(&e);
  if_error_goto_error("friendsNewList");

  for (i = 0; i < 10; ++i) {
    d = newData(park, friendsNumricAtom, i, NULL, &e);
    if_error_goto_error("newData");

    friendsListAppend(l, d, &e);
    if_error_goto_error("friendsListAppend");
  }

  printList(l);

  for (i = 0; i < 2; ++i) {
    l = friendsListNext(l);
  }
  for (i = 0; i < 2; ++i) {
    d = newData(park, friendsTextAtom, 0,
                "\\u3059\\u3054\\u30fc\\u3044 (%d)"  /* すごーい */,
                &e, i);
    if_error_goto_error("newData");

    friendsListInsert(l, d, &e);
    if_error_goto_error("friendsListInsert");
  }

  printList(l);

  l = friendsListParent(l);

  printBorder();
  if (friendsListIsParent(l) == friendsTrue) {
    friendsPrintCF(stdout, NULL, "l is parent\n");
  } else {
    friendsTestExpectCS(l, 0,
                        "\\u5b50" /* 子 */,
                        "\\u89aa" /* 親 */, friendsTestTrue);
  }

  d = newData(park, friendsTextAtom, 0,
              "\\u305f\\u30fc\\u306e\\u3057\\u30fc"  /* たーのしー */, &e);
  if_error_goto_error("newData");

  friendsListInsert(l, d, &e);
  if_error_goto_error("friendsListInsert");

  printList(l);

  for(i = 0; i < 9; ++i) {
    l = friendsListNext(l);
  }
  if (!l) {
    friendsPrintErrorF(friendsErrorLevelError, __FILE__, 0, 0,
                       "\\u8981\\u7d20\\u6570\\u304c5"        /* 要素数が3 */
                       "\\u3064\\u3088\\u308a\\u5c0f\\u3055"  /* つより小さ */
                       "\\u3044\\u306e\\u3067\\u3059"         /* いのです */);
    goto error;
  }

  d = newData(park, friendsTextAtom, 0,
              "\\u3042\\u306a\\u305f\\u306f\\u30e4"  /* あなたはヤ */
              "\\u30ae\\u306d\\uff01"                /* ギね！ */, &e);
  if_error_goto_error("newData");

  friendsListInsert(l, d, &e);
  if_error_goto_error("friendsListInsert");

  printList(l);

  l = friendsListPrev(l); /* あなたはヤギね！ */

  friendsListRemoveAt(l, -2, &e); /* Remove 4 */
  if_error_goto_error("friendsListRemoveAt");

  printList(l);

  friendsListRemoveAt(l, 2, &e); /* Remeve 6 */
  if_error_goto_error("friendsListRemoveAt");

  printList(l);

  p = friendsListParent(l);
  friendsListRemoveAt(l, 0, &e); /* Remove ヤギ */
  if_error_goto_error("friendsListRemoveAt");

  printList(p);
  l = p;

  friendsListRemoveAt(l, 1, &e); /* Remove 0 */
  if_error_goto_error("friendsListRemoveAt");

  friendsListRemoveAt(l, -1, &e); /* Remove 9 */
  if_error_goto_error("friendsListRemoveAt");

  printList(l);

  l = friendsNewList(&e);
  if_error_goto_error("friendsListRemoveAt");

  friendsDeleteList(l);

  l = p;

  friendsListAppend(l, d, &e); /* Append ヤギ */
  if_error_goto_error("friendsListAppend");

  printList(l);

  l = friendsListBegin(l);
  d = friendsListData(l);
  ctxt = friendsDataToText(d);

  friendsTestExpectText(ctxt, "\\u305f\\u30fc\\u306e\\u3057\\u30fc");
  /* たーのしー */

  friendsListAppend(l, NULL, &e);
  printList(l);

  l = friendsListEnd(l);
  friendsListRemoveAt(l, 2, &e);
  friendsTestExpect(e, friendsErrorListIndex, "%d", friendsTestEqual);

  e = friendsNoError;
  friendsSetListData(l, d);
  printList(l);

 clean:
  if (p && p != friendsListParent(l)) friendsDeleteList(p);
  if (l)    friendsDeleteList(l);
  if (park) friendsDeletePark(park);
  i = friendsTestErrorCount();
  if (ret < i) ret = i;
  return ret;

 error:
  ret = 2;
  goto clean;
}
