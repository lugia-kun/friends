
#include "friends_defs.h"
#include "friends_global.h"
#include "friends_error.h"
#include "enc/utf8.h"

struct friendsGlobalT {
  const friendsCodeSet *terminal_encoding;
};

static struct friendsGlobalT friendsGlobalData = {
  NULL,
};

const friendsCodeSet *friendsGetTerminalEncoding(void)
{
  if (!friendsGlobalData.terminal_encoding) {
    return friendsUtf8Set();
  } else {
    return friendsGlobalData.terminal_encoding;
  }
}

void friendsSetTerminalEncoding(const friendsCodeSet *set)
{
  friendsAssert(set);
  friendsAssert(set->enc);
  friendsAssert(set->dec);
  friendsAssert(set->name);

  friendsGlobalData.terminal_encoding = set;
}
