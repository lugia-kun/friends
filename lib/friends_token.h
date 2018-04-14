/**
 * @file friends_token.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_TOKEN_H
#define FRIENDS_TOKEN_H

#include "friends_defs.h"

friendsToken *friendsNewTokenList(friendsError *e);

friendsToken *friendsTokenNext(friendsToken *t);

friendsToken *friendsTokenPrev(friendsToken *t);

void friendsDeleteToken(friendsToken *t);

void friendsDeleteTokenAll(friendsToken *t);


friendsToken *
friendsTokenAppend(friendsToken *list, int token_type,
                   const friendsChar *start, const friendsChar *end,
                   long line, long column, friendsError *e);

int friendsTokenType(const friendsToken *data);

void friendsSetTokenType(friendsToken *d, int type);

friendsChar *friendsTokenText(const friendsToken *data);

long friendsTokenLinePosition(const friendsToken *data);

long friendsTokenColumnPosition(const friendsToken *data);

void friendsTokenMarkDelete(friendsToken *data);

friendsBool friendsTokenMarkDeleted(const friendsToken *data);

#endif
