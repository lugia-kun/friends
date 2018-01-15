
#include <locale.h>

#include "friends_config.h"
#include "friends_defs.h"
#include "friends_global.h"
#include "friends_error.h"
#include "enc/utf8.h"

#if defined(FRIENDS_ENABLE_LIBEDIT)
#include <histedit.h>
#endif

struct friendsGlobal {
  const friendsCodeSet *terminal_encoding;
#if defined(FRIENDS_ENABLE_LIBEDIT)
  EditLine *el;
#endif
};

static struct friendsGlobal friendsGlobalData = {
  NULL,
#if defined(FRIENDS_ENABLE_LIBEDIT)
  NULL, /* el */
#endif
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

void friendsInit(int argc, char **argv)
{
#if defined(FRIENDS_ENABLE_LIBEDIT)
  EditLine *el;
#endif

  friendsUnUsed(argc);
  friendsUnUsed(argv);

  setlocale(LC_CTYPE, "");

#if defined(FRIENDS_ENABLE_LIBEDIT)
  el = el_init("friends", stdin, stdout, stderr);
  if (el) {
    el_set(el, EL_EDITOR, "emacs");
  }
  friendsGlobalData.el = el;
#endif
}

void friendsFinalize(void)
{
#if defined(FRIENDS_ENABLE_LIBEDIT)
  if (friendsGlobalData.el) {
    el_end(friendsGlobalData.el);
  }
#endif
}

#if defined(FRIENDS_ENABLE_LIBEDIT)
EditLine *friendsGetEditLine(void)
{
  return friendsGlobalData.el;
}
#endif
