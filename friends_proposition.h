/**
 * @file friends_proposition.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_PROPOSITION_H
#define FRIENDS_PROPOSITION_H

#include "friends_defs.h"

const friendsPropositionData *friendsGetProposition(friendsData *d);

void friendsSetProposition(friendsData *dest, friendsChar *verb,
                           friendsDataList *arguments,
                           friendsDataList *conditions,
                           friendsBool stop,
                           friendsError *err);

friendsDataList *friendsPropositionArguments(const friendsPropositionData *p);

friendsDataList *friendsPropositionConditions(const friendsPropositionData *p);

friendsBool friendsPropositionIsStop(const friendsPropositionData *p);

const friendsChar *friendsPropositionVerb(const friendsPropositionData *p);

#endif /* FRIENDS_PROPOSITION_H */
