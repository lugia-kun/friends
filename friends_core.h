/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_CORE_H
#define FRIENDS_CORE_H

#include "friends_defs.h"

/**
 * @brief 新しい記憶域を確保するのです。
 * @param size 確保するサイズをよこすのです。
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 確保した領域を返すのです。
 *
 * 確保した領域は 0 でクリアされているのです。
 *
 * 確保した領域は必ず `friendsFree` 関数で解放するのです。`free` 関数
 * で解放すると予期しない動作になってしまうのです。
 *
 * とおどしてみたものの、今のところは C 言語の calloc 関数と何も変わら
 * ないのです。
 */
void *friendsMalloc(size_t size, friendsError *e);

/**
 * @brief 確保した記憶域を捨てるのです。
 * @param p 捨てる記憶域をよこすのです。
 *
 * 今のところは、C 言語の free 関数と何も変わらないのです。
 */
void friendsFree(void *p);

#endif
