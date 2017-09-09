#ifndef FRIENDS_PATHNAME_H
#define FRIENDS_PATHNAME_H

#include "friends_defs.h"

size_t friendsDirname(friendsChar **output,
                      const friendsChar *input,
                      friendsError *err);

size_t friendsBasename(friendsChar **output,
                       const friendsChar *input,
                       friendsError *err);


#endif /* FRIENDS_PATHNAME_H */
