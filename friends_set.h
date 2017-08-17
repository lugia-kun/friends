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
 *
 * セットの中のデータは捨てないのです。
 */
void friendsDeleteSet(friendsDataSet *set);

/**
 * @brief データセットの中から、指定したテキストのデータを見つけるのです。
 * @param set 探すデータセットをよこすのです。
 * @param type 見つけるデータの種類をよこすのです。
 * @param text 見つけるデータの文字列をよこすのです。
 * @return 見つかったデータのリストを返すのです。見つからない時は NULL を返すのです。
 */
friendsDataList *friendsSetFindText(friendsDataSet *set,
                                    friendsType type, const friendsChar *text);

/**
 * @brief データセットの中から、指定した値のアトムを見つけるのです。
 * @param set 探すデータセットをよこすのです。
 * @param a 探す値をよこすのです。
 * @return 見つかったデータのリストを返すのです。見つからない時は NULL を返すのです。
 */
friendsDataList *friendsSetFindNumericAtom(friendsDataSet *set, int a);

/**
 * @brief データセットにデータを追加するのです。
 * @param set 探すデータセットをよこすのです。
 * @param d 追加するデータをよこすのです。
 * @param e NULLでない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 */
void friendsSetInsert(friendsDataSet *set, friendsData *d, friendsError *e);

/**
 * @brief データセットから最初に見つかったデータを削除するのです。
 * @param set 探すデータセットをよこすのです。
 * @param d 削除するデータをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 *
 * この関数は、d のハッシュ値を信用して検索するのです。
 *
 * もし見つからなくてもエラーは記録しないのです。
 */
void friendsSetRemove(friendsDataSet *set, friendsData *d, friendsError *e);

/**
 * @brief データセットからデータを全部削除するのです。
 * @param set 探すデータセットをよこすのです。
 * @param d 削除するデータをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 *
 * この関数は、セット全体を検索して削除するのです。
 *
 * もし見つからなくてもエラーは記録しないのです。
 */
void friendsSetRemoveAll(friendsDataSet *set, friendsData *d, friendsError *e);

/**
 * @brief データセット内のデータを順に処理するのです。
 * @param set データセットをよこすのです。
 * @param f 処理する関数をよこすのです。
 * @param a 関数に渡したい追加データがあるなら、よこすのです。
 */
void friendsSetEach(friendsDataSet *set,
                    friendsBool (*f)(friendsDataList *list, void *a),
                    void *a);

typedef unsigned char friendsDataSetHash;

/**
 * @brief データセット用にハッシュ値をさらに小さくするのです。
 * @param h 潰す値をよこすのです。
 * @return 潰した値を返すのです。
 */
friendsDataSetHash friendsSmallHash(friendsHash h);

#endif /* FRIENDS_SET_H */
