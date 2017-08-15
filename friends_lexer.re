/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "friends_struct_data.h"
#include "friends_lexer.h"
#include "friends_error.h"
#include "friends_string.h"
#include "friends_parser.h"

/* provided by lemon paraser generator. */
void *friendsParserCoreAlloc(void *(*allocator)(size_t));
void friendsParserCoreFree(void *parser, void (*deallocator)(void *));
void friendsParserCore(void *parser, int token, friendsParser *p);

#define Parse(token) friendsParserCore(p.parserData, friends##token, &p)

void friendsParse(friendsPark *park, const friendsChar *str,
                  friendsFillFunc *fillfunc, void *fillarg,
                  friendsError *err)
{
  friendsParser p;
  memset(&p, 0, sizeof(friendsParser));

  friendsAssert(park);
  friendsAssert(str || fillfunc);

  if (fillfunc) {
    p.fill = fillfunc;
    p.fill(&p, 1024, fillarg);
  } else if (str) {
    p.cur = str;
    p.limit = str + friendsStringArrayLength(str);
  }

  p.parserData = friendsParserCoreAlloc(malloc);
  if (!p.parserData) {
    friendsSetError(err, NOMEM);
    return;
  }

  for(;;) {
    p.token = p.cur;

    /*!re2c
      re2c:define:YYCTYPE = "friendsChar";
      re2c:define:YYCURSOR = "p.cur";
      re2c:define:YYMARKER = "p.mark";
      re2c:define:YYLIMIT = "p.limit";
      re2c:define:YYFILL:naked = 1;
      re2c:define:YYFILL = "{ if(p.fill) p.fill(&p, @@, fillarg); }";
      re2c:indent:string = "  ";
      re2c:indent:top = 2;

      utf8 = [\u0001-\uffff\U00010000-\U0010ffff];

      wss = [\u3001\uff0c\u3000\t ,]+; // 、，（全角SP）
      nl = ("\n" | "\r\n" | "\r");
      Exc = [\uff01!];  // ！
      Que = [\uff1f?];  // ？

      Dakuten = [\u3099\u309b];
      H_A  =  "\u3042"; // あ
      H_I  =  "\u3044"; // い
      H_Ka =  "\u304b"; // か
      H_Ga = ("\u304c" | "\u304b" Dakuten); // が
      H_Ki =  "\u304d"; // き
      H_Gi = ("\u304e" | "\u304d" Dakuten); // ぎ
      H_Ke =  "\u3051"; // け
      H_Ko =  "\u3053"; // こ
      H_Si =  "\u3057"; // し
      H_Ji = ("\u3058" | "\u3057" Dakuten); // じ
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

      Waai     = H_Wa ____ H_I  Exc;           // わーい！
      Friends  = K_Fu K_Re K_N  K_Zu;          // フレンズ
      Nandane  = H_Na H_N  H_Da H_Ne Exc;      // なんだね！
      Nandakke = H_Na H_N  H_Da H_tu H_Ke Que; // なんだっけ？
      Tanoshi  = H_Ta ____ H_No H_Si ____ Exc; // たーのしー！

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
      To = H_To; // と
      Toka = H_To H_Ka; // とか
      Tsugi = (H_Tu H_Gi | "\u6b21"); // つぎ | 次

      * { friendsSetError(err, ILSEQ); break; }
      "\x00" { break; }

      Waai (wss|nl)     { Parse(WAAI); continue; }
      Nandane  (wss|nl) { Parse(NANDANE); continue; }
      Nandakke (wss|nl) { Parse(NANDAKKE); continue; }
      No (wss|nl)       { Parse(NO); continue; }
      Tanoshi (wss|nl)  { Parse(TANOSHI); continue; }
      Tsugi (wss|nl)    { Parse(TSUGI); continue; }
      Friends (wss|nl)  { Parse(FRIEND); continue; }

      utf8 { continue; }
    */
  }

  friendsParserCoreFree(p.parserData, free);
}
