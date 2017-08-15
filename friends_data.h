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
 * @param e NULL ではない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 新しいデータの場所を返すのです。失敗した時は、NULL を返すのです。
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
 * @return パークを返すのです。d が NULL なら、我々も NULL を返すのです。
 */
friendsPark *friendsGetPark(friendsData *d);

/**
 * @brief データの種類を得るのです。
 * @param d データをよこすのです。
 * @return データの種類を返すのです。d が NULL なら、我々は friendsInvalidType を返すのです。
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
 * @brief データに入っている文字列を得るのです。
 * @param d データをよこすのです。
 * @return データが、アトム，変数ならその文字を、命題なら述語の文字列を返すのです。
 *         それ以外の場合は NULL を返すのです。
 */
const friendsChar *friendsDataToText(friendsData *d);

/**
 * @brief データに入っている命題のデータを返すのです。
 * @param d データをよこすのです。
 * @return 命題データを返すのです。種類が違っている時は NULL を返すのです。
 */
const friendsPropositionData *friendsGetProposition(friendsData *d);

/**
 * @brief データに入っている変数データを返すのです。
 * @param d データをよこすのです。
 * @return 変数データを返すのです。種類が違っている時はNULLを返すのです。
 */
const friendsVariableData *friendsGetVariable(friendsData *d);

/**
 * @brief 変数を設定するです。
 * @param dest 設定先のデータをよこすのです。
 * @param t    設定する文字列をよこすのです。
 * @param offset 連番の差分、すなわち、「〜の次」を使った回数をよこすのです。
 * @param e    NULL でない値をよこしたなら、そこにエラーの情報を書き込むのです。
 * @return よこしたデータを返すのです。
 *
 * 設定した文字列はデータの一部になり、パークが捨てられたときに一緒に捨てられるのです。
 */
friendsData *friendsSetVariable(friendsData *dest, const friendsChar *t,
                                int offset, friendsError *e);

/**
 * @brief 数値をハッシュするのです。
 * @param a ハッシュする値をよこすのです。
 * @return ハッシュした値を返すのです。
 *
 * ハッシュ値は、上位 2 ビットが 00 なら数値、上位 2 ビットが 01 なら
 * バイナリデータ、最上位ビットが 1 なら文字列にしているです。でも、こ
 * のルールは型の判別に使ってはいけないのです。
 * そして、ハッシュ値はファイルに保存しないほうが良いのです。
 *
 * 数値のハッシュ値は最上位 4 ビットが 0 なら値がそのままハッシュ値に
 * し、上位 4 ビットを 0001 にするのです。それ以外の場合は、文字列扱い
 * でハッシュされ、上位 3 ビットを 001 にするのです。
 *
 * 上位 4 ビットが 0000 になるハッシュは使われていないのです。
 * ハッシュ値 0 は無効なハッシュとして扱われるのです。
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
 * `e` を指定している時、`e` の指している場所は含まないのです。NULL のときは、
 * NUL 文字までを調べるのです。
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

#endif /* FRIENDS_DATA_H */
