/**
 * @file friends_etcetra.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_ETCETRA_H
#define FRIENDS_ETCETRA_H

#include "friends_defs.h"

/**
 * @brief データに、リストを省略していることを示すデータをセットするのです。
 * @param dest データをよこすのです。
 * @param err NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 *
 * この種類にはデータがないので、データを得る関数はないのです。
 * `friendsGetType` 関数を使って型を調べるのです。
 */
void friendsSetEtcetra(friendsData *d, friendsError *err);

#endif /* FRIENDS_ETCETRA_H */
