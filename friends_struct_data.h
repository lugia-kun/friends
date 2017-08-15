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
  friendsDataList *begin;  /*!< 最初のデータ */
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
  const friendsChar *txt;             /*!< データの文字列情報 */
  friendsPointerDeleter *txt_deleter; /*!< テキストデータの削除関数 */
};

struct friendsConditionT {
  friendsDataList *propositions; /*!< 命題のリスト */
};

struct friendsAtomDataT {
  friendsAtomType type;          /*!< アトムの型 */
  union friendsAtomInternalT {
    int n;                       /*!< 数値アトムの値 */
    const friendsChar *t;        /*!< 文字列アトムの値 */
  } data;                        /*!< アトムのデータ */
};

struct friendsVariableDataT {
  const friendsChar *text;   /*!< 変数名 */
  int offset;                /*!< 差異（「〜の次」を使用した回数） */
};

struct friendsArgumentT {
  friendsData *variable;       /*!< 変数またはアトム、またはそのリスト */
  const friendsChar *particle; /*!< 助詞 */
};

struct friendsPropositionDataT
{
  friendsData *conditions;     /*!< 前提条件 */
  friendsData *arguments;      /*!< パラメータのリスト */
  const friendsChar *verb;     /*!< 述語 */
  friendsBool  stop;           /*!< たーのしー！ */
};

struct friendsVariableMatchT
{
  friendsPark *park;           /*!< 所属するパーク */
  friendsData *friend;
  friendsData *variable1;      /*!< 問い合わせ文での変数 */
  friendsData *variable2;
  friendsData *atom;           /*!< アトム */
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
  friendsBool deleting;
};

struct friendsLineColumnT {
  long line;
  long column;
};

struct friendsParserT
{
  void *parserData;
  const friendsChar *cur;
  const friendsChar *mark;
  const friendsChar *cmark;
  const friendsChar *limit;
  const friendsChar *token;
  friendsLineColumn curpos;
  friendsLineColumn tokpos;
  friendsFillFunc *fill;
};

#endif /* FRIENDS_STRUCT_DATA_H */
