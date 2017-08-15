/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_LEXER_H
#define FRIENDS_LEXER_H

#include "friends_defs.h"

/**
 * @brief Friends 言語を読むのです。
 * @param park パークをよこすのです。
 * @param str 読む文字列をよこすのです。
 * @param fillFunc 文字列をよこさない (NULL) なら、代わりに続きを読むための関数をよこすのです。
 * @param fillarg  fillFunc に渡せるデータを一つよこすのです。
 * @param err NULL ではない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 *
 * `str` が `NULL` ではないなら、我々は `fillFunc` を無視するです。
 *
 * `w` が `NULL` もしくは、`str` と `fillFunc` の両方が `NULL` のとき
 * は、`e` に `INVAL` を設定するです。
 * 記憶できるスペースが無い時は、`e` に `NOMEM` を設定するです。
 */
void friendsParse(friendsPark *park, const friendsChar *str,
                  friendsFillFunc *fillfunc, void *fillarg,
                  friendsError *err);

#endif
