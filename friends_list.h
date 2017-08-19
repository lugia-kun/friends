/**
 * @file friends_list.h
 *
 * 型 `friendsDataList` には暗に2種類のデータがあるのです。この判別は
 * `friendsListIsParent` 関数を使うのです。
 *
 * 1つ目は、全体を制御する親なのです。
 * `friendsNewList` 関数はこれを返すのです。
 *
 * 2つ目は、リストのデータを保持する子なのです。
 * `friendsListNext`/`friendsListPrev` 関数などで使われるのです。
 * データの設定は子にしかできないのです。
 *
 * ちなみに、リストのデータは単なるポインタでしかなくて、リストの操作自体には、
 * `friendsData` のデータを利用することは無いので、ある種の合意が取れれば
 * 別のデータを入れても安全なのです。
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_LIST_H
#define FRIENDS_LIST_H

#include "friends_defs.h"

/**
 * @brief 新しいリストを作るのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を保存するのです。
 * @return 確保したリストのポインタを渡すのです。
 */
friendsDataList *friendsNewList(friendsError *e);

/**
 * @brief リスト全体を削除するのです。
 * @param l 削除するリストをよこすのです。
 *
 * `l` は親でも子でも良いのです。いずれの場合もリスト全体を削除するの
 * です。
 *
 * リストの中のデータは削除しないのです。
 */
void friendsDeleteList(friendsDataList *l);

/**
 * @brief 次の要素を得るのです。
 * @param l 対象の要素をよこすのです。
 * @return 次の要素を返すのです。末尾の場合は、NULL を返すのです。
 *
 * `l` が親の場合は最初の子を返すのです。
 */
friendsDataList *friendsListNext(friendsDataList *l);

/**
 * @brief 前の要素を得るのです。
 * @param l 対象の要素をよこすのです。
 * @return 前の要素を返すのです。先頭の場合は、NULL を返すのです。
 *
 * `l` が親の場合は最後の子を返すのです。
 */
friendsDataList *friendsListPrev(friendsDataList *l);

/**
 * @brief 要素数を得るのです。
 * @param l リストをよこすのです。
 * @return 要素数を返すのです。
 */
size_t friendsListSize(friendsDataList *l);

/**
 * @brief 要素のデータを得るのです。
 * @param l 対象の要素をよこすのです。
 * @return データを返すのです。
 *
 * `l` が親の時は NULL を返すのです。
 */
friendsData *friendsListData(friendsDataList *l);

/**
 * @brief 要素のデータをセットするのです。
 * @param l セット先の要素をよこすのです。
 * @param d セットするデータをよこすのです。
 *
 * `l` には必ず子をよこすのです。
 */
void friendsSetListData(friendsDataList *l, friendsData *d);

/**
 * @brief 要素が先頭かどうかを判定すのるです。
 * @param l 検証する要素をよこすのです。
 * @return 先頭なら、`friendsTrue`、それ以外なら `friendsFalse` を返すのです。
 *
 * `l` が親の場合は `friendsFalse` を返すのです。
 */
friendsBool friendsListIsBegin(friendsDataList *l);

/**
 * @brief 要素が末尾かどうかを判定するのです。
 * @param l 検証する要素をよこすのです。
 * @return 末尾なら、`friendsTrue`、それ以外なら `friendsFalse` を返すのです。
 *
 * `l` が親の場合は `friendsFalse` を返すのです。
 */
friendsBool friendsListIsEnd(friendsDataList *l);

/**
 * @brief 要素が親か子かを判定するのです。
 * @pararm l 検証する要素をよこすのです。
 * @return 親なら `friendsTrue` を、子なら `friendsFalse` を返すのです。
 */
friendsBool friendsListIsParent(friendsDataList *l);

/**
 * @brief 先頭の要素を返すのです。
 * @param l リスト（中の1要素）をよこすのです。
 * @return 先頭の要素を返すのです。
 *
 * `l` は親でも子でも良いのです。
 */
friendsDataList *friendsListBegin(friendsDataList *l);

/**
 * @brief 末尾の要素を返すのです。
 * @param l リスト（中の1要素）をよこすのです。
 * @return 末尾の要素を返すのです。
 *
 * `l` は親でも子でも良いのです。
 */
friendsDataList *friendsListEnd(friendsDataList *l);

/**
 * @brief 親を返すのです。
 * @param l 要素をよこすのです。
 * @return 親を返すのです。
 *
 * `l` は親でも子でも良いのです。当然、親の場合はそのまま返すのです。
 */
friendsDataList *friendsListParent(friendsDataList *l);

/**
 * @brief データを検索するのです。
 * @param l 検索開始場所の要素をよこすのです。
 * @param d 比較するデータをよこすのです。
 * @return 見つかった場所の要素を返すのです。
 *
 * `l` は親でも子でも良いのです。親の場合は、先頭から検索するのです。
 *
 * この関数での比較は、同じオブジェクト（アドレス）を見つけるのです。
 */
friendsDataList *friendsListFind(friendsDataList *l, friendsData *d);

/**
 * @brief データを検索するのです。
 * @param l 検索開始場所の要素をよこすのです。
 * @param f 条件を判定するための関数をよこすのです。
 * @param a 関数に渡す追加の引数を（あれば）よこすのです。
 * @return 見つかった場所の要素を返すのです。
 *
 * `l` は親でも子でも良いのです。親の場合は、先頭から検索するのです。
 */
friendsDataList *friendsListFindIf(friendsDataList *l,
                                   friendsBool (*f)(friendsData *d, void *a),
                                   void *a);

/**
 * @brief 全要素に対して処理を行うのです。
 * @param l リストをよこすのです。
 * @param f 処理する関数をよこすのです。
 * @param a 関数に渡す追加の引数を（あれば）よこすのです。
 * @return 見つかった場所の要素を返すのです。
 *
 * `l` は親でも子でも良いのです。親でも子でも、先頭からすべて処理するのです。
 *
 * 処理関数から friendsFalse を返すと処理を中止 (break) できるのです。
 *
 * データが NULL の要素には、処理関数は呼ばれないのです。
 */
void friendsListEach(friendsDataList *l,
                     friendsBool (*f)(friendsData *d, void *a),
                     void *a);

/**
 * @brief 全要素に対して処理を行うのです。
 * @param l リストをよこすのです。
 * @param f 処理する関数をよこすのです。
 * @param a 関数に渡す追加の引数を（あれば）よこすのです。
 * @return 見つかった場所の要素を返すのです。
 *
 * `l` は親でも子でも良いのです。親でも子でも、先頭からすべて処理するのです。
 *
 * 処理関数から friendsFalse を返すと処理を中止 (break) できるのです。
 *
 * データが NULL の場合も処理関数が呼ばれるのです。
 */
void friendsListEachItem(friendsDataList *l,
                         friendsBool (*f)(friendsDataList *l, void *a),
                         void *a);

/**
 * @brief データを挿入するのです。
 * @param l 挿入位置をよこすのです。
 * @param d 挿入するデータをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を保存するのです。
 * @return 挿入した要素を返すのです。失敗した時は NULL を返すのです。
 *
 * `l` の前にデータを追加するのです。`l` が親の場合は先頭に追加するのです。
 */
friendsDataList *friendsListInsert(friendsDataList *l,
                                   friendsData *d, friendsError *e);

/**
 * @brief データを追加するのです。
 * @param l リストをよこすのです。
 * @param d 追加するデータをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を保存するのです。
 * @return 追加した要素を返すのです。失敗した時は NULL を返すのです。
 *
 * この関数は必ず末尾に追加するのです。`l` は親でも子でも良いのです。
 */
friendsDataList *friendsListAppend(friendsDataList *l,
                                   friendsData *d, friendsError *e);

/**
 * @brief 要素を削除するのです。
 * @param l 削除する要素をよこすのです。
 *
 * `l` は必ず子をよこすのです。
 */
void friendsListRemove(friendsDataList *l);

/**
 * @brief 指定された場所の要素を削除するのです。
 * @param l 要素の基準位置をよこすのです。
 * @param index 削除する場所を相対値でよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 *
 * index が 0 なら、指定した要素を、正の値なら前方に、負の数なら後方に
 * カウントするのです。最後と最初は超えないのです。例えば 2 番目の要素
 * から -3 の位置の削除はエラーになるのです。
 *
 * `l` が親の場合は、正の値は先頭から、負の値は末尾からカウントするの
 * です。index が 0 のとき、先頭の要素になるのです。
 */
void friendsListRemoveAt(friendsDataList *l,
                         int index, friendsError *e);

/**
 * @brief データにリストを設定するのです。
 * @param dest 設定先のデータをよこすのです。
 * @param l 設定するリストをよこすのです。
 * @param e NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 *
 * `l` は親でも子でも良いのです。ただし、実際に設定するデータは親になるのです。
 */
void friendsSetList(friendsData *dest, friendsDataList *l, friendsError *err);

/**
 * @brief データからリストを得るのです。
 * @param d データをよこすのです。
 */
friendsDataList *friendsGetList(friendsData *d);

#endif
