/**
 * @file friends_match.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_MATCH_H
#define FRIENDS_MATCH_H

#include "friends_defs.h"

friendsMatchData *friendsGetMatch(friendsData *data);

friendsData *friendsSetMatch(friendsData *dest, friendsMatchData *m,
                             friendsError *err);


#endif /* FRIENDS_MATCH_H */
