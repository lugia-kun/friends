/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_LEXER_H
#define FRIENDS_LEXER_H

#include "friends_defs.h"

/**
 * @brief Friends 言語を読んでトークンに分割するのです。
 * @param p 読み取り装置をよこすのです。
 */
void friendsLex(friendsParser *p);

#endif
