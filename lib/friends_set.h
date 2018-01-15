/**
 * @file friends_set.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_SET_H
#define FRIENDS_SET_H

#include "friends_defs.h"

/**
 * @brief 新しいデータセットを作るのです。
 * @param e NULL でない値をよこしてくれたなら、ここにエラーの情報を書き込むのです。
 * @return 作ったデータセットを返すのです。作れなかった時は NULL を返すのです。
 */
friendsDataSet *friendsNewSet(friendsError *e);

/**
 * @brief 作ったデータセットを捨てるのです。
 * @param set 捨てるデータセットをよこすのです。
 */
void friendsDeleteSet(friendsDataSet *set);

friendsDataSetNode *
friendsSetFindByNode(friendsDataSet *set, friendsDataSetNode *node);

friendsDataSetNode *
friendsSetFindByData(friendsDataSet *set, const friendsData *d);


/**
 * @brief データセットの中から、指定したテキストのデータを見つけるのです。
 * @param set 探すデータセットをよこすのです。
 * @param type 見つけるデータの種類をよこすのです。
 * @param text 見つけるデータの文字列をよこすのです。
 * @return 見つかったデータのリストを返すのです。見つからない時は NULL を返すのです。
 */
friendsDataSetNode *
friendsSetFindByText(friendsDataSet *set, friendsType type,
                     const friendsChar *text, friendsError *e);

/**
 * @brief データセットの中から、指定した値のアトムを見つけるのです。
 * @param set 探すデータセットをよこすのです。
 * @param a 探す値をよこすのです。
 * @return 見つかったデータのリストを返すのです。見つからない時は NULL を返すのです。
 */
friendsDataSetNode *
friendsSetFindNumericAtom(friendsDataSet *set, int a, friendsError *e);

/**
 * @brief データセットにデータを追加するのです。
 * @param set 探すデータセットをよこすのです。
 * @param d 追加するデータをよこすのです。
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 */
friendsDataSetNode *
friendsSetInsert(friendsDataSet *set, friendsData *d, friendsError *e);

friendsDataSetNode *
friendsSetInsertMulti(friendsDataSet *set, friendsData *d, friendsError *e);

/**
 * @brief データセットから最初に見つかったデータを削除するのです。
 * @param set 探すデータセットをよこすのです。
 * @param d 削除するデータをよこすのです。
 */
void friendsSetRemove(friendsDataSet *set, friendsData *d);

/**
 * @brief データセットからデータを全部削除するのです。
 * @param set 探すデータセットをよこすのです。
 * @param d 削除するデータをよこすのです。
 */
void friendsSetRemoveAll(friendsDataSet *set, friendsData *d);

/**
 * @brief データセット内のデータを順に処理するのです。
 * @param set データセットをよこすのです。
 * @param f 処理する関数をよこすのです。
 * @param a 関数に渡したい追加データがあるなら、よこすのです。
 */
void friendsSetEach(friendsDataSet *set,
                    friendsBool (*f)(friendsDataList *list, void *a),
                    void *a);

#endif /* FRIENDS_SET_H */
