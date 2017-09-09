/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "friends_data.h"
#include "friends_struct_data.h"
#include "friends_lexer.h"
#include "friends_error.h"
#include "friends_string.h"
#include "friends_grammer.h"
#include "friends_list.h"
#include "friends_token.h"

/*!re2c
  re2c:define:YYCTYPE = "friendsChar";
*/

static void friendsLexerFill(friendsParser *p, size_t reqs) {
  size_t uses;
  size_t c;
  const friendsChar *lexeme;
  const friendsChar *t;
  friendsChar *newp;
  friendsDataList *ep;

  lexeme = NULL;

  if (p->bufp) {
    friendsAssert(p->listcur);
    p->listcur = friendsListNext(p->listcur);

    lexeme = p->cur;
    if (p->token  && p->token < lexeme)  lexeme = p->token;
    if (p->stoken && p->stoken < lexeme) lexeme = p->stoken;
    if (p->mark   && p->mark < lexeme) lexeme = p->mark;
    if (p->cmark  && p->cmark < lexeme) lexeme = p->cmark;

    uses = friendsStringArrayLength(lexeme);

  } else {
    /* 初期化するのです */
    if (!p->listcur) {
      p->listcur = friendsListBegin(p->buffer_list);
    }

    uses = 0;
  }

  /* もうこれ以上読み込めるデータは無いのです。 */
  if (!p->listcur) return;

  ep = p->listcur;

  c = 0;
  for (; ep; ep = friendsListNext(ep)) {
    t = (const friendsChar *)friendsListData(ep);
    c += friendsStringArrayLength(t);
    if (c >= reqs) break;
  }

  reqs = uses;
  reqs += c;
  reqs += 1; /* For NUL. */
  if (reqs >= p->bufp_size) {
    newp = (friendsChar *)calloc(sizeof(friendsChar), reqs);
    if (!newp) {
      friendsSetErrorV(p->err, NOMEM);
      return;
    }
  } else {
    newp = p->bufp;
  }
  if (lexeme && lexeme != newp) {
    memmove(newp, lexeme, sizeof(friendsChar) * uses);
    uses = uses + 1;
  }
  while (1) {
    if (!p->listcur) break;
    t = (const friendsChar *)friendsListData(p->listcur);

    friendsCopyString(newp + uses, t);
    uses += friendsStringArrayLength(t);

    if (p->listcur == ep) break;
    p->listcur = friendsListNext(p->listcur);
  }
  newp[uses] = 0;

  p->limit = newp + uses;
  if (p->token)  p->token = newp + (p->token - lexeme);
  if (p->stoken) p->stoken = newp + (p->stoken - lexeme);
  if (p->mark)   p->mark = newp + (p->mark - lexeme);
  if (p->cmark)  p->cmark = newp + (p->cmark - lexeme);

  if (newp != p->bufp) {
    free(p->bufp);
  }
  p->bufp = newp;
  p->bufp_size = reqs;
}

static void friendsLexerAddToken(friendsParser *p, int type)
{
  size_t n;
  friendsChar *ch;
  friendsData *d, *ret;
  friendsDataList *l;

  friendsAssert(p);
  friendsAssert(p->park);
  friendsAssert(p->token);
  friendsAssert(p->cur);

  if (type >= 0) {
    p->last_type = type;
    d = friendsNewData(p->park, &p->err);
    if (!d) return;

    n = friendsStringDuplicate(&ch, p->token, p->cur, &p->err);
    if (n == (size_t)-1) return;

    ret = friendsSetToken(d, ch, type, p->tokpos.line, p->tokpos.column,
                          &p->err);
    if (!ret) {
      free(ch);
      return;
    }

    l = friendsListAppend(p->tokens, d, &p->err);
    if (!l) return;
  }
  p->token = NULL;
}

static void friendsLexerControlPos(friendsParser *p, friendsBool newline)
{
  const friendsChar *start;
  const friendsChar *YYCURSOR;
  size_t n;

  if (newline == friendsTrue) {
    while (YYCURSOR < p->cur) {
      /*!re2c
        re2c:yyfill:enable = 0;

        *    { continue; }
        "\n" { start = YYCURSOR; p->curpos.line += 1; continue; }
      */
      friendsUnreachable();
    }
    p->curpos.column = 0;
  } else {
    start = p->token;
  }
  n = friendsStringCharCount(start, p->cur, NULL);
  p->curpos.column += n;
}

static void friendsLexerSetTokenPos(friendsParser *p)
{
  p->tokpos = p->curpos;
}

#define AddToken(token)                         \
  friendsLexerAddToken(p, friends##token)

#define SkipToken()                             \
  friendsLexerAddToken(p, -1)

#define IncrementLine() \
  friendsLexerControlPos(p, friendsTrue)

#define IncrementPos() \
  friendsLexerControlPos(p, friendsFalse)

void friendsLex(friendsParser *p)
{
  int ttype = 0;

  friendsAssert(p);
  friendsAssert(p->buffer_list);

  if (!p->bufp) {
    friendsLexerFill(p, 1);
  }

  /* エラー復帰時は、トークンの場所から読み直すのです */
  if (p->token) {
    p->cur = p->token;
  }

  for(;;) {
    p->mark  = NULL;
    p->cmark = NULL;

    switch(p->lexer_state) {
    case friendsLexerNORMAL:     break;
    case friendsLexerUNDERSCORE: goto und;
    case friendsLexerDBRACKET:   goto Dcl_continue;
    case friendsLexerPAREN:      goto Pcl_continue;
    default: friendsUnreachable();
    }

    p->token = p->cur;
    friendsLexerSetTokenPos(p);

    /*!re2c
      re2c:define:YYCTYPE = "friendsChar";
      re2c:define:YYCURSOR = "p->cur";
      re2c:define:YYMARKER = "p->mark";
      re2c:define:YYCTXMARKER = "p->cmark";
      re2c:define:YYLIMIT = "p->limit";
      re2c:define:YYFILL:naked = 1;
      re2c:define:YYFILL = "{ friendsLexerFill(p, @@); }";
      re2c:yyfill:enable = 1;
      re2c:indent:string = "  ";
      re2c:indent:top = 2;

      utf8     = [\u0020-\uffef\U00010000-\U0010ffff];
      u_ascii  = [\u0022-\u0027\u002a\u002b\u002d-\u003e\u0040-\u007e];
      u_noncjk = [\u00a1-\u2fff\ua000-\uf8ff\ufb00-\ufb4f\ufe70-\ufeff];
      u_extra  = [\U00010000-\U0010ffff];
      u_jpunct = [\u3003-\u3007\u3012\u3013\u301c\u3020-\u303f];
      u_cjk    = [\u3040-\u9fff\uf900-\ufaff\uff65-\uffee];
      u_falpha = [\uff02-\uff07\uff0a\uff0b\uff0d-\uff1e\uff20-\uff60];
      atomchar = (u_ascii|u_noncjk|u_extra|u_jpunct|u_cjk|u_falpha);

      _Exc = [!\uff01];  // ！
      _Que = [?\uff1f];  // ？
      _Kop = [\u300c\uff62];   // 「
      _Kcl = [\u300d\uff63];   // 」
      _Pop = [(\uff08];  // (
      _Pcl = [)\uff09];  // )
      _Dop = [\u300e];   // 『
      _Dcl = [\u300f];   // 』
      _Und = [_\uff3f];  // ＿

      end = "\x00";
      brackets = (_Kop|_Kcl|_Pop|_Pcl|_Dop|_Dcl);
      ws = [\t \u00a0\u3000];        // 全角SP
      comma = [,\u3001\uff0c\uff64]; // 、，
      wss = ws+;
      nl = ("\n" | "\r\n" | "\r");
      ssep = (nl|wss|brackets|end);
      csep = (ssep|comma+);

      Dakuten = [\u3099\u309b];
      H_A  =  "\u3042"; // あ
      H_I  =  "\u3044"; // い
      H_Ka =  "\u304b"; // か
      H_Ga = ("\u304c" | "\u304b" Dakuten); // が
      H_Ki =  "\u304d"; // き
      H_Gi = ("\u304e" | "\u304d" Dakuten); // ぎ
      H_Ke =  "\u3051"; // け
      H_Ko =  "\u3053"; // こ
      H_Go = ("\u3054" | "\u3053" Dakuten); // ご
      H_Si =  "\u3057"; // し
      H_Ji = ("\u3058" | "\u3057" Dakuten); // じ
      H_Su =  "\u3059"; // す
      H_So =  "\u305d"; // そ
      H_Ta =  "\u305f"; // た
      H_Da = ("\u3060" | "\u305f" Dakuten); // だ
      H_tu =  "\u3063"; // っ
      H_Tu =  "\u3064"; // つ
      H_Te =  "\u3066"; // て
      H_De = ("\u3067" | "\u3066" Dakuten); // で
      H_To =  "\u3068"; // と
      H_Do = ("\u3069" | "\u3068" Dakuten); // ど
      H_Na =  "\u306a"; // な
      H_Ni =  "\u306b"; // に
      H_Ne =  "\u306d"; // ね
      H_No =  "\u306e"; // の
      H_Ha =  "\u306f"; // は
      H_He =  "\u3078"; // へ
      H_Ma =  "\u307e"; // ま
      H_Mi =  "\u307f"; // み
      H_yo =  "\u3087"; // ょ
      H_Yo =  "\u3088"; // よ
      H_Ra =  "\u3089"; // ら
      H_Ri =  "\u308a"; // り
      H_Re =  "\u308c"; // れ
      H_Wa =  "\u308f"; // わ
      H_Wo =  "\u3092"; // を
      H_N  =  "\u3093"; // ん
      K_Zu = ("\u30ba" | "\u30b9" Dakuten); // ズ
      K_Fu =  "\u30d5"; // フ
      K_He =  "\u30d8"; // ヘ
      K_Re =  "\u30ec"; // レ
      K_N  =  "\u30f3"; // ン
      ____ = [\u30fc\uff0d]; // ー (+ 全角ハイフン）

      Sugoi    = H_Su H_Go ____ H_I  _Exc;      // すごーい！
      Friends  = K_Fu K_Re K_N  K_Zu;           // フレンズ
      Nandane  = H_Na H_N  H_Da H_Ne _Exc;      // なんだね！
      Nandakke = H_Na H_N  H_Da H_tu H_Ke _Que; // なんだっけ？
      Tanoshi  = H_Ta ____ H_No H_Si ____ _Exc; // たーのしー！

      Anata  = H_A  H_Na H_Ta;      // あなた
      Kimi   = H_Ki H_Mi;           // きみ
      Kare   = H_Ka H_Re;           // かれ
      Kanojo = H_Ka H_No H_Ji H_yo; // かのじょ
      Dare   = H_Da H_Re;           // だれ
      Nani   = H_Na H_Ni;           // なに
      Are    = H_A  H_Re;           // あれ
      Kore   = H_Ko H_Re;           // これ
      Korera = H_Ko H_Re H_Ra;      // これら
      Sore   = H_So H_Re;           // それ
      Sorera = H_So H_Re H_Ra;      // それら

      Koko   = H_Ko H_Ko;           // ここ
      Soko   = H_So H_Ko;           // そこ
      Asoko  = H_A  H_So H_Ko;      // あそこ
      Doko   = H_Do H_Ko;           // どこ
      Dore   = H_Do H_Re;           // どれ

      No = H_No; // の
      Toka = H_To H_Ka; // とか
      Tsugi = (H_Tu H_Gi | "\u6b21"); // つぎ | 次

      De = H_De; // で
      Nara = H_Na H_Ra; // なら

      *   { friendsSetErrorV(p->err, ILSEQ); goto end; }
      end { goto end; }

      nl     { IncrementLine(); continue; }
      wss    { IncrementPos(); continue; }
      comma+ { IncrementPos(); continue; }

      Sugoi/ssep    { IncrementPos(); AddToken(SUGOI); continue; }
      Nandane/ssep  { IncrementPos(); AddToken(NANDANE); continue; }
      Nandakke/ssep { IncrementPos(); AddToken(NANDAKKE); continue; }
      Tanoshi/ssep  { IncrementPos(); AddToken(TANOSHI); continue; }
      No/csep       { IncrementPos(); goto may_no; }
      Tsugi/csep    { IncrementPos(); AddToken(TSUGI); continue; }
      Friends/csep  { IncrementPos(); AddToken(FRIEND); continue; }
      Toka/csep     { IncrementPos(); AddToken(TOKA); continue; }
      De/csep       { IncrementPos(); goto may_and; }
      Nara/csep     { IncrementPos(); goto may_then; }

      (Anata|Kimi|Kare|Kanojo|Dare|Nani|Are|Kore|Korera|
       Sore|Sorera|Koko|Soko|Asoko|Doko|Dore)/csep
                     { IncrementPos(); AddToken(VARIABLE); continue; }

      _Und           { IncrementPos(); goto und; }
      _Kop           { IncrementPos(); AddToken(LKAGI); continue; }
      _Kcl           { IncrementPos(); AddToken(RKAGI); continue; }
      _Pop           { IncrementPos(); ttype = friendsATOM; goto Pcl; }
      _Dop           { IncrementPos(); ttype = friendsATOM; goto Dcl; }
      atomchar+/csep { IncrementPos(); goto general_token; }
    */

    friendsUnreachable();

  may_no:
    switch (p->last_type) {
    case friendsATOM:
    case friendsVARIABLE:
    case friendsTSUGI:
    case friendsRKAGI:
      AddToken(NO);
      break;
    default:
      goto general_token;
      break;
    }
    continue;

  may_and:
    switch (p->last_type) {
    case friendsFRIEND:
      AddToken(AND);
      break;
    default:
      goto general_token;
    }
    continue;

  may_then:
    switch (p->last_type) {
    case friendsFRIEND:
      AddToken(THEN);
      break;
    default:
      goto general_token;
    }
    continue;

  general_token:
    switch (p->last_type) {
    case friendsATOM:
    case friendsVARIABLE:
    case friendsTSUGI:
    case friendsTOKA:
    case friendsRKAGI:
      AddToken(PARTICLE);
      break;
    default:
      AddToken(ATOM);
      break;
    }
    continue;

  und:
    ttype = friendsVARIABLE;
    p->lexer_state = friendsLexerUNDERSCORE;
    p->token = p->cur;

    /*!re2c
      * { friendsSetErrorV(p->err, ILSEQ); goto end; }
      end  { goto end; }
      _Pop { goto Pcl; }
      _Dop { goto Dcl; }
      atomchar+/csep {
        IncrementPos();
        friendsLexerAddToken(p, ttype);
        continue;
      }
     */

    friendsUnreachable();

  Pcl:
    p->token = p->cur;
  Pcl_continue:
    for (;;) {
      p->stoken = p->cur;
      /*!re2c
        re2c:indent:top = 3;

        * { friendsSetErrorV(p->err, ILSEQ); goto end; }
        end  { goto end; }
        _Pcl { break; }
        utf8 { continue; }
      */
    }
    goto bracket;

  Dcl:
    p->token = p->cur;
  Dcl_continue:
    for (;;) {
      p->stoken = p->cur;
      /*!re2c
        re2c:indent:top = 3;

        * { friendsSetErrorV(p->err, ILSEQ); goto end; }
        end  { goto end; }
        _Dcl { break; }
        utf8 { continue; }
      */
    }
    goto bracket;

  bracket:
    p->mark = p->cur;
    p->cur = p->stoken;
    friendsLexerAddToken(p, ttype);
    p->cur = p->mark;
    p->stoken = NULL;
    p->mark = NULL;
    continue;
  }

 end: /* No postactions here. */;
}
