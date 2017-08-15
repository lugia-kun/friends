
#include "friends_defs.h"
#include "friends_global.h"
#include "friends_error.h"
#include "enc/utf8.h"

struct friendsGlobalT {
  friendsCodeSet terminal_encoding;
};

static struct friendsGlobalT friendsGlobalData = {
  {friendsUtf8ToFChar, friendsFCharToUtf8, "UTF-8"},
};

const friendsCodeSet *friendsGetTerminalEncoding(void)
{
  return &friendsGlobalData.terminal_encoding;
}

void friendsSetTerminalEncoding(const friendsCodeSet *set)
{
  friendsAssert(set);
  friendsAssert(set->enc);
  friendsAssert(set->dec);
  friendsAssert(set->name);

  friendsGlobalData.terminal_encoding = *set;
}
