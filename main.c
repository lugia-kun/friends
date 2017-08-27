
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "friends_error.h"
#include "friends_io.h"
#include "friends_list.h"
#include "friends_park.h"
#include "friends_data.h"
#include "friends_atom.h"
#include "friends_string.h"

int main(int argc, char **argv)
{
  friendsError e;
  friendsSetErrorV(e, NOMEM);
  return 0;
}
