/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_GLOBAL_H
#define FRIENDS_GLOBAL_H

#include "friends_defs.h"

/**
 * @brief 端末が使っている文字コードを返すのです。
 */
const friendsCodeSet *friendsGetTerminalEncoding(void);

/**
 * @brief 端末が使う文字コードを設定するのです。
 */
void friendsSetTerminalEncoding(const friendsCodeSet *set);

#endif
