/**
 * @file friends_argument.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_ARGUMENT_H
#define FRIENDS_ARGUMENT_H

/**
 * @brief 引数データを得るのです。
 * @param d データをよこすのです。
 */
const friendsArgumentData *friendsGetArgument(friendsData *d);

/**
 * @brief 引数データを設定するのです。
 * @param dest 設定先のデータをよこすのです。
 * @param d 引数のデータ（アトム、変数、もしくはそれらのリスト）をよこすのです。
 * @param particle 助詞の文字列をよこすのです。
 * @param err NULL でない値をよこしてくれたなら、そこにエラーの情報を書き込むのです。
 */
void friendsSetArgument(friendsData *dest, friendsData *d,
                        friendsChar *particle, friendsError *err);

/**
 * @brief 引数データから助詞を得るのです。
 * @param d 引数データをよこすのです。
 * @return 助詞の文字列を返すのです。
 */
const friendsChar *friendsArgumentParticle(const friendsArgumentData *d);

/**
 * @brief 引数データからデータを得るのです。
 * @param d 引数データをよこすのです。
 * @return 引数データを返すのです。
 */
friendsData *friendsArgumentVariable(const friendsArgumentData *d);


#endif /* FRIENDS_ARGUMENT_H */
