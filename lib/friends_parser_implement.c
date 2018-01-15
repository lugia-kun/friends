
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
#include "friends_grammer.h"

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
static friendsData *
friendsParserEnsureList(friendsDataList **list,
                        friendsData *data, friendsPark *park,
                        friendsError *err)
{
  friendsDataList *ll, *lp;
  friendsData *ld;
  friendsData *td;

  friendsAssert(data);
  friendsAssert(park);
  friendsAssert(friendsGetPark(data) == park);

  ld = NULL;
  ll = friendsGetList(data, err);
  if (ll) {
    ld = data;
  } else {
    ld = friendsNewData(park, err);
    ll = friendsNewDataList(err);
    td = NULL;
    lp = NULL;
    if (ll && ld) {
      lp = friendsDataListAppend(ll, data, err);
      td = friendsSetList(ld, ll, err);
      friendsDataListDeleteAll(ll);
      ll = friendsGetList(ld, NULL);
    }
    if (!(td && lp)) {
      if (ld) friendsDeleteData(ld);
      if (list) *list = NULL;
      return NULL;
    }
  }

  if (list) *list = ll;
  return ld;
}

friendsData *
friendsParserAddDataToTerm(friendsData *term, friendsData *data,
                           friendsPark *park, friendsError *err)
{
  friendsDataList *ll;
  friendsData *ld;

  friendsAssert(park);

  if (!(term && data)) return NULL;

  ld = friendsParserEnsureList(&ll, term, park, err);
  if (!ld) return NULL;

  ll = friendsDataListAppend(ll, data, err);
  if (!ll) return NULL;

  return ld;
}

friendsData *
friendsParserCreateAtom(friendsPark *park, friendsAtomType type,
                        friendsToken *token_data, friendsError *err)
{
  const friendsChar *cc;
  const friendsChar *ep;
  friendsData *d, *ret;
  long int ld;
  int id;

  friendsAssert(park);

  cc = NULL;
  if (token_data) {
    cc = friendsTokenText(token_data);
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
    ret = friendsSetTextAtom(d, cc, err);
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

friendsData *
friendsParserCreateVariable(friendsPark *park, friendsToken *token_data,
                            friendsBool tail, friendsError *err)
{
  const friendsChar *cc;
  friendsData *d, *ret;

  friendsAssert(park);
  friendsAssert(token_data);

  cc = friendsTokenText(token_data);

  d = friendsNewData(park, err);
  if (!d) {
    return NULL;
  }

  ret = friendsSetVariable(d, cc, tail, err);
  if (!ret) {
    friendsDeleteData(d);
  }

  return ret;
}


friendsData *
friendsParserCreateProposition(friendsPark *park,
                               friendsDataList *conds, friendsDataList *args,
                               friendsToken *verb_token,
                               friendsPropositionMode mode,
                               friendsError *err)
{
  const friendsChar *cc;
  friendsData *d, *ret;

  friendsAssert(park);
  friendsAssert(args);
  friendsAssert(verb_token);

  cc = friendsTokenText(verb_token);

  d = friendsNewData(park, err);
  if (!d) {
    return NULL;
  }

  ret = friendsSetProposition(d, cc, args, conds, mode, err);
  if (!ret) {
    friendsDeleteData(d);
  }
  return d;
}

void friendsParserInsertProposition(friendsDataList *parsed_data,
                                    friendsPark *park,
                                    friendsDataList *conds,
                                    friendsDataList *args,
                                    friendsToken *verb_data,
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

  l = friendsDataListAppend(parsed_data, prop, err);
  if (!l) goto error;

  return;

 error:
  friendsDataListDeleteAll(conds);
  friendsDataListDeleteAll(args);

  return;
}


friendsData *
friendsParserCreateListTerm(friendsPark *park, friendsDataList *list,
                            friendsData *add_data, friendsError *err)
{
  friendsData *d, *ret;
  friendsDataList *lp;

  friendsAssert(park);
  friendsAssert(list);

  if (add_data) {
    lp = friendsDataListAppend(list, add_data, err);
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

void friendsPrintSyntaxError(friendsParser *parser, friendsToken *token,
                             int yytoken, const char *yyTokenName)
{
  const char *s, *e;

  friendsAssert(parser);

  e = "\\u3092"  /* を */;
  if (token) {
    switch (friendsTokenType(token)) {
    case friendsVARIABLE:
      s = "\\u5909\\u6570\\u306e"  /* 変数の */;
      break;
    case friendsATOM:
    case friendsTSUGI:
      s = "\\u30a2\\u30c8\\u30e0\\u306e"  /* アトムの */;
      break;
    case friendsFRIEND_NAME:
      s = "\\u8ff0\\u8a9e\\u306e"  /* 述語の */;
      break;
    case friendsPARTICLE:
    case friendsNO:
      s = "\\u52a9\\u8a5e\\u306e"  /* 助詞の */;
      break;
    case friendsAND:
    case friendsTHEN:
      s = "\\u63a5\\u7d9a\\u8a5e\\u306e"  /* 接続詞の */;
      break;
    case friendsLKAGI:
    case friendsRKAGI:
      s = "";
      break;
    default:
      // s = "\\u30b5\\u30fc\\u30d0\\u30eb\\u304c"  /* サーバルが */;
      s = "";
      e = "\\u3068"  /* と */;
      break;
    }

    friendsPrintErrorF(friendsErrorLevelError, parser->filename,
                       0, 0,
                       "%d \\u884c\\u76ee\\u306e "  /* %d 行目の  */
                       "%d \\u6587\\u5b57\\u76ee\\u3067"  /* %d 文字目で */
                       "\\u306f%s\\u300e%ls\\u300f"  /* は%s『%ls』 */
                       "%s\\u8a00\\u3046\\u3053\\u3068"  /* %s言うこと */
                       "\\u306f\\u3067\\u304d\\u306a"  /* はできな */
                       "\\u3044\\u306e\\u3067\\u3059"  /* いのです */
                       "\\u3002"  /* 。 */,
                       friendsTokenLinePosition(token),
                       friendsTokenColumnPosition(token),
                       s, friendsTokenText(token), e);
  }
}
