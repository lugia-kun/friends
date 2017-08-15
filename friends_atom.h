/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_ATOM_H
#define FRIENDS_ATOM_H

#include "friends_defs.h"

/**
 * @brief データに入っているアトムデータを返すのです。
 * @param d データをよこすのです。
 * @return アトムデータを返すのです。種類が違っている時はNULLを返すのです。
 */
const friendsAtomData *friendsGetAtom(friendsData *d);

/**
 * @brief 数字のアトムを設定するのです。
 * @param dest 設定先のデータをよこすのです。
 * @param a    設定する値をよこすのです。
 * @param e    NULL でない値をよこしたなら、そこにエラーの情報を書き込むのです。
 * @return よこしたデータを返すのです。設定できなかった時は NULL を返すのです。
 */
friendsData *friendsSetNumeralAtom(friendsData *dest, int a, friendsError *e);

/**
 * @brief 文字のアトムを設定するのです。
 * @param dest 設定先のデータをよこすのです。
 * @param t    設定する文字列をよこすのです。
 * @param e    NULL でない値をよこしたなら、そこにエラーの情報を書き込むのです。
 * @return よこしたデータを返すのです。
 *
 * 設定した文字列はデータの一部になり、パークが捨てられたときに一緒に捨てられるのです。
 */
friendsData *friendsSetTextAtom(friendsData *dest, const friendsChar *t,
                                friendsError *e);

/**
 * @brief アトムデータの数値を得るのです。
 * @param a アトムデータをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 数値を返すのです。数値でない時は -1 を返すのです。
 *
 * @note 将来の拡張で負数を使えるようにするかもしれないので、エラーの
 *       判定には e を使うのをおすすめするのです。
 */
int friendsGetAtomNumber(const friendsAtomData *a, friendsError *e);

/**
 * @brief アトムデータから文字列を得るのです。
 * @param a アトムデータをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 文字列を返すのです。文字列アトムでない時は、NULL を返すのです。
 */
const friendsChar *friendsGetAtomText(const friendsAtomData *a, friendsError *e);

#endif
