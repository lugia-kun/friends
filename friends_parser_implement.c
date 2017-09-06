
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_parser_implement.h"
#include "friends_error.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_atom.h"
#include "friends_list.h"
#include "friends_variable.h"
#include "friends_proposition.h"
#include "friends_token.h"
#include "friends_string.h"
#include "friends_park.h"

/**
 * @brief リストでなければ、リストを作って格納するのです。
 * @param list NULL でなければ、ここに取得したか作成したリストを設定す
 *        るのです。
 * @param data 確かめるデータをよこすのです。
 * @param park パークをよこすのです。
 * @param err NULL でなければ、エラーの情報を書き込むのです。
 * @return 作ったデータ、ないしは、data をそのまま返すのです。失敗した
 *         時は、NULL を返すのです。
 */
static friendsData *friendsParserEnsureList(friendsDataList **list,
                                            friendsData *data,
                                            friendsPark *park,
                                            friendsError *err)
{
  friendsDataList *ll, *lp;
  friendsData *ld;
  friendsData *td;

  friendsAssert(data);
  friendsAssert(park);
  friendsAssert(friendsGetPark(data) == park);

  ld = NULL;
  ll = friendsGetList(data);
  if (ll) {
    ld = data;
  } else {
    ld = friendsNewData(park, err);
    ll = friendsNewList(err);
    td = NULL;
    lp = NULL;
    if (ll && ld) {
      td = friendsSetList(ld, ll, err);
      lp = friendsListAppend(ll, data, err);
    }
    if (!(td && lp)) {
      if (ld) friendsDeleteData(ld);
      if (ll) friendsDeleteList(ll);
      return NULL;
    }
  }

  if (list) *list = ll;
  return ld;
}

static const friendsChar *friendsParserGetToken(friendsData *token_data)
{
  const friendsChar *cc;
  const friendsTokenData *td;

  td = friendsGetToken(token_data);
  friendsAssert(td);

  cc = friendsTokenText(td);
  friendsAssert(cc);

  return cc;
}

friendsData *friendsParserAddDataToTerm(friendsData *term, friendsData *data,
                                        friendsPark *park, friendsError *err)
{
  friendsDataList *ll;
  friendsData *ld;

  friendsAssert(park);

  if (!(term && data)) return NULL;

  ld = friendsParserEnsureList(&ll, term, park, err);
  if (!ld) return NULL;

  ll = friendsListAppend(ll, data, err);
  if (!ll) return NULL;

  return ld;
}

friendsData *friendsParserCreateAtom(friendsPark *park,
                                     friendsAtomType type,
                                     friendsData *token_data,
                                     friendsError *err)
{
  const friendsChar *cc;
  const friendsChar *ep;
  friendsChar *ch;
  friendsData *d, *ret;
  size_t n;
  long int ld;
  int id;

  friendsAssert(park);

  if (token_data) {
    cc = friendsParserGetToken(token_data);
  }

  d = friendsNewData(park, err);
  if (!d) {
    return NULL;
  }

  ret = NULL;
  switch(type) {
  case friendsNumericAtom:
    friendsAssert(cc);

    ld = friendsStringToLong(cc, &ep, 10, NULL);
    if (ep && *ep == 0) {
      id = (int)ld;

      if (id >= 0 && id == ld) {
        ret = friendsSetNumeralAtom(d, ld, err);
        break;
      }
    }

  case friendsTextAtom:
    friendsAssert(cc);
    n = friendsStringDuplicate(&ch, cc, NULL, err);
    if (n == (size_t)-1) {
      ret = NULL;
      break;
    }

    ret = friendsSetTextAtom(d, ch, err);
    if (!ret) {
      free(ch);
    }
    break;

  case friendsNextAtom:
    ret = friendsSetNextAtom(d, err);
    break;

  default:
    friendsUnreachable();
  }
  if (!ret) {
    friendsDeleteData(d);
    return NULL;
  }
  return d;
}

friendsData *friendsParserCreateVariable(friendsPark *park,
                                         friendsData *token_data,
                                         friendsBool tail,
                                         friendsError *err)
{
  const friendsChar *cc;
  const friendsChar *ep;
  friendsChar *ch;
  friendsData *d, *ret;
  size_t n;

  friendsAssert(park);
  friendsAssert(token_data);

  cc = friendsParserGetToken(token_data);

  n = friendsStringDuplicate(&ch, cc, NULL, err);
  if (n == (size_t)-1) {
    return NULL;
  }

  d = friendsNewData(park, err);
  if (!d) {
    return NULL;
  }

  ret = friendsSetVariable(d, ch, tail, err);
  if (!ret) {
    free(ch);
    friendsDeleteData(d);
  }

  return ret;
}


friendsData *friendsParserCreateProposition(friendsPark *park,
                                            friendsDataList *conds,
                                            friendsDataList *args,
                                            friendsData *verb_data,
                                            friendsPropositionMode mode,
                                            friendsError *err)
{
  const friendsChar *cc;
  friendsChar *ch;
  friendsData *d, *ret;
  size_t n;

  friendsAssert(park);
  friendsAssert(args);
  friendsAssert(verb_data);

  cc = friendsParserGetToken(verb_data);

  n = friendsStringDuplicate(&ch, cc, NULL, err);
  if (n == (size_t)-1) return NULL;

  d = friendsNewData(park, err);
  if (!d) {
    free(ch);
    return NULL;
  }

  ret = friendsSetProposition(d, ch, args, conds, mode, err);
  if (!ret) {
    free(ch);
    friendsDeleteData(d);
  }
  return d;
}

void friendsParserInsertProposition(friendsDataList *parsed_data,
                                    friendsPark *park,
                                    friendsDataList *conds,
                                    friendsDataList *args,
                                    friendsData *verb_data,
                                    friendsPropositionMode mode,
                                    friendsError *err)
{
  friendsDataList *l;
  friendsData *prop;

  friendsAssert(parsed_data);
  friendsAssert(park);

  if (!verb_data || !args) goto error;

  prop = friendsParserCreateProposition(park, conds, args, verb_data,
                                        mode, err);
  if (!prop) goto error;

  l = friendsListAppend(parsed_data, prop, err);
  if (!l) goto error;

  return;

 error:
  friendsDeleteList(conds);
  friendsDeleteList(args);

  return;
}


friendsData *friendsParserCreateListTerm(friendsPark *park,
                                         friendsDataList *list,
                                         friendsData *add_data,
                                         friendsError *err)
{
  friendsData *d, *ret;
  friendsDataList *lp;

  friendsAssert(park);
  friendsAssert(list);

  if (add_data) {
    lp = friendsListAppend(list, add_data, err);
    if (!lp) {
      return NULL;
    }
  }

  d = friendsNewData(park, err);
  if (!d) return NULL;

  ret = friendsSetList(d, list, err);
  if (!ret) {
    friendsDeleteData(d);
    return NULL;
  }

  return d;
}
