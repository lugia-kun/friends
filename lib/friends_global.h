/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_GLOBAL_H
#define FRIENDS_GLOBAL_H

#include "friends_config.h"
#include "friends_defs.h"

#if defined(LIBEDIT_FOUND) && defined(FRIENDS_ENABLE_LIBEDIT)
#include <histedit.h>
#endif

/**
 * @brief 端末が使っている文字コードを返すのです。
 */
const friendsCodeSet *friendsGetTerminalEncoding(void);

/**
 * @brief 端末が使う文字コードを設定するのです。
 */
void friendsSetTerminalEncoding(const friendsCodeSet *set);


/**
 * @brief Friends 言語を実行するための環境を用意するのです。
 * @param argc 引数の数をよこすのです。
 * @param argv 引数の配列をよこすのです。
 */
void friendsInit(int argc, char **argv);

/**
 * @brief Friends の環境の後始末をするのです。
 */
void friendsFinalize(void);

#if defined(FRIENDS_ENABLE_LIBEDIT)
/**
 * @brief libedit の制御データを返すのです。
 * @return libedit の制御データを返すのです。
 */
EditLine *friendsGetEditLine(void);
#endif

#endif
