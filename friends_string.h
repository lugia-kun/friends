/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_STRING_H
#define FRIENDS_STRING_H

#include "friends_defs.h"

/**
 * @brief 文字列をコピーするのです。
 * @param output 書き込み先をよこすのです。
 * @param input  読み込み先をよこすのです。
 * @return コピーした配列要素数を返すのです。
 *
 * NUL 文字で終わっていることはあなたが保証しなくてはならないのです。
 */
size_t friendsCopyString(friendsChar *output, const friendsChar *input);

/**
 * @brief 文字列の配列要素数を返すのです。
 * @param str 文字列をよこすのです。
 * @return 配列要素数を返すのです。
 *
 * UTF-16 モードでコンパイルしている時は、2バイト単位、
 * UTF-8  モードでコンパイルしている時は、1バイト単位に
 * なるのです。
 */
size_t friendsStringArrayLength(const friendsChar *str);

/**
 * @brief 文字列を比較するのです。
 * @param a 比較する文字列をよこすのです。
 * @param b 比較する文字列をよこすのです。
 * @return 同じなら 0、文字コード順で a が前なら負の値、b が前なら正の値を返すのです。
 *
 * 返す絶対値の値は、UTF-8 モードと UTF-16 モードで基本的に異なる値に
 * なるので、使ってはいけないのです。でも、等価性と順序は変わらないのです。
 */
int friendsStringCompare(const friendsChar *a, const friendsChar *b);

/**
 * @brief 文字列の文字数を返すのです。
 * @param start 文字列の先頭をよこすのです。
 * @param end   文字列の最後をよこすのです。
 * @param err   NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 文字数を返すのです。
 *
 * 正しくない文字が含まれている時は、そこまでの文字数を返し、`err` に
 * `ILSEQ` を設定するのです。
 *
 * `end` が NULL ならば、`start` から NUL 文字にあたるまで調べるのです。
 */
size_t friendsStringCharCount(const friendsChar *start,
                              const friendsChar *end,
                              friendsError *err);

/**
 * @brief C 言語の文字列定数をフレンズ用の文字列に変換するのです。
 * @param output ポインタの出力先のをよこすのです。
 * @param input  入力文字列をよこすのです。
 * @param err    NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 書き込んだ配列要素数を返すのです。失敗した時は、負の値を返すのです。
 *
 * この関数は、適当な記憶スペースを確保して、Unicode と C 言語のエスケー
 * プ文字を展開した文字列を作るのです。例えば、`"\\u3000"` という文字
 * 列は全角スペースになるのです。ソースに記述する時は、バックスラッシュ
 * を2つ記述しないといけないことに注意するのです。
 *
 * 対応しているのは `\\a`, `\\b`, `\\t`, `\\n`, `\\v`, `\\f`, `\\r`,
 * `\\e`, `\\\"`, `\\\'`, `\\?`, `\\\\`, `\\xHH`, `\\uHHHH`,
 * `\\UHHHHHHHH`, `\\OOO` なのです。`H` は16進数の数値、`O` は 8 進数
 * の数字を入れるのです。`\\xHH` と `\\u00HH` はフレンズ用の文字列に使っ
 * ている文字コードに変換するかどうかの違いがあるのです。なお、
 * `friendsChar` が UTF-16 を使っていても、`\\xHH` では 16bit の値を入れ
 * ることはできないのです。
 *
 * 入力文字列は ASCII 文字だけで記述する必要があるのです。ASCII 文字以
 * 外が含まれている時は、`err` に `INVAL` を設定するのです。
 *
 * 記憶できるスペースが足りない時は、`err` に `NOMEM` を設定するのです。
 *
 * この関数は、ユーザーが入力した文字列の処理には適さないのです。
 */
int friendsUnescapeStringLiteral(friendsChar **output, const char *input,
                                 friendsError *err);

#endif
