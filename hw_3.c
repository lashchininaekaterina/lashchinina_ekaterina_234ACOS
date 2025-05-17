#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s regex text replacement\n", argv[0]);
        return 1;
    }

    regex_t re;
    if (regcomp(&re, argv[1], REG_EXTENDED) != 0) {
        printf("Regex error!\n");
        return 1;
    }

    char *text = argv[2];
    char *rep = argv[3];
    int text_len = strlen(text);
    int rep_len = strlen(rep);
    
    char *new_text = malloc(text_len + rep_len * 2);
    int new_len = 0;
    int pos = 0;
    regmatch_t match;

    while (1) {
        if (regexec(&re, text + pos, 1, &match, 0) != 0) break;

        int start = pos + match.rm_so;
        int end = pos + match.rm_eo;
        int before = start - pos;

        if (new_len + before + rep_len > text_len + rep_len * 2) {
            new_text = realloc(new_text, new_len + before + rep_len + 100);
        }

        if (before > 0) {
            memcpy(new_text + new_len, text + pos, before);
            new_len += before;
        }

        memcpy(new_text + new_len, rep, rep_len);
        new_len += rep_len;

        pos = end;

        if (match.rm_so == match.rm_eo) {
            if (pos >= text_len) break;
            pos++;
        }
    }

    int remaining = text_len - pos;
    if (new_len + remaining > text_len + rep_len * 2) {
        new_text = realloc(new_text, new_len + remaining + 1);
    }

    memcpy(new_text + new_len, text + pos, remaining);
    new_len += remaining;
    new_text[new_len] = '\0';

    printf("%s\n", new_text);

    free(new_text);
    regfree(&re);
    return 0;
}