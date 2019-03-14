/*
 * UNG's Not GNU
 * 
 * Copyright (c) 2011, Jakob Kaivo <jakob@kaivo.net>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <utmpx.h>
#include <unistd.h>

const char *write_desc = "write to another user";
const char *write_inv  = "write user_name [terminal]";

int
main(int argc, char **argv)
{
  char *user, *tty;
  char *buf;
  char line[PATH_MAX];
  size_t nread, len = 0;
  struct utmpx *ut;
  FILE *out;

  if (argc < 2 || argc > 3) {
    return 1;
  } else if (argc == 2) {
    user = argv[1];
    tty = NULL;
  } else {
    user = argv[1];
    tty = argv[2];
  }

  setutxent();
  while ((ut = getutxent()) != NULL) {
    if ((ut->ut_type == USER_PROCESS || ut->ut_type == LOGIN_PROCESS) && !strcmp (user, ut->ut_user)) {
      if (tty == NULL)
        break;
      else if (!strcmp (tty, ut->ut_line))
        break;
    }
  }
  endutxent();

  if (ut == NULL)
    return 1;

  strcat (line, "/dev/");
  strcat (line, ut->ut_line);
  out = fopen (line, "w");
  fprintf (out, "Message from %s (%s) [%s]...\n", getlogin(), ttyname(fileno(stdin)), "FIXME");
  while (!feof(stdin)) {
    if ((nread = getline (&buf, &len, stdin)) != -1)
      fwrite (buf, sizeof(char), nread, out);
    free (buf);
    len = 0;
  }
  fprintf (out, "EOT\n");
  fclose (out);

  return 0;
}
