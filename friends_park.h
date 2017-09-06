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
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 新しいパークの場所を返すのです。失敗した時は、NULL を返すの
 *         です。
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
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 * @param deleter ポインタを捨てるときに使う関数をよこすのです。
 * @return 追加したポインタを返すのです。失敗した時は NULL を返すので
 *         す。
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
 * @brief パークに登録されている命題データの中から、指定した述語の命題
 *        を集めてくるのです。
 * @param park パークをよこすのです。
 * @param verb 述語をよこすのです。
 * @return 集めた命題のリストを返すのです。存在しない時は、NULL を返すのです。
 */
friendsDataList *friendsPropositionListInPark(friendsPark *park,
                                              const friendsChar *verb);

/**
 * @brief フレンズ言語を解読する装置を作るのです。
 * @param park パークをよこすのです。
 * @param err NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 解読装置を返すのです。作成に失敗した時は、NULL を返すのです。
 *
 * パークの中になかったら、新しく作るのです。
 *
 * 返したポインタは、friendsDeletePark か friendsDeleteParser を呼び出
 * すまで使えるのです。
 *
 * フレンズ言語の解読の仕方を説明するのです。
 *
 * 1. まず、この関数を使って解読装置を得るのです。
 *
 * 2. 適当な長さの文字列を読み込んで、friendsAppendParseText 関数で解
 *    読装置にセットするのです。
 *
 * 3. 解読装置を friendsLex 関数に渡すのです。friendsLex 関数は、その
 *    文字列を単語に区切って解読装置に設定するのです。
 *
 * 4. 解読装置を friendsParse 関数に渡すと分解した単語の組み合わせから、
 *    情報を得るのです。
 *
 * * エラーの時は、friendsResetParser 関数でリセットできるのです。こ
 *   の時、読みかけの情報は捨てられてしまうのです。リセットした後は、
 *   2. からやり直すのです。
 *
 * * 3. で、「続きが必要」と言われたときも、4. は実行しておくと良いのです。
 */
friendsParser *friendsGetParser(friendsPark *park, friendsError *err);

/**
 * @brief フレンズ言語を解読する装置を作り直すのです。
 * @param park パークをよこすのです。
 * @param err NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 新しい解読装置を返すのです。作成に失敗した時とパーク内に無
 *         い時は、NULL を返すのです。
 *
 * 返したポインタは、friendsDeletePark か friendsResetParser を呼び出
 * すまで使えるのです。
 */
friendsParser *friendsResetParser(friendsPark *park, friendsError *err);

#endif
