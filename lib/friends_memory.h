
#ifndef FRIENDS_MEMORY_H
#define FRIENDS_MEMORY_H

#include "friends_defs.h"

/**
 * @brief 新しいメモリ管理領域を作るのです。
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 新しく作ったメモリ管理領域を返すのです。
 */
friendsMemory *friendsNewMemory(friendsError *e);

/**
 * @brief メモリ管理領域を削除するのです。
 * @param pool 削除するメモリ管理領域をよこすのです。
 *
 * 登録されているすべてのメモリ領域を解放するのです。
 */
void friendsDeleteMemory(friendsMemory *pool);

/**
 * @brief メモリ管理領域にポインタを登録するのです。
 * @param pool 登録先のメモリ管理領域をよこすのです。
 * @param ptr 登録するポインタをよこすのです。
 * @param ptrdel 登録したポインタを削除するときに使う関数をよこすのです。
 * @param e NULL でなければ、そこにエラーの情報を書き込むのです。
 * @return 成功した場合は ptr を、失敗した場合は NULL を返すのです。
 */
void *
friendsMemoryAddPointer(friendsMemory *pool, void *ptr,
                        friendsPointerDeleter *ptrdel, friendsError *e);

/**
 * @brief 指定したポインタが管理領域に登録されているかを調べるのです。
 * @param pool 調べるメモリ管理領域をよこすのです。
 * @param ptr 調べるポインタをよこすのです。
 * @return 登録されていれば friendsTrue を、そうでなければ
 *         friendsFalse を返すのです。
 */
friendsBool friendsMemoryHasPointer(friendsMemory *pool, void *ptr);

/**
 * @brief 指定したポインタを管理領域の登録から解除するのです。
 * @param pool メモリ管理領域をよこすのです。
 * @param ptr 登録を解除するポインタをよこすのです。
 * @return ptr か、登録されていない場合は NULL を返すのです。
 */
void *friendsMemoryUnregister(friendsMemory *pool, void *ptr);

/**
 * @brief 管理領域に登録されている指定したポインタを解放するのです。
 * @param pool メモリ管理領域をよこすのです。
 * @param ptr 解放するポインタをよこすのです。
 * @return 解放した場合は NULL を、登録されていない場合は ptr を返すのです。
 */
void *friendsMemoryDeletePointer(friendsMemory *pool, void *ptr);

#endif
