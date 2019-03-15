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

#define _XOPEN_SOURCE 700

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <utmpx.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char *user = NULL;
	char *terminal = NULL;

	setlocale(LC_ALL, "");

	int c;
	while ((c = getopt(argc, argv, "")) != -1) {
		return 1;
	}

	if (argc == optind + 1) {
		user = argv[optind];
	} else if (argc == optind + 2) {
		user = argv[optind];
		terminal = argv[optind + 1];
	} else {
		return 1;
	}

	struct utmpx *ut = NULL;
	setutxent();
	while ((ut = getutxent()) != NULL) {
		if ((ut->ut_type == USER_PROCESS || ut->ut_type == LOGIN_PROCESS) && !strcmp(user, ut->ut_user)) {
			if (terminal == NULL || !strcmp(terminal, ut->ut_line)) {
				break;
			}
		}
	}
	endutxent();

	if (ut == NULL) {
		return 1;
	}

	char line[PATH_MAX];
	sprintf(line, "/dev/%s", ut->ut_line);
	FILE *out = fopen(line, "w");

	char date[PATH_MAX];
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	strftime(date, sizeof(date), "%c", tm);
	terminal = ttyname(STDIN_FILENO);
	if (!strncmp(terminal, "/dev/", 5)) {
		terminal += 5;
	}
	fprintf(out, "Message from %s (%s) [%s]...\n", getlogin(), terminal, date);

	printf("\a\a");
	fflush(stdout);

	ssize_t nread = 0;
	size_t len = 0;
	char *buf = NULL;
	while ((nread = getline(&buf, &len, stdin)) != -1) {
		fwrite(buf, 1, nread, out);
	}

	fprintf(out, "EOT\n");
	fclose(out);

	return 0;
}
