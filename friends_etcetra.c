
#include "friends_etcetra.h"
#include "friends_struct_data.h"
#include "friends_defs.h"
#include "friends_data.h"
#include "friends_error.h"

void friendsSetEtcetra(friendsData *d, friendsError *err)
{
  friendsAssert(d);

  if (friendsGetType(d) != friendsInvalidType) {
    friendsSetError(err, ValidType);
    return;
  }

  d->type = friendsEtcetra;
  d->data = NULL;
  d->deleter = NULL;
  d->hash = 0x1;
  d->txt = NULL;
  d->txt_deleter = NULL;
}
