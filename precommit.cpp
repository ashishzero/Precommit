#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#define popen _popen
#define pclose _pclose
#endif

const char KEY[] = "nocheckin";
const char *CMD = "git diff --cached";    

typedef unsigned int uint;

#define static_count(a) (sizeof(a) / sizeof((a)[0]))

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
    
    printf("== CONTENT ==\n");
    printf("%s\n", string.data);
    printf("== CONTENT ==\n");
    
    return 0;
}
