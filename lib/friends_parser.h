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
 * @return 解読装置を返すのです。作成に失敗した時とパーク内にすでに存
 *         在する時は、NULL を返すのです。
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

/**
 * @brief 解読装置に読んでもらう文章を追加するのです。
 * @param parser 解読装置をよこすのです。
 * @param text 読んでもらう文章をよこすのです。
 * @return 問題なく追加できたら、friendsTrue を、失敗したときは、
 *         friendsFalse を返すのです。
 *
 * `text` はコピーして追加するので、追加したあと解放して良いのです。
 */
friendsBool
friendsAppendParseText(friendsParser *parser, const friendsChar *text);

/**
 * @brief 解読を行うのです。
 * @param parser 解読装置をよこすのです。
 * @return 問題なく解読できれば friendsTrue を返すのです。
 *
 * 解読の際は、以下の場合を除いて最後の1つのトークンは残して処理するの
 * です。
 *
 * 最後のトークンが、
 *   - プログラムを終了させる friendsEND の時。
 *   - 「なんだっけ？」friendsNANDAKKE の時。
 *   - 「なんだね！」  friendsNANDANE  の時。
 *   - 「たーのしー！」friendsTANOSHI  の時。
 */
friendsBool friendsParse(friendsParser *parser);

/**
 * @brief 解読装置に記録されたエラー情報を返すのです。
 * @param parser 解読装置をよこすのです。
 * @return エラー情報を返すのです。
 *
 * なお、ユーザーへの通知は、基本的にその場で行っているのです。
 */
friendsError friendsGetParserError(friendsParser *parser);

/**
 * @brief 解読装置に記録されたエラー情報を消去するのです。
 *
 * この関数はそれ以外のことはしないのです。
 */
void friendsClearParserError(friendsParser *parser);

/**
 * @brief 解読装置の現在の行位置を得るのです。
 * @param parser 解読装置をよこすのです。
 * @return 行位置を返すのです。
 */
long friendsParserCurrentLine(friendsParser *parser);

/**
 * @brief 解読装置の現在位置の行頭からの文字数を得るのです。
 * @param parser 解読装置をよこすのです。
 * @return 文字数を返すのです。
 */
long friendsParserCurrentColumn(friendsParser *parser);

/**
 * @brief 解読装置が読んでいるファイルの名前を指定するのです。
 * @param parser 解読装置をよこすのです。
 * @param filename ファイル名をよこすのです。
 *
 * この関数を呼び出すと、現在位置を 1 行 1 文字目にリセットするのです。
 *
 * ファイル名は、エラーの出力にだけ使われるのです。
 */
void friendsSetParserFileName(friendsParser *parser, const char *filename);

/**
 * @brief 解読装置が読んでいるファイルの名前を得るのです。
 * @param parser 解読装置をよこすのです。
 * @return 設定されているファイル名をよこすのです。
 *
 * 設定されていない状態では、"-" を返すのです。
 */
const char *friendsParserFileName(friendsParser *parser);

/**
 * @brief 解読装置の状態を得るのです。
 * @param parser 解読装置をよこすのです。
 * @return 状態を返すのです。
 */
friendsLexerState friendsGetLexerState(friendsParser *parser);

/**
 * @brief 解読装置が読み取ったトークンをメッセージに表示するのです。
 * @param parser 解読装置をよこすのです。
 *
 * この関数はデバッグ向けなのです。
 */
void friendsPrintTokens(friendsParser *parser);

#endif /* FRIENDS_PARSER_H */
