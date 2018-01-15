/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_DATA_H
#define FRIENDS_DATA_H

#include "friends_defs.h"

/**
 * @brief 新しいデータを作るのです。
 * @param park パークをよこすのです。
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 新しいデータの場所を返すのです。失敗した時は、NULL を返すの
 *         です。
 *
 * `park` が NULL の時は、`e` に INVAL を設定するです。
 * 記憶できるスペースが無い時は、`e` に NOMEM を設定するです。
 */
friendsData *friendsNewData(friendsPark *park, friendsError *e);

/**
 * @brief データを削除するのです。
 * @param d 削除するデータをよこすのです。
 */
void friendsDeleteData(friendsData *d);

/**
 * @brief データを解放した初期化するのです。
 * @param d 解放するデータをよこすのです。
 *
 * `d` そのものは削除しないのです。
 * `d` そのものを削除するときは `friendsDeleteData` を使うのです。
 */
void friendsDataDelink(friendsData *d);

/**
 * @brief データから所属するパークを得るのです。
 * @param d データをよこすのです。
 * @return パークを返すのです。d が NULL なら、我々も NULL を返すので
 *         す。
 */
friendsPark *friendsGetPark(const friendsData *d);

/**
 * @brief データの種類を得るのです。
 * @param d データをよこすのです。
 * @return データの種類を返すのです。
 */
friendsType friendsGetType(const friendsData *d);

/**
 * @brief データのハッシュ値を得るのです。
 * @param d データをよこすのです。
 * @return ハッシュ値を返すのです。
 */
friendsHash friendsGetHash(const friendsData *d);

/**
 * @brief データに入っている文字列を得るのです。
 * @param d データをよこすのです。
 * @return データが、アトム，変数ならその文字を、命題なら述語の文字列
 *         を返すのです。それ以外の場合はたぶん NULL を返すのです。
 */
const friendsChar *friendsDataToText(friendsData *d);

/**
 * @brief データをコピーするのです。
 * @param dest コピー先をよこすのです。
 * @param src コピー元をよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return dest を返すのです。失敗した時は、NULL を返すのです。
 */
friendsData *friendsDataCopy(friendsData *dest, const friendsData *src,
                             friendsError *e);

/**
 * @brief データ同士の比較を行うのです。
 * @param a データをよこすのです。
 * @param b データをよこすのです。
 * @return 比較結果を返すのです。
 *
 * 結果は複合値を返すことがあるのです。例えば、所属しているパークが違
 * うけど、中身が同じなら `friendsDataInAnotherPark |
 * friendsDataEqual` な値を返すのです。
 *
 * 通常の比較では `friendsDataEqual` とかの単体と比較すれば良いと思う
 * のです。
 */
friendsDataCompareResult
friendsDataCompare(const friendsData *a, const friendsData *b);

/**
 * @brief 数値をハッシュするのです。
 * @param a ハッシュする値をよこすのです。
 * @return ハッシュした値を返すのです。
 *
 * ハッシュ値は、上位 2 ビットが 00 なら数値、上位 2 ビットが 01 なら
 * バイナリデータ、最上位ビットが 1 なら文字列にしているです。でも、こ
 * のルールは型の判別に使ってはいけないのです。そして、ハッシュ値はファ
 * イルに保存しないほうが良いのです。
 *
 * 数値のハッシュ値は最上位 4 ビットが 0 なら値がそのままハッシュ値に
 * し、上位 4 ビットを 0001 にするのです。それ以外の場合は、文字列扱い
 * でハッシュされ、上位 3 ビットを 001 にするのです。
 *
 * 上位 4 ビットが 0000 になるハッシュは使われていないのです。
 */
friendsHash friendsHashNumeral(int a);

/**
 * @brief 文字列をハッシュするのです。
 * @param s ハッシュする文字列をよこすのです。
 * @param e 文字列の最後をよこすのです。
 * @return ハッシュした値を返すのです。
 *
 * UTF-8 モードでも UTF-16 モードでも 1 バイトずつの処理をするのです。
 * でも、文字列のデータをそのまま使うので、UTF-8 モードのハッシュ値と
 * UTF-16 モードのハッシュ値は異なる値になるのです。
 *
 * `e` を指定している時、`e` の指している場所は含まないのです。NULL の
 * ときは、NUL 文字までを調べるのです。
 */
friendsHash friendsHashString(const friendsChar *s, const friendsChar *e);

/**
 * @brief バイナリをハッシュするのです。
 * @param s ハッシュするデータの先頭をよこすのです。
 * @param e ハッシュするデータの最後をよこすのです。
 * @return ハッシュした値を返すのです。
 *
 * `e` の指している場所は含まないのです。
 */
friendsHash friendsHashBinary(const void *s, const void *e);

/**
 * @brief 比較結果が等しいか調べるのです。
 * @param x 比較結果をよこすのです。
 * @return 等しければ 1 を返すのです。等しくなければ 0 を返すのです。
 */
static inline
int friendsDataCompareIsEqual(friendsDataCompareResult x)
{
  return x == friendsDataEqual;
}

/**
 * @brief 比較結果が等しくないか調べるのです。
 * @param x 比較結果をよこすのです。
 * @return 等しくなければ 1 を返すのです。等しければ 0 を返すのです。
 */
static inline
int friendsDataCompareIsNotEqual(friendsDataCompareResult x)
{
  return (x != friendsDataEqual);
}

/**
 * @brief 比較結果が Set 用の比較で等しいか調べるのです。
 * @param x 比較結果をよこすのです。
 * @return 等しければ 1 を返すのです。等しくなければ 0 を返すのです。
 */
static inline
int friendsDataCompareIsSetEqual(friendsDataCompareResult x)
{
  return friendsDataCompareIsEqual(x) || ((x & friendsDataSetEqual) > 0);
}

/**
 * @brief 新しいリストを作るのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 作ったリストを返すのです。エラーの時は NULL を返すのです。
 *
 * この関数は「頭」を返すのです。
 */
friendsDataList *friendsNewDataList(friendsError *e);

/**
 * @brief 項目がデータか「頭」かを調べるのです。
 * @param item 調べる項目をよこすのです。
 * @return 項目が「頭」なら friendsTrue を返すのです。
 */
friendsBool friendsDataListIsHead(friendsDataList *item);

/**
 * @brief リストから項目を一つ消すのです。
 * @param item 消す項目をよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 *
 * `item` に「頭」を渡された時は、何もせず、`e` に `friendsErrorINVAL`
 * を設定するのです。
 */
void friendsDataListDelete(friendsDataList *item, friendsError *e);

/**
 * @brief リストの項目をすべて消すのです。
 * @param list 消すリストをよこすのです。
 */
void friendsDataListDeleteAll(friendsDataList *list);

/**
 * @brief リストが空かどうかを返すのです。
 * @param list チェックするリストをよこすのです。
 * @return リストが空なら friendsTrue を、空でなければ friendsFalse を返
 * すのです。
 */
friendsBool friendsDataListIsEmpty(friendsDataList *list);

/**
 * @brief リストに項目を追加するのです。
 * @param item 追加する場所をよこすのです。
 * @param data 追加するデータをよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 追加したリストの項目か、失敗した場合は NULL を返すのです。
 *
 * `item` の前に `data` を追加するのです。
 *
 * `item` が「頭」の場合は `friendsListAppend` と同じなのです。
 */
friendsDataList *
friendsDataListInsert(friendsDataList *item, const friendsData *data,
                      friendsError *e);

/**
 * @brief リストに項目を追加するのです。
 * @param item 追加する場所をよこすのです。
 * @param data 追加するデータをよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 追加したリストの項目か、失敗した場合は NULL を返すのです。
 *
 * `item` の後に `data` を追加するのです。
 *
 * `item` が「頭」の場合は `friendsListPrepend` と同じなのです。
 */
friendsDataList *
friendsDataListInsertAfter(friendsDataList *item, const friendsData *data,
                           friendsError *e);

/**
 * @brief リストに項目を追加するのです。
 * @param list 追加するリストをよこすのです。
 * @param data 追加するデータをよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 追加したリストの項目か、失敗した場合は NULL を返すのです。
 *
 * `list` の最後に `data` を追加するのです。
 */
friendsDataList *
friendsDataListAppend(friendsDataList *list, const friendsData *data,
                      friendsError *e);

/**
 * @brief リストに項目を追加するのです。
 * @param list 追加するリストをよこすのです。
 * @param data 追加するデータをよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 追加したリストの項目か、失敗した場合は NULL を返すのです。
 *
 * `list` の最初に `data` を追加するのです。
 */
friendsDataList *
friendsDataListPrepend(friendsDataList *list, const friendsData *data,
                       friendsError *e);

/**
 * @brief リストの次を返すのです。
 * @param item 次を求める項目をよこすのです。
 * @return 次の項目を返すのです。最後の場合は NULL を返すのです。
 */
friendsDataList *friendsDataListNext(friendsDataList *item);

/**
 * @brief リストの次を返すのです。
 * @param item 次を求める項目をよこすのです。
 * @return 前の項目を返すのです。最初の場合は NULL を返すのです。
 */
friendsDataList *friendsDataListPrev(friendsDataList *item);

/**
 * @brief リストの頭を返すのです。
 * @param item 「頭」を求める項目をよこすのです。
 * @return 「頭」を返すのです。
 */
friendsDataList *friendsDataListHead(friendsDataList *item);

/**
 * @brief 項目のデータを返すのです。
 * @param item 求める項目をよこすのです。
 * @return データを返すのです。
 *
 * 返すポインタの内容を変更してはいけないのです。
 */
friendsData *friendsDataListGetData(friendsDataList *item);

/**
 * @brief 先頭のデータを返すのです。
 * @param list 求める項目をよこすのです。
 * @return 先頭へのポインタを返すのです。
 */
friendsDataList *friendsDataListBegin(friendsDataList *list);

/**
 * @brief 末尾のデータを返すのです。
 * @param list 求める項目をよこすのです。
 * @return 末尾へのポインタを返すのです。
 */
friendsDataList *friendsDataListEnd(friendsDataList *list);

/**
 * @brief リストの各要素をリストに追加するのです。
 * @param dest 挿入する位置をよこすのです。
 * @param srcst 追加するリストの最初の要素をよこすのです。
 * @param srced 追加するリストの最後の要素をよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 成功した時は、`dest` のリストでの `srcst` にあたる要素を返す
 *         のです。失敗した時は、NULL を返すのです。
 *
 * `dest` が「頭」ならば、リストの最後に追加するのです。
 *
 * `srcst` が「頭」ならば、リストの先頭からコピーするのです。
 * `srced` が NULL や「頭」ならば、リストの最後までコピーするのです。
 *
 * もし `srcst` の後続に `srced` がなかった場合、エラーになるのです。
 */
friendsDataList *
friendsDataListInsertList(friendsDataList *dest,
                          friendsDataList *srcst, friendsDataList *srced,
                          friendsError *e);

/**
 * @brief リストを複製するのです。
 * @param list 複製するリストをよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 成功した時は、複製したリストを、失敗した時は、NULL を返すのです。
 *
 * 返すポインタは `list` にかかわらず「頭」になるのです。
 */
friendsDataList *
friendsDataListDuplicate(friendsDataList *list, friendsError *e);

/**
 * @brief データにリストを設定するのです。
 * @param data データをよこすのです。
 * @param list 設定するリストをよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 成功した時は `data` を、失敗した時は NULL を返すのです。
 *
 * `list` は「頭」でなくても良いですが、設定するのは「頭」なのです。別の
 * 言い方をすれば、リスト中の「位置」を設定することはできないのです。
 *
 * 設定する際、リストのコピーを作るので、この関数に渡した `list` とは独
 * 立するのです。
 */
friendsData *friendsSetList(friendsData *data, friendsDataList *list,
                            friendsError *e);

/**
 * @brief データに入っているリストを取り出すのです。
 * @param data データをよこすのです。
 * @param e もし NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return リストデータを返すのです。
 *
 * 返されたリストに対して `friendsDataListDeleteAll` を呼び出すと
 */
friendsDataList *friendsGetList(friendsData *data, friendsError *e);

#endif /* FRIENDS_DATA_H */
