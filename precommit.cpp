#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#if defined(_WIN32) || defined(_WIN64)
#define popen _popen
#define pclose _pclose
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

FILE *get_temporary_file() {
    FILE *fp = nullptr;
    
#if defined(_WIN32) || defined(_WIN64)
    tmpfile_s(&fp);
#else
    fp = tmpfile();
#endif
    
    return fp;
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
                break;
            }
            t->current += 1;
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

void string_print(String a) {
    char *p = a.data;
	for (uint i = 0; i < a.count; ++i) {
        putchar(*p);
        p += 1;
	}
}

int main() {
    FILE* tmpf = get_temporary_file();
    
    FILE *fp;
    if ((fp = popen(CMD, "rt")) == NULL) {
        printf("Error: git could not be opened\n");
        return 0;
    }
    
    char buffer[128];
    
    while(fgets(buffer, 128, fp))
    {
        fputs(buffer, tmpf);
    }
    
    pclose(fp);
    
    String string = read_entire_file(tmpf);
    
    Trav t;
    t.string = string;
    t.current = 0;
    
    printf("== CONTENT ==\n");
    
    while (trav_continue(&t)) {
        String line = trav_get_line(&t);
        if (string_starts_with(line, string_lit("+++"))) {
            string_print(line);
        }
    }
    
    printf("== CONTENT ==\n");
    
    
    return 0;
}
