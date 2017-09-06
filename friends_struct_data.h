/**
 * @file friends_data.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_STRUCT_DATA_H
#define FRIENDS_STRUCT_DATA_H

#include "friends_defs.h"

struct friendsDataListT
{
  friendsData     *data;   /*!< データ */
  friendsDataList *next;   /*!< 次のデータ */
  friendsDataList *prev;   /*!< 前のデータ */
  friendsDataList *begin;  /*!< 親へのポインタ */
  size_t size;             /*!< リストの要素数（最初のデータのみ使用） */
};

struct friendsDataSetT
{
  friendsDataSetNode **table;  /*!< テーブル */
};

struct friendsDataSetNodeT
{
  friendsDataList *list;    /*!< リスト */
  friendsDataSet *root;     /*!< ルートノード */
  friendsDataSetNode *next; /*!< 次のノード */
};

struct friendsDataT
{
  friendsPark *park;                  /*!< 所属するパーク */
  friendsType  type;                  /*!< データの種類 */
  friendsHash  hash;                  /*!< ハッシュ値（連想配列型で使用） */
  void   *data;                       /*!< データへのポインタ */
  friendsPointerDeleter *deleter;     /*!< データの削除関数 */
  friendsChar *txt;                   /*!< データの文字列情報 */
  friendsPointerDeleter *txt_deleter; /*!< テキストデータの削除関数 */
  friendsDataCompareFunc *comp_func;  /*!< データの比較関数 */
};

struct friendsAtomDataT {
  friendsAtomType type;          /*!< アトムの型 */
  union friendsAtomInternalT {
    int n;                       /*!< 数値アトムの値 */
    friendsChar *t;              /*!< 文字列アトムの値 */
  } data;                        /*!< アトムのデータ */
};

struct friendsVariableDataT {
  friendsChar *text;         /*!< 変数名 */
  friendsBool tail;
};

struct friendsPropositionDataT
{
  friendsDataList *conditions; /*!< 前提条件 */
  friendsDataList *arguments;  /*!< パラメータのリスト */
  friendsChar *verb;           /*!< 述語 */
  friendsPropositionMode mode; /*!< モード */
};

struct friendsMatchDataT
{
  friendsBool t;
};

struct friendsMemoryT
{
  void *p;
  friendsPointerDeleter *del;
};

struct friendsParkT
{
  friendsDataSet  *friends;    /*!< 命題のリスト（述語ごとに区分け） */
  friendsDataSet  *atoms;
  friendsDataList *alloc_table;
  friendsParser *parser;
  friendsBool deleting;
};

struct friendsLineColumnT {
  long line;
  long column;
};

struct friendsTokenDataT
{
  friendsChar *token;
  int token_type;
  friendsLineColumn lc;
};

struct friendsParserT
{
  friendsPark *park;
  void *parserData;
  friendsDataList *buffer_list; /*!< 文字列のリスト */
  friendsDataList *tokens;
  friendsDataList *listcur;
  friendsDataList *parsed_data;
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
  enum {
    friendsLexerNORMAL, friendsLexerPAREN, friendsLexerDBRACKET,
    friendsLexerUNDERSCORE,
  } lexer_state;
  int last_type;
  friendsError err;
};

#endif /* FRIENDS_STRUCT_DATA_H */
