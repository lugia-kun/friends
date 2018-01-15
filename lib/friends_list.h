/**
 * @file friends_list.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_LIST_H
#define FRIENDS_LIST_H

#include "friends_defs.h"

struct friendsList
{
  struct friendsList *next;
  struct friendsList *prev;
};
typedef struct friendsList friendsList;

#define friendsListContainer(ptr, type, field)      \
  ((type *)((char *)ptr - offsetof(type, field)))

/**
 * @brief リストデータを初期化するのです。
 * @param l 対象の要素をよこすのです。
 */
static inline
void friendsListInit(friendsList *l)
{
  l->next = l;
  l->prev = l;
}

/**
 * @brief 次の要素を得るのです。
 * @param l 対象の要素をよこすのです。
 * @return 次の要素を返すのです。
 */
static inline
friendsList *friendsListNext(friendsList *l)
{
  return l->next;
}

/**
 * @brief 前の要素を得るのです。
 * @param l 対象の要素をよこすのです。
 * @return 前の要素を返すのです。
 */
static inline
friendsList *friendsListPrev(friendsList *l)
{
  return l->prev;
}

/**
 * @brief `prev` の次に新しい要素を追加するのです。
 * @param prev 「前」になる要素をよこすのです。
 * @param new 追加する要素をよこすのです。
 */
static inline
void friendsListInsertNext(friendsList *prev, friendsList *new)
{
  prev->next->prev = new;
  new->next = prev->next;
  new->prev = prev;
  prev->next = new;
}

/**
 * @brief `next` の前に新しい要素を追加するのです。
 * @param next 「次」になる要素をよこすのです。
 * @param new 追加する要素をよこすのです。
 */
static inline
void friendsListInsertPrev(friendsList *next, friendsList *new)
{
  next->prev->next = new;
  new->prev = next->prev;
  new->next = next;
  next->prev = new;
}

static inline
void friendsListDeleteNoInit(friendsList *item)
{
  item->prev->next = item->next;
  item->next->prev = item->prev;
}

/**
 * @brief リストから削除するのです。
 * @param item 削除する要素をよこすのです。
 */
static inline
void friendsListDelete(friendsList *item)
{
  friendsListDeleteNoInit(item);
  friendsListInit(item);
}

/**
 * @brief リストが空かを調べるのです。
 * @param head リストをよこすのです。
 * @return 空 (自分以外に要素がない) なら friendsTrue を返すのです。
 */
static inline
friendsBool friendsListIsEmpty(friendsList *head)
{
  if (head->next == head) return friendsTrue;
  return friendsFalse;
}

/**
 * @brief リストにリストを追加するのです。
 * @param dest 挿入先をよこすのです。
 * @param list 挿入するリストをよこすのです。
 *
 * `dest` の後ろに `list` を追加するのです。
 *
 * 追加する要素に `list` 自身は含まないのです。`list->next` から
 * `list->prev` までの要素を追加するのです。
 *
 * `list` に `dest` が含まれていないことはあなたが保証しなければならない
 * のです。
 */
static inline
void friendsListInsertList(friendsList *dest, friendsList *list)
{
  if (friendsListIsEmpty(list)) return;

  list->next->prev = dest;
  dest->next->prev = list->prev;
  list->prev->next = dest->next;
  dest->next = list->next;
  list->next = list;
  list->prev = list;
}

/**
 * @brief リストの各要素のループを作るのです。
 * @param p 反復子にする変数をよこすのです。
 * @param start 最初の要素をよこすのです。
 * @param end 最後の要素をよこすのです。（除外）
 *
 * `p->next` を変更するような処理をしてはいけないのです。
 */
#define friendsListForeachRange(p, start, end)   \
  for((p) = (start); (p) != (end); (p) = (p)->next)

/**
 * @brief リストの各要素のループを作るのです。（安全版）
 * @param p 反復子にする変数をよこすのです。
 * @param n `n` の次を保存しておく変数をよこすのです。
 * @param start 最初の要素をよこすのです。
 * @param end 最後の要素をよこすのです。（除外）
 */
#define friendsListForeachRangeSafe(p, n, start, end) \
  for((p) = (start), (n) = (p)->next;                       \
      (p) != (end); (p) = (n), (n) = (n)->next)

/**
 * @brief リストの各要素のループを作るのです。
 * @param p 反復子にする変数をよこすのです。
 * @param list リストをよこすのです。
 *
 * `list` に対しては呼ばれないのです。
 *
 * `p->next` を変更するような処理をしてはいけないのです。
 */
#define friendsListForeach(p, list) \
  friendsListForeachRange((p), (p)->next, (list))

/**
 * @brief リストの各要素のループを作るのです。（安全版）
 * @param p 反復子にする変数をよこすのです。
 * @param n `p` の次を保存しておく変数をよこすのです。
 * @param list リストをよこすのです。
 *
 * `list` に対しては呼ばれないのです。
 *
 * `list` の最初から最後まで処理するのです。
 */
#define friendsListForeachSafe(p, n, list) \
  friendsListForeachRangeSafe((p), (n), (list)->next, (list))

#endif
