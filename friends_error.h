/**
 * @file friends_error.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_ERROR_H
#define FRIENDS_ERROR_H

#include <stdarg.h>
#include <stddef.h>
#include <errno.h>

#include "friends_defs.h"

/**
 * @macro friendsExpect
 * @brief よこした値が成り立っていることを前提とした最適化をできるよう
 *        にするのです。
 * @param cond 条件式を入れるのです。
 * @param var  比較値を入れるのです。
 *
 * コンパイラが対応していれば、`cond` が常に `var` であることを期待し
 * て最適化を行うのです。
 */
#ifdef __GNUC__
#define friendsExpect(cond, var) (__builtin_expect((cond), (var)))
#else
#define friendsExpect(cond, var) (cond)
#endif

/**
 * @brief エラーメッセージを表示するのです。
 * @param elvl エラーの程度をよこすのです。
 * @param filename 必要なら、エラーが起きたファイル名をよこすのです。
 * @param line 必要ならエラーが起きた行番号をよこすのです。不要な時は
 *        0 以下をよこすのです。
 * @param column 必要ならエラーが起きた列番号をよこすのです。不要な時
 *        は 0 以下をよこすのです。
 * @param format メッセージを `friendsAsprintCV` に渡す形式でよこすの
 *        です。
 *
 * この関数の処理中にエラーが起きた時は、その内容を我々にはよくわから
 * ない言葉で出力するのです。
 */
void friendsPrintErrorF(friendsErrorLevel elvl,
                        const char *filename, long int line, long int column,
                        const char *format, ...);

/**
 * @brief エラーメッセージを表示するのです。
 * @param elvl エラーの程度をよこすのです。
 * @param filename 必要なら、エラーが起きたファイル名をよこすのです。
 * @param line 必要ならエラーが起きた行番号をよこすのです。不要な時は
 *        0 以下をよこすのです。
 * @param column 必要ならエラーが起きた列番号をよこすのです。不要な時
 *        は 0 以下をよこすのです。
 * @param format メッセージを `friendsAsprintCV` に渡す形式でよこすの
 *        です。
 *
 * この関数の処理中にエラーが起きた時は、その内容を我々にはよくわから
 * ない言葉で出力するのです。
 */
void friendsPrintErrorV(friendsErrorLevel elvl,
                        const char *filename, long int line, long int column,
                        const char *format, va_list ap);

/**
 * @brief エラーメッセージを表示するのです。
 * @param elvl エラーの程度をよこすのです。
 * @param filename 必要なら、エラーが起きたファイル名をよこすのです。
 * @param line 必要ならエラーが起きた行番号をよこすのです。不要な時は
 *        0 以下をよこすのです。
 * @param column 必要ならエラーが起きた列番号をよこすのです。不要な時
 *        は 0 以下をよこすのです。
 * @param ainfo メッセージを `friendsAsprintV` に渡す形式でよこすので
 *        す。
 *
 * この関数の処理中にエラーが起きた時は、その内容を我々にはよくわから
 * ない言葉で出力するのです。
 */
void friendsPrintError(friendsErrorLevel elvl,
                       const char *filename, long int line, long int column,
                       const char *ainfo, friendsError e);

/**
 * @brief エラーメッセージを表す文字列を作るのです。
 * @param buf メモリを確保して文字列を作る場所をよこすのです。
 * @param ecnv 変換するエラーの値をよこすのです。
 * @return 書き込んだ配列要素数を返すのです。エラーの時は -1 を返すの
 *         です。
 */
int friendsErrorString(friendsChar **buf, friendsError ecnv);

void friendsAssertImplement(const char *conditional, const char *fname,
                            long int lineno, const char *funcname);
void friendsUnreachableImplement(const char *fname,
                                 long int lineno, const char *funcname);

/**
 * @macro friendsAssert
 * @param cond 条件式をよこすのです。
 *
 * 指定した条件を満たしていなかったら、エラーを表示して終了させるので
 * す。
 *
 * このマクロは、デバッグ用なのです。デバッグモードでない時は、条件式
 * を満たしていなくても何もおきないのです。
 */
#ifdef NDEBUG
#define friendsAssert(cond)
#else
#define friendsAssert(cond)                              \
  do {                                                   \
    if (friendsExpect(!!(cond), 1) != 1)                 \
      friendsAssertImplement(#cond, FRIENDS_SOURCE_FILE, \
                             __LINE__, __func__);        \
  } while(0)
#endif

/**
 * @macro friendsUnrechable
 *
 * このマクロを呼び出したら、エラーを表示して終了させるのです。
 *
 * このマクロは、デバッグ用なのです。デバッグモードでない時に呼び出さ
 * れると、何が起きるかわからないのです。
 */
#ifdef NDEBUG
#ifdef __GNUC__
#define friendsUnreachable() __builtin_unreachable()
#else
#define friendsUnreachable()
#endif /* __GNUC__ */
#else
#define friendsUnreachable() \
  friendsUnreachableImplement(FRIENDS_SOURCE_FILE, __LINE__, __func__)
#endif /* NDEBUG */

void friendsSetErrorImplement(friendsError *errpvar, friendsError ev);

#define friendsSetError(errpvar, esym)                  \
  friendsSetErrorImplement((errpvar), friendsError##esym)

#define friendsSetErrorV(errvar, esym)                  \
  friendsSetErrorImplement(&(errvar), friendsError##esym)

void friendsSetErrorFromErrno(friendsError *errpvar, int errn);

#define friendsAnyError(errvar)                 \
  ((errvar) != friendsNoError)

#define friendsIsError(errvar, esym)            \
  ((errvar) == friendsError##esym)

void friendsClearError(friendsError *e);

#endif
