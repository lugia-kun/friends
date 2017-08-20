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
 * @brief パークが捨てられているかどうかを調べるのです。
 * @param park パークをよこすのです。
 * @return 捨てられているなら friendsTrue を返すのです。
 *
 * ここでいう「捨てられている」は `friendsDeletePark` の呼び出し中にあ
 * るかを意味するのです。例えば、各データが他の何らかのデータを所有す
 * る時、その削除関数の実装において、`friendsDeleteData` 関数を呼ぶか
 * どうかを決めなければならないのです。パークを捨てている最中にある時
 * にデータの削除関数から `friendsDeleteData` を呼ぶと二重解放になって
 * しまうのです。
 *
 * この関数を使うとそれを回避することができるのです。でも、
 * `friendsData` のデータはパークが捨てられるときには削除されるので、
 * 削除関数内で捨てる処理をしないことをおすすめするのです。
 */
friendsBool friendsParkIsDeleting(friendsPark *park);

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

/**
 * @brief データをパークに登録するのです。
 * @param park パークをよこすのです。
 * @param d 登録するデータをよこすのです。
 * @param err NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 成功なら、friendsTrue を、失敗なら friendsFalse を返すのです。
 *
 * この関数はアミメキリンさんの知識に追加する関数なのです。
 * 追加できるデータは、アトムか命題のどちらかなのです。
 */
friendsBool friendsAddDataToPark(friendsPark *park, friendsData *d,
                                 friendsError *e);

/**
 * @brief パークに登録されている命題データの中から、指定した述語の命題を集めてくるのです。
 * @param park パークをよこすのです。
 * @param verb 述語をよこすのです。
 */
friendsDataList *friendsPropositionListInPark(friendsPark *park,
                                              const friendsChar *verb);

#endif
