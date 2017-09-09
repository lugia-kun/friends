/**
 * @file friends_token.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_TOKEN_H
#define FRIENDS_TOKEN_H

#include "friends_defs.h"

friendsData *friendsSetToken(friendsData *data,
                             friendsChar *token_text,
                             int token_type,
                             long line, long column, friendsError *e);

const friendsTokenData *friendsGetToken(friendsData *data);

int friendsTokenType(const friendsTokenData *data);
friendsChar *friendsTokenText(const friendsTokenData *data);
long friendsTokenLinePosition(const friendsTokenData *data);
long friendsTokenColumnPosition(const friendsTokenData *data);

#endif
