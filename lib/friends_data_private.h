/**
 * @file friends_data_private.h
 * @brief Internal private data functions
 */

#ifndef FRIENDS_DATA_PRIVATE_H
#define FRIENDS_DATA_PRIVATE_H

#include "friends_defs.h"
#include "friends_data.h"
#include "friends_struct_data.h"

friendsError
friends_DataSet(friendsData *d, friendsType type, friendsSize data_size,
                const friendsDataFunctions *funcs, friendsHash hash);

void *friends_DataGetTypeData(friendsData *d);

void friends_DataInit(friendsData *d);

#endif
