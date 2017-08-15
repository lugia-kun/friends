/**
 * @file friends_list.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_SET_H
#define FRIENDS_SET_H

#include "friends_defs.h"

friendsDataSet *friendsNewSet(friendsError *e);
void friendsDeleteSet(friendsDataSet *set);

friendsDataList *friendsSetFindText(friendsDataSet *set,
                                    friendsType type, const friendsChar *text);
friendsDataList *friendsSetFindNumericAtom(friendsDataSet *set, int a);

void friendsSetInsert(friendsDataSet *set, friendsData *d, friendsError *e);
void friendsSetRemove(friendsDataSet *set, friendsData *d, friendsError *e);
void friendsSetRemoveAll(friendsDataSet *set, friendsData *d, friendsError *e);

#endif /* FRIENDS_SET_H */
