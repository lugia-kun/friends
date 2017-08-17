/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_ENC_Utf8_H
#define FRIENDS_ENC_Utf8_H

#include "friends_defs.h"

/**
 * \brief Friends の文字コードの文字列を UTF-8 でエンコードされ
 *        た文字列に変換するのです。
 * \param buf 書き込む場所をよこすのです。
 * \param b   変換する文字列をよこすのです。
 * \param err NULL でない値をよこしてくれたなら、エラーの情報を書き込むのです。
 * \return 変換したバイト数を返すのです。エラーの時は -1 を返すのです。
 *
 * `buf` には必要な分のメモリを確保するのです。使い終わったら、`free`
 * 関数で解放するのですよ。
 */
int friendsFCharToUtf8(char **buf, const friendsChar *b, friendsError *err);

/**
 * \brief UTF-8 でエンコードされた文字列を
 *        Friends の文字コードに変換するのです。
 * \param b 書き込む場所をよこすのです。
 * \param buf 変換する文字列をよこすのです。
 * \param err NULL でない値をよこしてくれたなら、エラーの情報を書き込むのです。
 * \return 変換した配列要素数を返すのです。エラーの時は -1 を返すのです。
 *
 * `buf` は必ず NUL 文字で終端させておくのですよ。
 * `ret` には必要な分のメモリを確保するのです。使い終わったら、`free`
 * 関数で解放するのですよ。
 */
int friendsUtf8ToFChar(friendsChar **b, const char *buf, friendsError *err);

/**
 * \brief 文字列を UTF-8 でエンコードするときに1文字あたりのバイ
 *        ト数の最大値を返すのです。
 * \return 4 を返すのです。
 *
 * \note U+10FFFF まで変換できるのです。
 */
int friendsUtf8MaxChar(void);

/**
 * \brief UTF-8 の ICU ライブラリでの名前を返すのです。
 * \return NULL を返すのです。
 *
 * \note UTF-8 は ICU ライブラリのデータを使っていないのです。
 */
const char *friendsUtf8ICUName(void);

/**
 * \brief UTF-8 の IANA での名前を返すのです。
 * \return "UTF-8" を返すのです。
 */
const char *friendsUtf8IANAName(void);

/**
 * \brief UTF-8 を使うための関数群を返すのです。
 * \return UTF-8 を使うための関数群を返すのです。
 */
const friendsCodeSet *friendsUtf8Set(void);

#endif
