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
friendsSize friendsCopyString(friendsChar *output, const friendsChar *input);

/**
 * @brief 文字列の配列要素数を返すのです。
 * @param str 文字列をよこすのです。
 * @return 配列要素数を返すのです。
 *
 * UTF-16 モードでコンパイルしている時は、2バイト単位、
 * UTF-8  モードでコンパイルしている時は、1バイト単位に
 * なるのです。
 *
 * 当然、その配列に確保したメモリの大きさ、じゃなくて、NUL 文字までの
 * 要素数なのです。ですから、NUL 文字で終わっていることはあなたが保証
 * しなくてはならないのです。NUL 文字は含まないのです。
 */
friendsSize friendsStringArrayLength(const friendsChar *str);

/**
 * @brief 文字列を複製するのです。
 * @param output 出力先をよこすのです。
 * @param fstp コピー元の最初の位置をよこすのです。
 * @param endp コピー元の最後の位置をよこすのです。
 * @param NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return コピーした配列要素数を返すのです。失敗した時は -1 を返すのです。
 *
 * endp は NULL にしても良いのです。この時は、friendsStringArrayLength
 * を使って長さを調べるのです。
 *
 * そして、メモリを確保して friendsCopyString を使って文字列をコピーす
 * るのです。
 *
 * 使い終わったらちゃんと free 関数で解放するのですよ。
 */
friendsSize
friendsStringDuplicate(friendsChar **output,
                       const friendsChar *fstp, const friendsChar *endp,
                       friendsError *err);

/**
 * @brief 文字列を比較するのです。
 * @param a 比較する文字列をよこすのです。
 * @param b 比較する文字列をよこすのです。
 * @return 同じなら 0、文字コード順で a が前なら負の値、b が前なら正の
 *         値を返すのです。
 *
 * 返す絶対値の値は、UTF-8 モードと UTF-16 モードで基本的に異なる値に
 * なるので、使ってはいけないのです。でも、符号が変わることはないので
 * す。
 */
int friendsStringCompare(const friendsChar *a, const friendsChar *b);

/**
 * @brief 文字列の文字数を返すのです。
 * @param start 文字列の先頭をよこすのです。
 * @param end   文字列の最後をよこすのです。
 * @param err   NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 文字数を返すのです。
 *
 * 正しくない文字が含まれている時は、そこまでの文字数を返し、`err` に
 * `ILSEQ` を設定するのです。
 *
 * `end` が NULL ならば、`start` から NUL 文字にあたるまで調べるのです。
 */
friendsSize friendsStringCharCount(const friendsChar *start,
                              const friendsChar *end,
                              friendsError *err);

/**
 * @brief 数値で書かれた文字列を数値に変換するのです。
 * @param test 変換する文字列をよこすのです。
 * @param end NULL でなければ、変換できた文字の最後を書き込むのです。
 * @param base 基数をよこすのです。
 * @param err NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 変換した値を返すのです。エラーの時は LONG_MIN を返すのです。
 *
 * この関数は strtol の Friends 版なのです。基数は 2 から 36 まで指定
 * できるのです。
 *
 * 日本語なプログラムなので、全角の使用を許しているのです。ただし、負
 * 符号には、全角ハイフン「－」(U+FF0D) マイナス (U+2212) だけを許して
 * いるのです。長音記号「ー」(U+30FC)、ダッシュ「―」(U+2015) ではダメ
 * ですよ。
 */
long int friendsStringToLong(const friendsChar *text, const friendsChar **end,
                             int base, friendsError *err);

/**
 * @brief C 言語の文字列定数をフレンズ用の文字列に変換するのです。
 * @param output ポインタの出力先のをよこすのです。
 * @param input  入力文字列をよこすのです。
 * @param err    NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 書き込んだ配列要素数を返すのです。失敗した時は、負の値を返
 *         すのです。
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
 * `friendsChar` が UTF-16 を使っていても、`\\xHH` では 16bit の値を入
 * れることはできないのです。
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

friendsStringList *friendsNewStringList(friendsError *e);
void friendsDeleteStringList(friendsStringList *head);
void friendsStringListRemove(friendsStringList *item);

friendsStringList *
friendsStringListInsert(friendsStringList *list, const friendsChar *start,
                        const friendsChar *end, friendsError *e);

friendsStringList *friendsStringListNext(friendsStringList *item);

friendsStringList *friendsStringListPrev(friendsStringList *item);

const friendsChar *friendsStringListGetString(friendsStringList *item);

#endif
