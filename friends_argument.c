
#include <stdlib.h>

#include "friends_defs.h"
#include "friends_struct_data.h"
#include "friends_data.h"
#include "friends_list.h"
#include "friends_error.h"
#include "friends_io.h"

static void friendsArgumentDeleter(void *a)
{
  friendsArgumentData *d;
  d = (friendsArgumentData *)a;

  /* friendsDeleteData(d->variable); */
  free(d->particle);
  free(d);
}

const friendsArgumentData *friendsGetArgument(friendsData *d)
{
  if (!d) return NULL;
  if (friendsGetType(d) != friendsArgument) return NULL;

  return (friendsArgumentData *)d->data;
}

friendsData *friendsSetArgument(friendsData *dest, friendsData *d,
                                friendsChar *particle, friendsError *err)
{
  friendsType t;
  friendsArgumentData *a;
  friendsChar *ch;
  int x;

  friendsAssert(dest);
  friendsAssert(d);
  friendsAssert(particle);
  friendsAssert(dest != d);

  t = friendsGetType(d);
  switch (t) {
  case friendsAtom:
  case friendsVariable:
  case friendsEtcetra:
    break;
  default:
    friendsSetError(err, InvalidType);
    return NULL;
  }

  a = (friendsArgumentData *)calloc(sizeof(friendsArgumentData *), 1);
  if (!a) {
    friendsSetError(err, NOMEM);
    return NULL;
  }

  if (t != friendsEtcetra) {
    x = friendsAsprintCF(&ch, err, "%ls %ls",
                         friendsDataToText(d), particle);
  } else {
    x = friendsAsprintCF(&ch, err, "\\u3068\\u304b %ls" /* とか */,
                         particle);
  }
  if (x < 0) {
    free(a);
    return NULL;
  }

  a->variable = d;
  a->particle = particle;

  dest->type = friendsArgument;
  dest->data = a;
  dest->deleter = friendsArgumentDeleter;
  dest->txt = ch;
  dest->txt_deleter = free;

  return dest;
}

const friendsChar *friendsArgumentParticle(const friendsArgumentData *d)
{
  friendsAssert(d);

  return d->particle;
}

friendsData *friendsArgumentVariable(const friendsArgumentData *d)
{
  friendsAssert(d);

  return d->variable;
}
