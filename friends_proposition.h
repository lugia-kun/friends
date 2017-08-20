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
 * @param stop 検索する時に、この文で止めるなら、friendsTrue をよこすのです。
 * @param err NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 * @return 成功した時は dest、失敗した時は NULL を返すのです。
 *
 * 引数のリストは、引数データ (`friendsArgument`) のリスト、
 * 条件のリストは、命題データ (`friendsProposition`) のリストをよこすのです。
 * 他のデータが混ざっているとエラーにするのです。
 *
 * 設定するリストと文字列はこのデータが削除される時に、削除するのです。
 * （リスト内のデータは削除されないのです）
 */
friendsData *friendsSetProposition(friendsData *dest, friendsChar *verb,
                                   friendsDataList *arguments,
                                   friendsDataList *conditions,
                                   friendsBool stop,
                                   friendsError *err);

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
 * @brief 述語を得るのです。
 * @param p 命題データをよこすのです。
 * @return 述語を返すのです。
 */
const friendsChar *friendsPropositionVerb(const friendsPropositionData *p);

#endif /* FRIENDS_PROPOSITION_H */
