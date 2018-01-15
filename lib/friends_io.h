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
 *   こすのです。
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
int friendsGetLineF(friendsChar **buf, FILE *fp, friendsError *err);

/**
 * @brief 文字列を一行読み込むのです。
 * @param buf ここに必要なだけメモリを確保して保存するのです。
 * @param fp 読み込み元をよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @return 読み込んだ配列要素数を返すのです。
 */
int friendsGetLine(friendsChar **buf, friendsFile *fp, friendsError *err);

/**
 * @brief 端末にコメントを表示して文字列を一行読み込むのです。
 * @param result ここに必要なだけメモリを確保して保存するのです。
 * @param prompt 表示するコメントをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @return 読み込んだ配列要素数を返すのです。
 *
 * @note libedit が有効な場合とそうではない場合で Unicode に変換できな
 *       い文字が入力に含まれていたときの挙動が異なるのです。
 *
 * - libedit が有効の場合: libedit を信頼するので、空の文字列を得るの
 *   です。
 *
 * - libedit が無効の場合: 認識できない文字は U+fffd に置き換えるので
 *   す。
 */
int friendsPrompt(friendsChar **result, const friendsChar *prompt,
                  friendsError *err);

/**
 * @brief 端末にコメントを表示して文字列を一行読み込むのです。
 * @param result ここに必要なだけメモリを確保して保存するのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 * @param prompt_format 表示するコメントのフォーマットをよこすのです。
 * @param ... フォーマットに使うデータをよこすのです。
 * @return 読み込んだ配列要素数を返すのです。
 *
 * フォーマットについては `friendsAsprintCV` 関数を参照するのです。
 *
 * フォーマットする都合上、`friendsPrompt` 関数と引数の順序が違うので
 * 注意するのです。
 */
int friendsPromptCF(friendsChar **result, friendsError *err,
                    const char *prompt_format, ...);

/**
 * @brief ファイルを開くのです。
 * @param pathname ファイルのパスをよこすのです。
 * @param mode ファイルの読み込みモードをよこすのです。
 * @param cset ファイルの文字コードをよこすのです。
 * @param err NULL でなければ、ここにエラーの情報を書き込むのです。
 *
 * `mode` と `pathname` の扱いは基本的に C 言語のものと同じで、そのま
 * ま `fopen` 関数に渡されるのです。
 *
 * `cset` が `NULL` の場合は、端末の文字コードが当てられるのです。
 */
friendsFile *friendsOpenFile(const char *pathname, const char *mode,
                             const friendsCodeSet *cset,
                             friendsError *err);

/**
 * @brief 開いたファイルを閉じるのです。
 * @param file ファイルをよこすのです。
 */
void friendsCloseFile(friendsFile *file);

/**
 * @brief ファイルの終端に達しているか調べるのです。
 * @param file ファイルをよこすのです。
 * @return ファイルの末尾に達していれば、`friendsTrue` を返すのです。
 *
 * この関数は `feof` の `friendsFile` 用なのです。
 */
friendsBool friendsFileEOF(friendsFile *file);

/**
 * @brief ファイルがエラーになっているか調べるのです。
 * @param file ファイルをよこすのです。
 * @return ファイルでエラーになっていたら、`friendsTrue` を返すのです。
 *
 * この関数は `ferror` の `friendsFile` 用なのです。
 */
friendsBool friendsFileError(friendsFile *file);

/**
 * @brief ファイルのエラー情報を消去するのです。
 * @param file ファイルをよこすのです。
 *
 * この関数は `clearerr` の `friendsFile` 用なのです。
 */
void friendsFileClearError(friendsFile *file);

/**
 * @brief C のファイルを friendsFile に変換するのです。
 * @param fp 変換するファイルをよこすのです。
 * @param cset 文字コードを指定したければよこすのです。
 * @param e NULL でなければエラーの情報を書き込むのです。
 * @return 変換したデータを返すのです。
 */
friendsFile *friendsFileForFILE(FILE *fp, const friendsCodeSet *cset,
                                friendsError *e);

#endif
