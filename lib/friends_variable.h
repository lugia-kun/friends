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
 * @return 変数データを返すのです。種類が違っている時はNULLを返すので
 *         す。
 */
const friendsVariableData *friendsGetVariable(friendsData *d);

/**
 * @brief 変数を設定するです。
 * @param dest 設定先のデータをよこすのです。
 * @param text 設定する文字列をよこすのです。
 * @param tail 後続を省略する時、friendsTrue をよこすのです。
 * @param e    NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return よこしたデータを返すのです。
 */
friendsData *friendsSetVariable(friendsData *dest, const friendsChar *text,
                                friendsBool tail, friendsError *e);

/**
 * @brief 変数の文字列を得るのです。
 * @param d 変数データをよこすのです。
 * @return 変数の文字列を返すのです。
 */
const friendsChar *friendsVariableText(const friendsVariableData *d);

#endif /* FRIENDS_VARAIBLE_H */
