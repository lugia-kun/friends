/**
 * @file friends_struct_data.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_STRUCT_DATA_H
#define FRIENDS_STRUCT_DATA_H

#include <stdint.h>

#include "friends_defs.h"

struct friendsDataFunctions
{
  friendsObjectDeleter   *deleter;     /*!< データの削除関数 */
  friendsDataCompareFunc *comparator;  /*!< データの比較関数 */
  friendsTextDeleter     *txt_deleter; /*!< 文字列データの削除関数 */
  friendsTextCreator     *txt_creator; /*!< 文字列データの作成関数 */
};

/**
 * @typedef friendsMaxAlignType
 * @brief アライメントの最も大きい型
 *
 * もしコンパイラが C11 が対応していれば `max_align_t` を使うのです。C11
 * に対応していない場合は、基本的な型を `union` で固めた型を使っているの
 * です (ただし、不十分かもしれません)。
 */
#if defined(HAVE_MAX_ALIGN_T)
typedef max_align_t friendsMaxAlignType;
#else
typedef union {
  short s; int i; long int l; long long int ll; intmax_t im;
  float f; double d; long double ld;
  void *p; void (*fp)(void);
} friendsMaxAlignType;
#endif

struct friendsDataObject
{
  friendsType type;    /*!< データの種類 */
  const friendsDataFunctions *funcs; /*!< データの処理関数 */
  friendsChar *txt;    /*!< 文字列データ */
  friendsSize refc;    /*!< 参照個数 */
  friendsSize size;    /*!< データのサイズ */
  friendsHash hash;    /*!< ハッシュ値 */
  friendsMaxAlignType data[];  /*!< データ */
};
typedef struct friendsDataObject friendsDataObject;

struct friendsData
{
  friendsPark *park;   /*!< 所属するパーク */
  friendsDataObject *object;
};

union friendsAtomInternal
{
  int n;            /*!< 数値アトムの値 */
  friendsChar *t;   /*!< 文字列アトムの値 */
};

struct friendsAtomData
{
  friendsAtomType type;           /*!< アトムの型 */
  union friendsAtomInternal data; /*!< アトムのデータ */
};

struct friendsVariableData
{
  friendsChar *text;         /*!< 変数名 */
  friendsBool tail;
};

struct friendsPropositionData
{
  friendsDataList *conditions; /*!< 前提条件 */
  friendsDataList *arguments;  /*!< パラメータのリスト */
  friendsChar *verb;           /*!< 述語 */
  friendsPropositionMode mode; /*!< モード */
};

struct friendsMatchData
{
  friendsBool t;
};

struct friendsMemory
{
  void *p;
  friendsBool delete_later;
  friendsPointerDeleter *del;
};

struct friendsPark
{
  friendsDataSet  *friends;    /*!< 命題のリスト（述語ごとに区分け） */
  friendsDataSet  *atoms;
  friendsDataList *alloc_table;
  friendsParser *parser;
  friendsBool deleting;
};

struct friendsLineColumn
{
  long line;
  long column;
};

struct friendsParser
{
  friendsPark *park;
  void *parserData;
  friendsToken *tokens;
  friendsToken *tokencur;
  friendsDataList *parsed_data;
  friendsStringList *buffer_list;
  friendsStringList *listcur;
  size_t bufp_size;
  friendsChar *bufp;
  const friendsChar *cur;
  const friendsChar *mark;
  const friendsChar *cmark;
  const friendsChar *limit;
  const friendsChar *token;
  const friendsChar *stoken;
  friendsLineColumn curpos;
  friendsLineColumn tokpos;
  friendsLineColumn errpos;
  friendsLexerState lexer_state;
  int last_type;
  const char* filename;
  friendsError err;
};

#endif /* FRIENDS_STRUCT_DATA_H */
