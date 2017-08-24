/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_DATA_H
#define FRIENDS_DATA_H

#include "friends_defs.h"

/**
 * @brief 新しいデータを作るのです。
 * @param park パークをよこすのです。
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 新しいデータの場所を返すのです。失敗した時は、NULL を返すの
 *         です。
 *
 * `park` が NULL の時は、`e` に INVAL を設定するです。
 * 記憶できるスペースが無い時は、`e` に NOMEM を設定するです。
 */
friendsData *friendsNewData(friendsPark *park, friendsError *e);

/**
 * @brief データを削除するのです。
 * @param d 削除するデータをよこすのです。
 */
void friendsDeleteData(friendsData *d);

/**
 * @brief データから所属するパークを得るのです。
 * @param d データをよこすのです。
 * @return パークを返すのです。d が NULL なら、我々も NULL を返すので
 *         す。
 */
friendsPark *friendsGetPark(friendsData *d);

/**
 * @brief データの種類を得るのです。
 * @param d データをよこすのです。
 * @return データの種類を返すのです。d が NULL なら、我々は
 *         friendsInvalidType を返すのです。
 */
friendsType friendsGetType(friendsData *d);

/**
 * @brief データのハッシュ値を得るのです。
 * @param d データをよこすのです。
 * @return ハッシュ値を返すのです。
 *
 * この関数は基本的にはデバッグ向きなのです。
 */
friendsHash friendsGetHash(friendsData *d);

/**
 * @brief データをセットするのです。
 * @param dest 設定するデータをよこすのです。
 * @param type データの種類をよこすのです。
 * @param data 追加データをよこすのです。
 * @param data_deleter データの削除関数をよこすのです。
 * @param comp_func 比較関数をよこすのです。
 * @param text データを表す文字列をよこすのです。
 * @param text_deleter text を削除する関数をよこすのです。
 * @param hash ハッシュ値をよこすのです。
 * @param allow_replace 他の同じ型のデータを置き換えることを許可するの
 *        です。
 * @param err NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 成功なら dest を、失敗なら NULL を返すのです。
 *
 * この関数は内部用なのです。
 *
 * 削除関数は自動削除して欲しくなければ NULL をよこすのです。
 */
friendsData *friendsSetData(friendsData *dest, friendsType type,
                            void *data, friendsPointerDeleter *data_deleter,
                            friendsDataCompareFunc *comp_func,
                            friendsChar *text,
                            friendsPointerDeleter *text_deleter,
                            friendsHash hash,
                            friendsBool allow_replace,
                            friendsError *err);

/**
 * @brief データに入っている文字列を得るのです。
 * @param d データをよこすのです。
 * @return データが、アトム，変数ならその文字を、命題なら述語の文字列
 *         を返すのです。それ以外の場合はたぶん NULL を返すのです。
 */
const friendsChar *friendsDataToText(friendsData *d);

/**
 * @brief データ同士の比較を行うのです。
 * @param a データをよこすのです。
 * @param b データをよこすのです。
 * @return 比較結果を返すのです。
 *
 * 結果は複合値を返すことがあるのです。例えば、所属しているパークが違
 * うけど、中身が同じなら `friendsDataInAnotherPark |
 * friendsDataEqual` な値を返すのです。
 *
 * 通常の比較では `friendsDataEqual` とかの単体と比較すれば良いと思う
 * のです。
 */
friendsDataCompareResult friendsDataCompare(const friendsData *a,
                                            const friendsData *b);

/**
 * @brief 数値をハッシュするのです。
 * @param a ハッシュする値をよこすのです。
 * @return ハッシュした値を返すのです。
 *
 * ハッシュ値は、上位 2 ビットが 00 なら数値、上位 2 ビットが 01 なら
 * バイナリデータ、最上位ビットが 1 なら文字列にしているです。でも、こ
 * のルールは型の判別に使ってはいけないのです。そして、ハッシュ値はファ
 * イルに保存しないほうが良いのです。
 *
 * 数値のハッシュ値は最上位 4 ビットが 0 なら値がそのままハッシュ値に
 * し、上位 4 ビットを 0001 にするのです。それ以外の場合は、文字列扱い
 * でハッシュされ、上位 3 ビットを 001 にするのです。
 *
 * 上位 4 ビットが 0000 になるハッシュは使われていないのです。
 */
friendsHash friendsHashNumeral(int a);

/**
 * @brief 文字列をハッシュするのです。
 * @param s ハッシュする文字列をよこすのです。
 * @param e 文字列の最後をよこすのです。
 * @return ハッシュした値を返すのです。
 *
 * UTF-8 モードでも UTF-16 モードでも 1 バイトずつの処理をするのです。
 * でも、文字列のデータをそのまま使うので、UTF-8 モードのハッシュ値と
 * UTF-16 モードのハッシュ値は異なる値になるのです。
 *
 * `e` を指定している時、`e` の指している場所は含まないのです。NULL の
 * ときは、NUL 文字までを調べるのです。
 */
friendsHash friendsHashString(const friendsChar *s, const friendsChar *e);

/**
 * @brief バイナリをハッシュするのです。
 * @param s ハッシュするデータの先頭をよこすのです。
 * @param e ハッシュするデータの最後をよこすのです。
 * @return ハッシュした値を返すのです。
 *
 * `e` の指している場所は含まないのです。
 */
friendsHash friendsHashBinary(const void *s, const void *e);

/**
 * @brief 比較結果が等しいか調べるのです。
 * @param x 比較結果をよこすのです。
 * @return 等しければ 1 を返すのです。等しくなければ 0 を返すのです。
 */
int friendsDataCompareIsEqual(friendsDataCompareResult x);

/**
 * @brief 比較結果が等しくないか調べるのです。
 * @param x 比較結果をよこすのです。
 * @return 等しくなければ 1 を返すのです。等しければ 0 を返すのです。
 */
int friendsDataCompareIsNotEqual(friendsDataCompareResult x);

/**
 * @brief 比較結果が Set 用の比較で等しいか調べるのです。
 * @param x 比較結果をよこすのです。
 * @return 等しければ 1 を返すのです。等しくなければ 0 を返すのです。
 */
int friendsDataCompareIsSetEqual(friendsDataCompareResult x);

#endif /* FRIENDS_DATA_H */
