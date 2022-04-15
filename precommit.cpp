#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define Assert assert

#if defined(_WIN32) || defined(_WIN64)
#define popen _popen
#define pclose _pclose
#endif

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define Minimum(a, b) (((a) < (b)) ? (a) : (b))
#define Maximum(a, b) (((a) > (b)) ? (a) : (b))
#define Clamp(a, b, v) Minimum(b, Maximum(a, v))

struct String {
	ptrdiff_t length;
	uint8_t * data;

	String() : data(0), length(0) {}
	template <ptrdiff_t _Length>
	constexpr String(const char(&a)[_Length]) : data((uint8_t *)a), length(_Length - 1) {}
	String(const uint8_t *_Data, ptrdiff_t _Length) : data((uint8_t *)_Data), length(_Length) {}
	String(const char *_Data, ptrdiff_t _Length) : data((uint8_t *)_Data), length(_Length) {}
	const uint8_t &operator[](const ptrdiff_t index) const { Assert(index < length); return data[index]; }
	uint8_t &operator[](const ptrdiff_t index) { Assert(index < length); return data[index]; }
	inline uint8_t *begin() { return data; }
	inline uint8_t *end() { return data + length; }
	inline const uint8_t *begin() const { return data; }
	inline const uint8_t *end() const { return data + length; }
};

static String StrRemovePrefix(String str, ptrdiff_t count) {
	Assert(str.length >= count);
	str.data += count;
	str.length -= count;
	return str;
}

static String SubStr(String str, ptrdiff_t index, ptrdiff_t count) {
	Assert(index < str.length);
	count = (ptrdiff_t)Minimum(str.length - index, count);
	return String(str.data + index, count);
}

static int StrCompare(String a, String b) {
	ptrdiff_t count = (ptrdiff_t)Minimum(a.length, b.length);
	return memcmp(a.data, b.data, count);
}

static int StrCompareCaseInsensitive(String a, String b) {
	ptrdiff_t count = (ptrdiff_t)Minimum(a.length, b.length);
	for (ptrdiff_t index = 0; index < count; ++index) {
		if (a.data[index] != b.data[index] && a.data[index] + 32 != b.data[index] && a.data[index] != b.data[index] + 32) {
			return a.data[index] - b.data[index];
		}
	}
	return 0;
}

static bool StrStartsWith(String str, String sub) {
	if (str.length < sub.length)
		return false;
	return StrCompare(String(str.data, sub.length), sub) == 0;
}

static ptrdiff_t StrFindCaseInsensitive(String str, String key, ptrdiff_t pos) {
	ptrdiff_t index = Clamp(0, str.length - 1, pos);
	while (str.length >= key.length) {
		if (StrCompareCaseInsensitive(String(str.data, key.length), key) == 0) {
			return index;
		}
		index += 1;
		str = StrRemovePrefix(str, 1);
	}
	return -1;
}

static String ReadEntireFile(FILE *fp) {
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *ptr = (char *)malloc(fsize + 1);
	fread(ptr, 1, fsize, fp);
	ptr[fsize] = 0;

	String string;
	string.length = (ptrdiff_t)fsize;
	string.data   = (uint8_t *)ptr;

	return string;
}

struct Reader {
	String    content;
	ptrdiff_t position;
	String    line;
};

static Reader ReaderInit(String content) {
	Reader reader;
	reader.content  = content;
	reader.position = 0;
	reader.line     = "";
	return reader;
}

static bool NextLine(Reader *reader) {
	if (reader->position < reader->content.length) {
		ptrdiff_t position = reader->position;
		for (; position < reader->content.length; ++position) {
			if (reader->content[position] == '\n' || reader->content[position] == '\r') {
				// Since content is null terminated, we can access position + 1
				if (reader->content[position] == '\r' && reader->content.data[position + 1] == '\n') {
					reader->line     = SubStr(reader->content, reader->position, position - reader->position);
					reader->position = position + 2;
				} else {
					reader->line     = SubStr(reader->content, reader->position, position - reader->position);
					reader->position = position + 1;
				}
				reader->line.data[reader->line.length] = 0;
				return true;
			}
		}
		reader->line     = SubStr(reader->content, reader->position, position - reader->position);
		reader->position = position;
		return true;
	}
	return false;
}

static void ReportError(String filepath, int line_number, String line, String keyword) {
	fprintf(stderr, "%.*s(%d): error : Keyword \"%.*s\" is present in the commit, which is not allowed.\n%.*s\n", 
		(int)filepath.length, filepath.data, line_number, (int)keyword.length, keyword.data, (int)line.length, line.data);
}

int main() {
	const String Keyword = "nocheckin";
	const int MaxKeyword = 1024;

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

	String string = ReadEntireFile(tmpf);

	Reader reader = ReaderInit(string);

	String filepath = "";
	int line_number = 0;
	int keywords = 0;

	while (NextLine(&reader)) {
		String line = reader.line;

		if (StrStartsWith(line, "+++")) {
			filepath = StrRemovePrefix(line, 6);
			line_number = 0;
		} else if (StrStartsWith(line, "@@")) {
			int dummy = 0;
			sscanf((char *)line.data, "@@ %d,%d %d,%d @@", &dummy, &dummy, &line_number, &dummy);
		} else if (StrStartsWith(line, "+")) {
			line = StrRemovePrefix(line, 1);

			ptrdiff_t index = StrFindCaseInsensitive(line, Keyword, 0);
			if (index >= 0) {
				ReportError(filepath, line_number, line, Keyword);
				keywords += 1;
				if (keywords > MaxKeyword) {
					break;
				}
			}
			line_number += 1;
		} else if (!StrStartsWith(line, "-")) {
			line_number += 1;
		}
	}

	return keywords;
}
