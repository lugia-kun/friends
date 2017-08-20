/**
 * @file friends_variable.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_VARIABLE_H
#define FRIENDS_VARIABLE_H

#include "friends_defs.h"

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
friendsData *friendsSetVariable(friendsData *dest, friendsChar *t,
                                int offset, friendsError *e);

/**
 * @brief 変数の文字列を得るのです。
 * @param d 変数データをよこすのです。
 * @return 変数の文字列を返すのです。
 */
const friendsChar *friendsVariableText(const friendsVariableData *d);

/**
 * @brief 変数の差分を得るのです。
 * @param d 変数データをよこすのです。
 * @return 変数の差分を返すのです。
 */
int friendsVariableOffset(const friendsVariableData *d);

#endif /* FRIENDS_VARAIBLE_H */
