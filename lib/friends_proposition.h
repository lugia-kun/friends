/**
 * @file friends_proposition.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_PROPOSITION_H
#define FRIENDS_PROPOSITION_H

#include "friends_defs.h"

/**
 * @brief データに入っている命題のデータを返すのです。
 * @param d データをよこすのです。
 * @return 命題データを返すのです。種類が違っている時は NULL を返すのです。
 */
const friendsPropositionData *friendsGetProposition(friendsData *d);

/**
 * @brief 命題データをセットするのです。
 * @param dest 設定先のデータをよこすのです。
 * @param verb 述語をよこすのです。
 * @param arguments 引数のリストをよこすのです。
 * @param condition 条件のリストをよこすのです。
 * @param mode この命題をどう扱うかを指定するのです。
 * @param err NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 成功した時は dest、失敗した時は NULL を返すのです。
 *
 * 引数のリストは、引数データ (`friendsArgument`) のリスト、
 * 条件のリストは、命題データ (`friendsProposition`) のリストをよこすのです。
 * 他のデータが混ざっているとエラーにするのです。
 */
friendsData *
friendsSetProposition(friendsData *dest, const friendsChar *verb,
                      friendsDataList *arguments, friendsDataList *conditions,
                      friendsPropositionMode mode, friendsError *e);

/**
 * @brief 引数リストを得るのです。
 * @param p 命題データをよこすのです。
 * @return 引数リストを返すのです。
 *
 * 返したリストを削除してはいけないのです。
 */
friendsDataList *friendsPropositionArguments(const friendsPropositionData *p);

/**
 * @brief 条件リストを得るのです。
 * @param p 命題データをよこすのです。
 * @return 条件リストを返すのです。
 *
 * 返したリストを削除してはいけないのです。
 */
friendsDataList *friendsPropositionConditions(const friendsPropositionData *p);

/**
 * @brief 停止条件にあるかを得るのです。
 * @param p 命題データをよこすのです。
 * @return 停止条件かどうかを返すのです。
 */
friendsBool friendsPropositionIsStop(const friendsPropositionData *p);

/**
 * @brief 質問文かを得るのです。
 * @param p 命題データをよこすのです。
 * @return 疑問文かどうかを返すのです。
 */
friendsBool friendsPropositionIsQuery(const friendsPropositionData *p);

/**
 * @brief 述語を得るのです。
 * @param p 命題データをよこすのです。
 * @return 述語を返すのです。
 */
const friendsChar *friendsPropositionVerb(const friendsPropositionData *p);

#endif /* FRIENDS_PROPOSITION_H */
