/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_TEST_H
#define FRIENDS_TEST_H

#include "friends_defs.h"

typedef void friendsTestErrorHandler(void);

/**
 * @brief テストのメッセージを表示するのです。
 * @param file ファイル名をよこすのです。
 * @param line 行番号をよこすのです。
 * @param data_format データを文字列化するための変換文字列をよこすのです。
 * @param object_name オブジェクトの名前をよこすのです (char *)。
 * @param object_data オブジェクトのデータをよこすのです (data_format に沿った型)。
 * @param expect_data 正しい値のデータをよこすのです (data_format に沿った型)。
 *
 * 可変個引数部分は必ず上記のように指定するのです。
 *
 * @note この関数は内部用なのです。
 */
void friendsTestExpectPrint(const char *file, long line,
                            const char *data_format, ...);

/**
 * @brief 文字列用のテスト補助関数なのです。
 * @param object 比較するデータをよこすのです。
 * @param expect 正しいの文字列をよこすのです。
 * @param file ファイル名をよこすのです。
 * @param line 行番号をよこすのです。
 * @param object_name オブジェクトの名前をよこすのです。
 *
 * @note この関数は内部用なのです。
 */
void friendsTestExpectTextImplement(const friendsChar *object,
                                    const char *expect,
                                    const char *file, long line,
                                    const char *object_name);

/**
 * @brief エラー回数を返すのです。
 * @return プログラムが起動してから `friendsTestExpect*` を呼び出して
 *         正しい値ではなかった回数を返すのです。
 *
 * エラーの数はスレッドごとにカウントされているのです（可能ならスレッ
 * ドローカルで保存）。
 */
int friendsTestErrorCount(void);

/**
 * @brief 単にイコールで比較するのです。
 * @param a 比較するデータをよこすのです。
 * @param b 比較するデータをよこすのです。
 */
#define friendsTestEqual(a, b) ((a) != (b))

/**
 * @brief 常に真になる比較なのです。
 * @param a 比較するデータをよこすのです。
 * @param b 比較するデータをよこすのです。
 */
#define friendsTestTrue(a, b) (1)

/**
 * @brief テストのときに、問題ないかを確かめるのです。
 * @param object 調べるものをよこすのです。
 * @param except 正しい値をよこすのです。
 * @param formatter 画面に表示するための変換文字列をよこすのです。
 * @param comparator 正しいかを比較するための関数をよこすのです。
 *
 * 単にイコールで比較するなら、`friendsTestEqual` を使うのです。
 * 文字列の比較をするなら、`strcmp` とか `friendsStringCompare` を使うのです。
 *
 * `formatter` には、例えば、int 型のデータなら `%d` を、
 * `friendsChar*` 型なら `%ls` といった具合に渡すのです。
 *
 * @warning このマクロの引数には絶対に副作用を発生されるものを入れては
 *          いけないのです。
 */
#define friendsTestExpect(object, expect, formatter, comparator)        \
  do {                                                                  \
    if (comparator((object), (expect)) != 0) {                          \
      friendsTestExpectPrint(FRIENDS_SOURCE_FILE, __LINE__, formatter,  \
                             #object, (object), (expect));              \
    }                                                                   \
  } while(0)

/**
 * @brief テストのときに、問題ないかを確かめるのです。
 * @param object 調べるものをよこすのです。
 * @param except 正しい値をよこすのです。
 *
 * この関数は friendsChar* 専用で、正しい値を C の文字列で書きたいとき
 * に使うのです。
 */
#define friendsTestExpectText(object, expect)                       \
  friendsTestExpectTextImplement((object), (expect),                \
                                 FRIENDS_SOURCE_FILE, __LINE__,     \
                                 #object)

/**
 * @brief テストのときに、問題ないかを確かめるのです。
 * @param object 調べるものをよこすのです。
 * @param except 正しい値をよこすのです。
 * @param actual_s 実際の値を文字列化したものをよこすのです (char *)。
 * @param expect_s 正しい値を文字列化したものをよこすのです (char *)。
 * @param comparator 正しいかを比較するための関数をよこすのです。
 *
 * 単にイコールで比較するなら、`friendsTestEqual` を使うのです。
 * 文字列の比較をするなら、`strcmp` とか `friendsStringCompare` を使うのです。
 *
 * @warning このマクロの引数には絶対に副作用を発生されるものを入れては
 *          いけないのです。
 *
 * @warning actual_s と expect_s に間違って friendsChar* の値をよこさ
 *          れてもコンパイルエラーにならないので注意するのです。
 */
#define friendsTestExpectCS(object, expect, actual_s, expect_s, comparator) \
  do {                                                                  \
    if (comparator((object), (expect)) != 0) {                          \
      friendsTestExpectPrint(FRIENDS_SOURCE_FILE, __LINE__, "%s",       \
                             #object, actual_s, expect_s);              \
    }                                                                   \
  } while(0)

/**
 * @brief テストのときに、問題ないかを確かめるのです。
 * @param object 調べるものをよこすのです。
 * @param except 正しい値をよこすのです。
 * @param actual_s 実際の値を文字列化したものをよこすのです (friendsChar *)。
 * @param expect_s 正しい値を文字列化したものをよこすのです (friendsChar *)。
 * @param comparator 正しいかを比較するための関数をよこすのです。
 *
 * 単にイコールで比較するなら、`friendsTestEqual` を使うのです。
 * 文字列の比較をするなら、`strcmp` とか `friendsStringCompare` を使うのです。
 *
 * @warning このマクロの引数には絶対に副作用を発生されるものを入れては
 *          いけないのです。
 *
 * @warning actual_s と expect_s に間違って char* の値をよこさ
 *          れてもコンパイルエラーにならないので注意するのです。
 */
#define friendsTestExpectFS(object, expect, actual_s, expect_s, comparator) \
  do {                                                                  \
    if (comparator((object), (expect)) != 0) {                          \
      friendsTestExpectPrint(FRIENDS_SOURCE_FILE, __LINE__, "%ls",      \
                             #object, actual_s, expect_s);              \
    }                                                                   \
  } while(0)

#endif
