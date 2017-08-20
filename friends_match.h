/**
 * @file friends_match.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_MATCH_H
#define FRIENDS_MATCH_H

#include "friends_defs.h"

friendsMatchData *friendsGetMatch(friendsData *data);

void friendsSetMatch(friendsData *data, friendsMatchData *m);

friendsBool friendsMatch(friendsPark *park,
                         friendsDataList **match_list,
                         const friendsPropositionData *query);

#endif /* FRIENDS_MATCH_H */
