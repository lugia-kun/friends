
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
  friendsChar *t;
  friendsError e;

  friendsPrintCF(stdout, &e, "> ");
  friendsGetLine(&t, stdin, &e);
  friendsPrintCF(stdout, &e, "%ls", t);

  free(t);
  return 0;
}
