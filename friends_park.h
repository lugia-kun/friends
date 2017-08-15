/**
 * @file friends_park.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_PARK_H
#define FRIENDS_PARK_H

#include "friends_defs.h"

/**
 * @brief 新しいパークを作るのです。
 * @param e NULL ではない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 新しいパークの場所を返すのです。失敗した時は、NULL を返すのです。
 *
 * 記憶できるスペースが無い時は、`e` に NOMEM を設定するです。
 */
friendsPark *friendsNewPark(friendsError *e);

/**
 * @brief 使い終わったパークを捨てるのです。
 * @param park 捨ててしまうパークをよこすのです。
 */
void friendsDeletePark(friendsPark *park);

/**
 * @brief ポインタをパークの管理下に置くのです。
 * @param park パークをよこすのです。
 * @param p 管理させるポインタをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @param deleter ポインタを捨てるときに使う関数をよこすのです。
 * @return 追加したポインタを返すのです。失敗した時は NULL を返すのです。
 */
void *friendsAddPointer(friendsPark *park, void *p,
                        friendsPointerDeleter *deleter,
                        friendsError *e);

/**
 * @brief ポインタをパークの管理下から取り除くのです。
 * @param park パークをよこすのです。
 * @param p 取り除くポインタをよこすのです。
 *
 * この関数をパークの削除中に呼び出した時は、何もしないのです。
 */
void friendsDeletePointer(friendsPark *park, void *p);

#endif
