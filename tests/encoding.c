
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_common.h"

#include "friends_defs.h"
#include "friends_config.h"

#ifdef FRIENDS_ENABLE_SJIS
#include "enc/sjis.h"
#endif
#ifdef FRIENDS_ENABLE_EUCJP
#include "enc/eucjp.h"
#endif
#include "enc/utf8.h"

#include "friends_io.h"
#include "friends_string.h"
#include "friends_test.h"

#define SAMPLE_CODE                                         \
  "\\u3059\\u3054\\u30fc\\u3044\\uff01"  /* すごーい！ */   \
  " 0 \\u306f \\u81ea\\u7136\\u6570"     /*  0 は 自然数 */ \
  "\\u306e \\u30d5\\u30ec\\u30f3\\u30ba" /* の フレンズ */  \
  " \\u306a\\u3093\\u3060\\u306d\\uff01" /*  なんだね！ */

#define LONG_CODE                                                 \
  "\\u3059\\u3054\\u30fc\\u3044\\uff01"  /* すごーい！ */         \
  " \\u304d\\u307f \\u304c 15\\u306e"    /*  きみ が 15の */      \
  "\\u500d\\u6570\\u306e \\u30d5\\u30ec" /* 倍数の フレ */        \
  "\\u30f3\\u30ba \\u306a\\u3089 "       /* ンズ なら  */         \
  "\\u304d\\u307f \\u3068 FizzBuzz "     /* きみ と FizzBuzz */   \
  "\\u306f FizzBuzz \\u30d5\\u30ec"      /* は FizzBuzz フレ */   \
  "\\u30f3\\u30ba \\u306a\\u3093\\u3060" /* ンズ なんだ */        \
  "\\u306d\\uff01 \\u305f\\u30fc\\u306e" /* ね！ たーの */        \
  "\\u3057\\u30fc\\uff01"                /* しー！ */

#define MIXED_CODE                                               \
  "\\u3059\\u3054\\u30fc\\u3044\\uff01"  /* すごーい！ */        \
  " \\u304d\\u307f \\u304c \\U0001f989"  /*  きみ が （助手） */ \
  "\\u3000\\u3068 \\u4ef2\\u306e\\u3044" /* 　と 仲のい */       \
  "\\u3044 \\u30d5\\u30ec\\u30f3\\u30ba" /* い フレンズ */       \
  " \\u3067\\u3001"                      /*  で、 */             \
  "\\u305d\\u308c \\u304c \\u03b1"       /* それ が α */         \
  " \\u3068 \\u8a00\\u308f\\u308c"       /*  と 言われ */        \
  "\\u308b \\u30d5\\u30ec\\u30f3\\u30ba" /* る フレンズ */       \
  " \\u306a\\u3089\\u3001"               /* なら、 */            \
  "\\u304d\\u307f \\u3068 \\u305d"       /* きみ と そ */        \
  "\\u308c \\u306f \\u3059\\u3066"       /* れ は すて */        \
  "\\u304d\\u306a \\u30d5\\u30ec\\u30f3" /* きな フレン */       \
  "\\u30ba \\u306a\\u3093\\u3060\\u306d" /* ズ なんだね */       \
  "\\uff01"                              /* ！ */


#define FULL_HIRAGANA "\\u3042"      /* あ */
#define KANJI         "\\u6c38"      /* 永 */
#define HALF_KATAKANA "\\uff71"      /* ア（半角）*/
#define FULL_PUNCT    "\\uff01"      /* ！ */
#define FULL_TILDE    "\\uff5e"      /* 〜 */
#define EMOJI         "\\U0001f989"  /* （助手） */
#define GREEK         "\\u03b1"      /* α */

typedef struct {
  char *test_char;
  int expect_byte_size;
} singleTestData;

typedef struct {
  singleTestData *tdata;
  const char **stringTests;
} codeTestData;

void encodingTester(const friendsCodeSet *set, codeTestData *info,
                    friendsError *e)
{
  friendsChar *fp, *fp2;
  char *buf;
  const char *cur;
  singleTestData *sp;
  const char **ssp;
  int n;
  friendsError ex;
  friendsChar fchar[FRIENDS_MAX_CHAR + 1];

  friendsPrintCF(stdout, NULL, "... "
                 "\\u3044\\u307e\\u304b\\u3089 %s"  /* いまから %s */
                 " \\u306e\\u30c6\\u30b9\\u30c8"  /*  のテスト */
                 "\\u3092\\u59cb\\u3081\\u308b"  /* を始めるの */
                 "\\u306e\\u3067\\u3059\\u3002\\n"  /* です。 */,
                 set->name);

  if (!e) {
    e = &ex;
  }

  friendsClearError(e);

  sp = info->tdata;
  fchar[FRIENDS_MAX_CHAR] = 0;
  for (; sp && sp->test_char; ++sp) {
    fp = NULL;
    buf = NULL;

    n = friendsUnescapeStringLiteral(&fp, sp->test_char, e);
    if_error_goto_error(*e, "friendsUnescapeStringLiteral", next_test);

    n = set->dec(&buf, fp, e);
    if_error_goto_error(*e, "decoder", next_test);

    friendsPrintCF(stdout, NULL, "    ... "
                   "\\u6587\\u5b57 %ls \\u3092\\u5909"  /* 文字 %ls を変 */
                   "\\u63db\\u3057\\u3066\\u307f\\u308b"  /* 換してみる */
                   "\\u306e\\u3067\\u3059\\u3002\\n"  /* のです。 */,
                   fp);

    cur = buf;
    n = set->one_enc(fchar, &cur, e);
    if_error_goto_error(*e, "one char encoder", next_test);
    n = cur - buf;
    friendsTestExpect(n, sp->expect_byte_size, "%d", friendsTestEqual);
    friendsTestExpect(fchar, fp, "%ls", friendsStringCompare);

  next_test:
    friendsClearError(e);
    if (fp)  free(fp);
    if (buf) free(buf);
  }

  ssp = info->stringTests;
  for (; ssp && *ssp; ++ssp) {
    fp  = NULL;
    fp2 = NULL;
    buf = NULL;

    n = friendsUnescapeStringLiteral(&fp, *ssp, e);
    if_error_goto_error(*e, "friendsUnescapeStringLiteral", next_str);

    n = set->dec(&buf, fp, e);
    if_error_goto_error(*e, "decoder", next_str);

    friendsPrintCF(stdout, NULL, "    ... %ls\\n", fp);
    n = set->enc(&fp2, buf, e);
    if_error_goto_error(*e, "encoder", next_str);

    friendsPrintCF(stdout, NULL, "    --> %ls\\n", fp2);
    friendsTestExpect(fp2, fp, "%ls", friendsStringCompare);

  next_str:
    friendsClearError(e);
    if (fp)  free(fp);
    if (buf) free(buf);
    if (fp2) free(fp2);
  }
}

int main(int argc, char **argv)
{
  friendsError e;
  int ret;
  int i;

#ifdef FRIENDS_ENABLE_SJIS
  singleTestData sjisTestChars[] = {
    { FULL_HIRAGANA, 2 }, { KANJI, 2 }, { HALF_KATAKANA, 1 },
    { FULL_PUNCT, 2 }, /* { EMOJI, -1 }, */ { GREEK, 2 },
    { FULL_TILDE, 2 },
    { NULL, 0 }
  };
  const char *sjisTestStrings[] = { SAMPLE_CODE, LONG_CODE, NULL };
  codeTestData sjisTestData = { sjisTestChars, sjisTestStrings };
#endif
#ifdef FRIENDS_ENABLE_EUCJP
  singleTestData eucjpTestChars[] = {
    { FULL_HIRAGANA, 2 }, { KANJI, 2 }, { HALF_KATAKANA, 2 },
    { FULL_PUNCT, 2 }, /* { EMOJI, -1 }, */ { GREEK, 2 },
    { FULL_TILDE, 3 },
    { NULL, 0 }
  };
  const char *eucjpTestStrings[] = { SAMPLE_CODE, LONG_CODE, NULL };
  codeTestData eucjpTestData = { eucjpTestChars, eucjpTestStrings };
#endif
  singleTestData utf8TestChars[] = {
    { FULL_HIRAGANA, 3 }, { KANJI, 3 }, { HALF_KATAKANA, 3 },
    { FULL_PUNCT, 3 }, { EMOJI, 4 }, { GREEK, 2 },
    { FULL_TILDE, 3 },
    { NULL, 0 }
  };
  const char *utf8TestStrings[] = { SAMPLE_CODE, LONG_CODE, MIXED_CODE, NULL };
  codeTestData utf8TestData = { utf8TestChars, utf8TestStrings };

  ret = EXIT_SUCCESS;
  e = friendsNoError;

#ifdef FRIENDS_ENABLE_SJIS
  encodingTester(friendsSjisSet(), &sjisTestData, &e);
#endif
#ifdef FRIENDS_ENABLE_EUCJP
  encodingTester(friendsEucjpSet(), &eucjpTestData, &e);
#endif
  encodingTester(friendsUtf8Set(), &utf8TestData, &e);

  i = friendsTestErrorCount();
  if (i > 0) ret = EXIT_FAILURE;
  return ret;
}
