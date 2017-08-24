/**
 * @file friends_io.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_IO_H
#define FRIENDS_IO_H

#include <stdio.h>
#include <stdarg.h>

#include "friends_defs.h"

/**
 * @brief 文字列をフォーマットするのです。
 * @param buf ここに必要なメモリを確保して文字列をフォーマットするので
 *        す。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ... フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * 基本的には C 言語の printf 関数と互換性があるのです。違うところは以
 * 下の通りなのです。
 *
 * - 浮動小数点数の変換は（まだ）対応していないのです。
 * - C 言語の文字列型を必要とする箇所はすべて `friendsUnescapeStringLiteral`
 *   を通してフィルタするのです（フォーマット、引数両方）。
 * - `%lc` は Unicode の文字を埋め込めるのです。`int` 型の変数や値をよ
     こすのです。
 * - `%ls` は `friendsChar` の文字列を埋め込むのです。
 * - 文字列の幅と精度はすべて「文字数」でカウントするのです。
 * - `%n` は対応しないのです。
 * - `%#d` （10進の整数変換に `#` 指示）は漢数字というもので印字するの
 *   です。この時の幅と精度は文字列出力に対する扱いと同等になるのです。
 * - フォーマット結果は機種依存なので、システムの printf 関数とは違う
 *   結果になる場合があるのです。我々は BSD の printf 関数の実装を参考
 *   にしたのです。
 */
int friendsAsprintF(friendsChar **buf, friendsError *err,
                    const friendsChar *format, ...);

/**
 * @brief 文字列をフォーマットするのです。
 * @param buf ここに必要なメモリを確保して文字列をフォーマットするので
 *            す。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ap フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 */
int friendsAsprintV(friendsChar **buf, friendsError *err,
                    const friendsChar *format, va_list ap);

/**
 * @brief 文字列をフォーマットするのです。
 * @param buf ここに必要なメモリを確保して文字列をフォーマットするので
 *            す。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ... フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 */
int friendsAsprintCF(friendsChar **buf, friendsError *err,
                     const char *format, ...);

/**
 * @brief 文字列をフォーマットするのです。
 * @param buf ここに必要なメモリを確保して文字列をフォーマットするので
 *            す。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ap フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 */
int friendsAsprintCV(friendsChar **buf, friendsError *err,
                     const char *format, va_list ap);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ... フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * `fp` に指定されている文字コードで出力するのです。
 */
int friendsFprintF(friendsFile *fp, friendsError *err,
                   const friendsChar *format, ...);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ap フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * `fp` に指定されている文字コードで出力するのです。
 */
int friendsFprintV(friendsFile *fp, friendsError *err,
                   const friendsChar *format, va_list ap);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ... フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * `fp` に指定されている文字コードで出力するのです。
 */
int friendsFprintCF(friendsFile *fp, friendsError *err,
                    const char *format, ...);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ap フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * `fp` に指定されている文字コードで出力するのです。
 */
int friendsFprintCV(friendsFile *fp, friendsError *err,
                    const char *format, va_list ap);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ... フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * 端末に指定されている文字コードで出力するのです。
 *
 * @relate friendsSetTerminalEncoding
 */
int friendsPrintF(FILE *fp, friendsError *err,
                  const friendsChar *format, ...);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ap フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * 端末に指定されている文字コードで出力するのです。
 *
 * @relate friendsSetTerminalEncoding
 */
int friendsPrintV(FILE *fp, friendsError *err,
                  const friendsChar *format, va_list ap);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ... フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * 端末に指定されている文字コードで出力するのです。
 *
 * @relate friendsSetTerminalEncoding
 */
int friendsPrintCF(FILE *fp, friendsError *err,
                   const char *format, ...);

/**
 * @brief 文字列を出力するのです。
 * @param fp ファイルポインタをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param format フォーマットする文字列をよこすのです。
 * @param ap フォーマットするデータをよこすのです。
 * @return 書き込んだ配列要素数を返すのです。
 *
 * 端末に指定されている文字コードで出力するのです。
 *
 * @relate friendsSetTerminalEncoding
 */
int friendsPrintCV(FILE *fp, friendsError *err,
                   const char *format, va_list ap);

/**
 * @brief 文字列を一行読み込むのです。
 * @param buf ここに必要なだけメモリを確保して保存するのです。
 * @param fp 読み込み元をよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @return 読み込んだ配列要素数を返すのです。
 *
 * 端末に指定されている文字コードで入力するのです。
 *
 * @relate friendsSetTerminalEncoding
 */
int friendsGetLine(friendsChar **buf, FILE *fp, friendsError *err);

#endif
