/**
 * @file friends_park.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_PARSER_H
#define FRIENDS_PARSER_H

#include "friends_defs.h"

/**
 * @brief 新しい解読装置を作るのです。
 * @param park パークをよこすのです。
 * @param err NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 解読装置を返すのです。作成に失敗した時とパーク内にすでに存在する時は、NULL を返すのです。
 *
 * この関数は内部向けなのです。通常は `friendsGetParser` 関数を使うのです。
 */
friendsParser *friendsNewParser(friendsPark *park, friendsError *err);

/**
 * @brief 解読装置を捨てるのです。
 * @param parser 解読装置をよこすのです。
 *
 * この関数を呼び出したあとで `friendsGetParser` 関数が呼び出されると、
 * 新しい解読装置を作るのです。
 */
void friendsDeleteParser(friendsParser *parser);

int friendsAppendParseText(friendsParser *parser, friendsChar *text);


#endif /* FRIENDS_PARSER_H */
