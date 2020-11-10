#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define popen _popen
#define pclose _pclose

static WORD global_win32_old_color_attrs;

void set_red_color() {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(h, &info);
	global_win32_old_color_attrs = info.wAttributes;
	SetConsoleTextAttribute(h, FOREGROUND_RED);
}

void set_yellow_color() {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(h, &info);
	global_win32_old_color_attrs = info.wAttributes;
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN);
}

void reset_color() {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, global_win32_old_color_attrs);
}

#else

void set_red_color() {
	printf("\033[0;31m");
}

void set_yellow_color() {
	printf("\033[01;33m");
}

void reset_color() {
	printf("\033[0m");
}

#endif

const char KEY[] = "nocheckin";
const char *CMD = "git diff --cached";

typedef unsigned int uint;

#define static_count(a) (sizeof(a) / sizeof((a)[0]))
#define string_lit(s) String { (uint)(sizeof(s) - 1), s }

struct String {
    uint count;
    char *data;
};

String read_entire_file(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *ptr = (char *)malloc(fsize + 1);
    fread(ptr, 1, fsize, fp);
    ptr[fsize] = 0;
    
    String string;
    string.count = fsize;
    string.data = ptr;
    
    return string;
}

struct Trav {
    String string;
    uint current;
};

bool trav_continue(Trav *t) {
    return t->current < t->string.count;
}

String trav_get_line(Trav *t) {
    String line;
    line.data = t->string.data + t->current;
    line.count = 0;
    
    char *p = t->string.data + t->current;
    
    while (*p) {
        if (*p == '\n' || *p == '\r') {
            if (*p == '\r' && *(p + 1) == '\n') {
                t->current += 1;
            }
            t->current += 1;
			break;
        }
        
        line.count += 1;
        p += 1;
        t->current += 1;
    }
    
    return line;
}

bool string_starts_with(String a, String b) {
    if (a.count < b.count) return false;
    
	for (uint i = 0; i < b.count; ++i) {
		if (a.data[i] != b.data[i]) return false;
	}
    
	return true;
}

bool string_match(String a, String b) {
	if (a.count != b.count) return false;
	
	for (uint i = 0; i < a.count; ++i) {
		if (a.data[i] != b.data[i]) return false;
	}
	
	return true;
}

struct String_Contains {
	bool found;
	uint index;
};

String_Contains string_contains(String string, String item, uint index) {
	String_Contains result;
	result.found = false;
	result.index = 0;
	
	if (string.count - index >= item.count) {
		uint end = string.count - index - item.count + 1;
		for (uint i = 0; i < end; ++i) {
			String matcher;
			matcher.data  = string.data + index + i;
			matcher.count = item.count;
			if (string_match(matcher, item)) {
				result.found = true;
				result.index = index + i;
				return result;
			}
		}
	}
	
	return result;
}

void string_print(String a) {
    char *p = a.data;
	for (uint i = 0; i < a.count; ++i) {
        putchar(*p);
        p += 1;
	}
}

static const uint BUFF_SIZE = 1024 * 1024;
static const uint MAX_INVALID_COUNT = 1024;
static uint invalids[MAX_INVALID_COUNT];

int main() {
    FILE* tmpf = tmpfile();
    
    FILE *fp;
    if ((fp = popen(CMD, "r")) == NULL) {
        printf("Error: git could not be opened\n");
        return 0;
    }
	
	char *buffer = (char *)malloc(BUFF_SIZE);
    
    while(fgets(buffer, BUFF_SIZE, fp))
    {
        fputs(buffer, tmpf);
    }
    
    pclose(fp);
    
    String string = read_entire_file(tmpf);
    
	int status = 0;
	
    Trav t;
    t.string = string;
    t.current = 0;
	
	String key;
	key.count = static_count(KEY) - 1;
	key.data = (char *)KEY;
    
	String line;
    while (trav_continue(&t)) {
        line = trav_get_line(&t);
        if (string_starts_with(line, string_lit("+++"))) {
			String file_name = line;
			file_name.data += 6;
			file_name.count -= 6;
			
			while (trav_continue(&t)) {
				line = trav_get_line(&t);
				
				if (!string_starts_with(line, string_lit("@@"))) break;
				
				int line_number = 0, line_count = 0, dummy = 0;
				sscanf(line.data, "@@ %d,%d %d,%d @@", &dummy, &dummy, &line_number, &line_count);
				
				String_Contains contain;
				contain.index = 0;
				
				for (int line_index = 0; line_index < line_count; ++line_index) {
					line = trav_get_line(&t);
					
					int invalid_count = 0;
					if (string_starts_with(line, string_lit("+"))) {
						line.data += 1;
						line.count -= 1;
						
						while (true) {
							contain = string_contains(line, key, contain.index);
							if (!contain.found || invalid_count == MAX_INVALID_COUNT) break;
							
							if (contain.index > 0 && contain.index + key.count < line.count) {
								if (line.data[contain.index - 1] == '"' && line.data[contain.index + key.count] == '"') {
									contain.index += key.count;
									continue;
								}
							}
							
							invalids[invalid_count] = contain.index;
							invalid_count += 1;
							contain.index += key.count;
						}
					}
					
					if (invalid_count) {
						status = 1;
						
						set_red_color();
						printf("Error: Invalid content. File: ");
						string_print(file_name);
						printf("\n");
						reset_color();
						
						printf("\t Line %d: ", line_count + line_index);
						
						String part;
						uint start = 0;
						for (int index = 0; index < invalid_count; ++index) {
							part.data = line.data + start;
							part.count = invalids[index] - start;
							
							string_print(part);
							
							start = invalids[index];
							part.data = line.data + start;
							part.count = key.count;
							
							set_yellow_color();
							string_print(part);
							reset_color();
							
							start += key.count;
						}
						
						part.data = line.data + start;
						part.count = line.count - start;
						
						string_print(part);
						printf("\n");
					}
				}
			}
		}
	}
	
	return status;
}
