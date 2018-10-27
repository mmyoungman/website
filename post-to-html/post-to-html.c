#include "lib-mmy.h"

char *helloworld(char *name) {
    char *result = str_copy("Hello ");
    str_concat(result, name);
    str_concat(result, "!");
    return result;
}

void print_bytes(char* str) {
    printf("%s\n\n", str);
}

typedef struct Post {
    char title[128];
    char author[128];
    int dateDay;
    int dateMonth;
    int dateYear;
    char **tags;
    char **categories;
    char series[128];
    int seriesPart;
    char *blurb;
    char *body;
} Post;

Post createPost(char* str) {
    Post post;
    return post;
}

char *convert_body(char* str) {
    Post post = createPost(str);
    return str;
}
