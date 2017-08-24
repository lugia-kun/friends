
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_common.h"

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_park.h"
#include "friends_list.h"
#include "friends_set.h"
#include "friends_data.h"
#include "friends_atom.h"
#include "friends_string.h"
#include "friends_test.h"

typedef struct
{
  friendsBool set_first;
  friendsBool list_first;
} listPrinterController;

void printListAtom(listPrinterController *x,
                   friendsDataList *l,
                   const char *format, ...)
{
  va_list ap;
  friendsChar *tmp;
  friendsError e;

  e = friendsNoError;

  va_start(ap, format);
  friendsAsprintCV(&tmp, &e, format, ap);
  va_end(ap);

  if (friendsAnyError(e)) {
    friendsPrintError(friendsErrorLevelError, FRIENDS_SOURCE_FILE, __LINE__,
                      0, "printListAtom", e);
    return;
  }

  friendsPrintCF(stdout, NULL,
                 "%s%s- %ls\n",
                 (x->list_first == friendsTrue) ? "+" : "|",
                 (x->list_first == friendsTrue) ? "-" :
                 ((l == friendsListEnd(l)) ? "`" : "|"),
                 tmp);

  free(tmp);
}

friendsBool listPrinter(friendsDataList *l, void *a)
{
  const friendsAtomData *ad;
  friendsData *d;
  const friendsChar *ch;
  friendsError e;
  friendsDataSetHash h;
  friendsHash hsh;
  friendsAtomType at;
  int n;
  listPrinterController *x = (listPrinterController *)a;

  d = friendsListData(l);
  h = 0;
  hsh = 0;
  if (d) {
    hsh = friendsGetHash(d);
    h = friendsSmallHash(hsh);
  }
  ad = friendsGetAtom(d);
  e = friendsNoError;
  if (ad) {
    at = friendsGetAtomType(ad);
    switch (at) {
    case friendsTextAtom:
      ch = friendsGetAtomText(ad, &e);
      printListAtom(x, l, "%02x %08x %p <text> %ls", h, hsh, d, ch);
      break;
    case friendsNumericAtom:
      e = friendsNoError;
      n = friendsGetAtomNumber(ad, &e);
      if (friendsAnyError(e)) {
        goto not_available;
      } else {
        printListAtom(x, l, "%02x %08x %p <int>  %d", h, hsh, d, n);
      }
      break;
    case friendsNextAtom:
      printListAtom(x, l, "%02x %08x %p <next>", h, hsh, d);
      break;
    not_available:
    default:
      printListAtom(x, l, "%02x %08x %p <n/a>", h, hsh, d);
    }
  } else {
    printListAtom(x, l, "%02x %08x %p <null>", h, hsh, d);
  }

  x->list_first = friendsFalse;
  return friendsTrue;
}

friendsBool setPrinter(friendsDataList *l, void *a)
{
  listPrinterController *x = (listPrinterController *)a;
  x->list_first = friendsTrue;
  friendsListEachItem(l, listPrinter, a);
  x->set_first = friendsFalse;
  return friendsTrue;
}

void printSet(friendsDataSet *set)
{
  listPrinterController x;
  x.list_first = friendsTrue;
  x.set_first  = friendsTrue;

  friendsPrintCF(stdout, NULL, "----------\n");
  friendsSetEach(set, setPrinter, &x);
}

int main(int argc, char **argv)
{
#define eerror(func) if_error_goto_error(e, func, error)
  friendsError e;
  friendsDataSet *s;
  friendsPark *park;
  friendsData *d;
  friendsChar *ch;
  const friendsChar *cch;
  const friendsAtomData *at;
  friendsDataList *l;
  int ret;
  int i;
  const char *animals[] = {
    "\\u30b5\\u30fc\\u30d0\\u30eb"  /* サーバル */,
    "\\u30ab\\u30d0"                /* カバ */,
    "\\u30aa\\u30fc\\u30ed\\u30c3\\u30af\\u30b9"  /* オーロックス */,
    "\\u30ea\\u30ab\\u30aa\\u30f3"         /* リカオン */,
    "\\u30ad\\u30f3\\u30b7\\u30b3\\u30a6"  /* キンシコウ */,
    "\\u30a2\\u30df\\u30e1\\u30ad\\u30ea\\u30f3"  /* アミメキリン */,
    "\\u30bf\\u30a4\\u30ea\\u30af\\u30aa"  /* タイリクオ */
    "\\u30aa\\u30ab\\u30df"                /* オカミ */,
    "\\u30a2\\u30d5\\u30ea\\u30ab\\u30bf"  /* アフリカタ */
    "\\u30c6\\u30ac\\u30df\\u30e4\\u30de"  /* テガミヤマ */
    "\\u30a2\\u30e9\\u30b7"                /* アラシ */,
    "\\u30d8\\u30e9\\u30b8\\u30ab"  /* ヘラジカ */,
    "\\u30d1\\u30f3\\u30b5\\u30fc\\u30ab"  /* パンサーカ */
    "\\u30e1\\u30ec\\u30aa\\u30f3"  /* メレオン */,
    "\\u30d2\\u30b0\\u30de"  /* ヒグマ */,
    "\\u30d5\\u30f3\\u30dc\\u30eb\\u30c8"  /* フンボルト */
    "\\u30da\\u30f3\\u30ae\\u30f3"  /* ペンギン */,
    "\\u30a2\\u30d5\\u30ea\\u30ab\\u30aa"  /* アフリカオ */
    "\\u30aa\\u30b3\\u30ce\\u30cf\\u30ba\\u30af"  /* オコノハズク */,
    "\\u30ef\\u30b7\\u30df\\u30df\\u30ba\\u30af"  /* ワシミミズク */
  };

  e = friendsNoError;
  ret = EXIT_SUCCESS;
  s = NULL;
  ch = NULL;
  park = friendsNewPark(&e);
  eerror("friendsNewPark");

  s = friendsNewSet(&e);
  eerror("friendsNewSet");

  for (i = 0; i < sizeof(animals) / sizeof(char *); ++i) {
    d = newData(park, friendsTextAtom, 0,
                animals[i], &e);
    eerror("newData");

    friendsSetInsert(s, d, &e);
    eerror("friendsSetInsert");
  }

  printSet(s);

  d = newData(park, friendsTextAtom, 0,
              "\\u30e9\\u30a4\\u30aa\\u30f3"  /* ライオン */,
              &e);
  eerror("newData");

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  d = newData(park, friendsTextAtom, 0,
              "\\u30b5\\u30fc\\u30d0\\u30eb\\u30ad"  /* サーバルキ */
              "\\u30e3\\u30c3\\u30c8"  /* ャット */,
              &e);
  eerror("newData");

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  d = newData(park, friendsTextAtom, 0,
              "\\u304b\\u3070\\u3093"  /* かばん */,
              &e);
  eerror("newData");

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  printSet(s);

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  d = newData(park, friendsTextAtom, 0,
              "\\u304b\\u3070\\u3093"  /* かばん */,
              &e);
  eerror("newData");

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  printSet(s);

  d = newData(park, friendsNextAtom, 0, NULL, &e);
  eerror("newData");

  friendsSetInsert(s, d, &e);
  eerror("friendsSetInsert");

  for (i = 0; i < 10; ++i) {
    d = newData(park, friendsNumericAtom, i, NULL, &e);
    eerror("newData");

    friendsSetInsert(s, d, &e);
    eerror("friendsSetInsert");
  }

  for (i = 10; i < 2000; i += 100) {
    d = newData(park, friendsNumericAtom, i, NULL, &e);
    eerror("newData");

    friendsSetInsert(s, d, &e);
    eerror("friendsSetInsert");
  }
  printSet(s);

  friendsUnescapeStringLiteral(&ch,
                               "\\u30b5\\u30fc\\u30d0\\u30eb"  /* サーバル */,
                               &e);
  eerror("friendsUnescapeStringliteral");

  l = friendsSetFindText(s, friendsAtom, ch);
  if (!l) {
    friendsTestExpectFS(l, NULL, NULL, ch, friendsTestTrue);
  } else {
    l = friendsListBegin(l);
    for (; l; l = friendsListNext(l)) {
      d = friendsListData(l);
      cch = friendsDataToText(d);
      friendsTestExpectText(cch,
                            "\\u30b5\\u30fc\\u30d0\\u30eb"  /* サーバル */);
    }
  }
  free(ch);
  ch = NULL;

  friendsUnescapeStringLiteral(&ch,
                               "\\u304b\\u3070\\u3093"  /* かばん */,
                               &e);
  eerror("friendsUnescapeStringliteral");

  l = friendsSetFindText(s, friendsAtom, ch);
  if (!l) {
    friendsTestExpectFS(l, NULL, NULL, ch, friendsTestTrue);
  } else {
    l = friendsListBegin(l);
    for (; l; l = friendsListNext(l)) {
      d = friendsListData(l);
      cch = friendsDataToText(d);
      friendsTestExpectText(cch,
                            "\\u304b\\u3070\\u3093"  /* かばん */);
    }
  }
  free(ch);
  ch = NULL;

  friendsUnescapeStringLiteral(&ch,
                               "\\u30ab\\u30d0\\u30f3"  /* カバン */,
                               &e);
  eerror("friendsUnescapeStringliteral");

  l = friendsSetFindText(s, friendsAtom, ch);
  if (l) {
    friendsTestExpectFS(l, NULL, ch, NULL, friendsTestTrue);
  }

  free(ch);
  ch = NULL;

  l = friendsSetFindNumericAtom(s, 0);
  if (!l) {
    friendsTestExpectCS(l, NULL, NULL, "0", friendsTestTrue);
  } else {
    l = friendsListBegin(l);
    for (; l; l = friendsListNext(l)) {
      d = friendsListData(l);
      at = friendsGetAtom(d);
      if (!at) {
        friendsTestExpectCS(at, NULL, NULL, "0", friendsTestTrue);
      } else {
        i = friendsGetAtomNumber(at, &e);
        eerror("friendsGetAtomNumber");
        friendsTestExpect(i, 0, "%d", friendsTestEqual);
      }
    }
  }

  l = friendsSetFindNumericAtom(s, 999);
  if (l) {
    friendsTestExpectCS(l, NULL, "999", NULL, friendsTestTrue);
  }

  friendsUnescapeStringLiteral(&ch,
                               "\\u30b5\\u30fc\\u30d0\\u30eb"  /* サーバル */,
                               &e);
  eerror("friendsUnescapeStringliteral");

  l = friendsSetFindText(s, friendsAtom, ch);
  if (!l) {
    friendsTestExpectFS(l, NULL, NULL, ch, friendsTestTrue);
  }

  l = friendsListBegin(l);
  d = friendsListData(l);
  if (!d) {
    friendsTestExpectFS(l, NULL, NULL, ch, friendsTestTrue);
  }
  free(ch);
  ch = NULL;

  friendsSetRemove(s, d, &e);
  eerror("friendsSetRemove");

  printSet(s);

  /* UTF-16 モードでは 8 は「サーバルキャット」と同じハッシュ値だったのです（経験則）。 */
  l = friendsSetFindNumericAtom(s, 8);
  if (!l) {
    friendsTestExpectCS(l, NULL, NULL, "8", friendsTestTrue);
  }

  l = friendsListBegin(l);
  d = friendsListData(l);
  if (!d) {
    friendsTestExpectCS(l, NULL, NULL, "8", friendsTestTrue);
  }

  friendsSetRemove(s, d, &e);
  eerror("friendsSetRemove");

  printSet(s);

  /* UTF-8 モードでは 3 は「かばん」と同じハッシュ値だったのです（経験則）。 */
  l = friendsSetFindNumericAtom(s, 3);
  if (!l) {
    friendsTestExpectCS(l, NULL, NULL, "3", friendsTestTrue);
  }

  l = friendsListBegin(l);
  d = friendsListData(l);
  if (!d) {
    friendsTestExpectCS(l, NULL, NULL, "3", friendsTestTrue);
  }

  friendsSetRemove(s, d, &e);
  eerror("friendsSetRemove");

  printSet(s);

  friendsUnescapeStringLiteral(&ch,
                               "\\u304b\\u3070\\u3093"  /* かばん */,
                               &e);
  eerror("friendsUnescapeStringliteral");

  l = friendsSetFindText(s, friendsAtom, ch);
  if (!l) {
    friendsTestExpectFS(l, NULL, NULL, ch, friendsTestTrue);
  }

  l = friendsListBegin(l);
  d = friendsListData(l);
  if (!d) {
    friendsTestExpectFS(l, NULL, NULL, ch, friendsTestTrue);
  }
  free(ch);
  ch = NULL;

  /* 1 つだけ消すのです。 */
  friendsSetRemove(s, d, &e);
  eerror("friendsSetRemove");

  printSet(s);

  friendsSetRemoveAll(s, d, &e);
  eerror("friendsSetRemoveAll");

  printSet(s);

 clean:
  if (ch)   free(ch);
  if (s)    friendsDeleteSet(s);
  if (park) friendsDeletePark(park);
  i = friendsTestErrorCount();
  if (i > 0) ret = EXIT_FAILURE;
  return ret;

 error:
  ret = EXIT_FAILURE;
  goto clean;
}
