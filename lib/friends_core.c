#include "friends_config.h"

#include <stdlib.h>

#include "friends_error.h"
#include "friends_defs.h"
#include "friends_core.h"

void *friendsMalloc(size_t size, friendsError *e)
{
  void *p;
  p = calloc(size, 1);
  if (!p) {
    friendsSetError(e, NOMEM);
    return NULL;
  }
  return p;
}

void friendsFree(void *p)
{
  free(p);
}
