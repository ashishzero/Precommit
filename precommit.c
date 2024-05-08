#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

#define Assert assert

#if defined(_WIN32) || defined(_WIN64)
#define popen _popen
#define pclose _pclose
#endif

#if __has_include("keyword.h")
#include "keyword.h"
#endif

#ifndef NocheckinKeyword 
#define NocheckinKeyword "nocheckin"
#endif

#define MaxErrorOutput 16

static char *read_entire_file(FILE *fp, size_t *count) {
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *ptr = (char *)malloc(fsize + 1);
	fread(ptr, 1, fsize, fp);
	ptr[fsize] = 0;

	*count = (size_t)fsize;

	return ptr;
}

static char *read_line(char **ptr) {
	char *line = *ptr;

	char *trav = line;

	for (;*trav;++trav) {
		char prev = trav[0];
		if (prev == '\n' || prev == '\r') {
			*trav++ = 0;
			if (prev == '\r' && *trav == '\n') {
				trav++;
			}
			break;
		}
	}

	*ptr = trav;
	return line;
}

static int starts_with(const char *src, const char *sub) {
	for (; *sub; ++sub, ++src) {
		if (tolower(*src) != tolower(*sub)) return 0;
	}
	return 1;
}

static int contains_nocheckin_keyword(const char *match) {
	for (int col = 0; match[col]; ++col) {
		if (starts_with(match + col, NocheckinKeyword))
			return col + 1;
	}
	return -1;
}

static void error(const char *filepath, int row, int col, const char *line) {
	fprintf(stderr, "%s(%d, %d): error : Keyword \"%s\" is present in the commit, which is not allowed.\n%s\n", 
			filepath, row, col, NocheckinKeyword, line);
}

int main() {
	FILE* tmpf = tmpfile();

	FILE *fp = popen("git diff --cached", "r");
	if (!fp) {
		printf("Error: git could not be opened\n");
		return 1;
	}

	char buffer[16 * 1024];
	while (fgets(buffer, sizeof(buffer), fp)) {
		fputs(buffer, tmpf);
	}
	pclose(fp);

	size_t count;
	char *content = read_entire_file(tmpf, &count);

	const char *filepath = "";
	int row = 0;
	int keywords = 0;

	while (*content) {
		char *line = read_line(&content);

		if (starts_with(line, "+++")) {
			filepath = line + 6;
			row = 0;
		} else if (starts_with(line, "@@")) {
			int dummy = 0;
			sscanf(line, "@@ %d,%d %d,%d @@", &dummy, &dummy, &row, &dummy);
		} else if (starts_with(line, "+")) {
			line = line + 1;
			int col = contains_nocheckin_keyword(line);
			if (col >= 0) {
				error(filepath, row, col, line);
				keywords += 1;
				if (keywords > MaxErrorOutput) {
					break;
				}
			}
			row += 1;
		} else if (!starts_with(line, "-")) {
			row += 1;
		}
	}

	return keywords;
}
