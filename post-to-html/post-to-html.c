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

char *str_match(char *ptr, char *match) {
    int matchLen = str_len(match);
    for(int i = 0; i < matchLen; i++) {
        if(ptr[i] != match[i]) { return 0; }
    }
    return ptr + matchLen;
}

char *str_copyCmd(char *str, char **cmd) {
    char *ptr = str;
    while(*ptr != '{') { //|| *ptr != ' ') { //TODO: THIS REVEALS A BUG
        ptr++;
    }
    int len = ptr - str;
    *cmd = xmalloc(sizeof(char) * (len+1));
    char *copyPtr = *cmd;
    while(*str != '{') {
        *copyPtr = *str;
        str++, copyPtr++;
    }
    *copyPtr = '\0';
    return str;
}

char *getStrArg(char *str, char *arg) {
    if(*str != '{') { return -1; }
    str++;
    int len = 0;
    for(int i = 0; i < 128; i++) {
        if(str[i] == '}') { len = i; break; }
        if(str[i] == '\0') { return -1; }
        if(i == 127) { return -1; }
    }
    
    for(int i = 0; i < len; i++) {
        arg[i] = *str;
        str++;
    }
    arg[len] = '\0';
    return str++;
}

char *getIntArg(char *str, int *arg) {
    if(*str != '{') { return -1; }
    str++;
    int len = 0;
    for(int i = 0; i < 6; i++) {
        if(str[i] == '}') { len = i; break; }
        if(str[i] == '\0') { return -1; }
        if(i == 5) { return -1; }
    }
    
    char strArg[6];
    for(int i = 0; i < len; i++) {
        strArg[i] = *str;
        str++;
    }
    strArg[len] = '\0';
    *arg = str_toint(&strArg);
    return ++str;
}

char *getTextToEnd(char *str, char *type, char **text) {
    char *strPtr = str; 
    while(*strPtr != '\0') {
        if(*strPtr == '\\') {
            char *strPtrBefore = strPtr; // Mark spot where text may end
            strPtr++;
            printf("char: %c\n", *strPtr);
            if(*strPtr == '_' || *strPtr == '\\' || *strPtr == '*') { continue; } // For escape underscores
            char *cmd; 
            strPtr = str_copyCmd(strPtr, &cmd);
            printf("cmdHere: %s\n", cmd);
            if(str_equal(cmd, "end")) {
                printf("But do we get here?\n");
                char arg[128];
                strPtr = getStrArg(strPtr, &arg);
                if(str_equal(arg, type)) {
                    *strPtrBefore = '\0';
                    *text = str_copy(str);
                    return strPtr;
                }
            }
            free(cmd);
        }
        strPtr++;
    }
    return -1;
}

char *convertToHtml(char *str) {
    char *result = str_copy("<p>"); 
    char *strPtr = str;
    char *strCur = str;

    while(*strCur != '\0') {
        if(*strCur == '\\') {
            *strCur = '\0'; // Needed to copy text we've just gone over
            strCur++;
            result = str_concat(result, strPtr);
            strPtr = strCur;

            char *cmd;
            if(*strCur == '_' || *strCur == '\\' || *strCur == '*') { continue; } // Escape underscore
            strCur = str_copyCmd(strCur, &cmd);
            if(str_equal(cmd, "href")) {
                char link[1024];
                char linkText[1024];
                strCur = getStrArg(strCur, &link);
                strCur++;
                strCur = getStrArg(strCur, &linkText);
                strCur++;
                printf("link: %s, text: %s\n", link, linkText);
                result = str_concat(result, "<a href=\"");
                result = str_concat(result, link);
                result = str_concat(result, "\">");
                result = str_concat(result, linkText);
                result = str_concat(result, "</a>");

                strPtr = strCur;
            } else if(str_equal(cmd, "section")) {
                char sectionTitle[1024];
                strCur = getStrArg(strCur, &sectionTitle);
                strCur++;
                printf("sectionTitle: %s\n", sectionTitle);
                result = str_concat(result, "<br>\n<h3>");
                result = str_concat(result, sectionTitle);
                result = str_concat(result, "</h3>\n");

                strPtr = strCur;
            }
        } else if(*strCur == '\n') {
            *strCur = '\0'; // Needed to copy text we've just gone over
            strCur++;
            result = str_concat(result, strPtr);
            strPtr = strCur;

            if(*strCur == '\n') { // Two newlines in a row
                result = str_concat(result, "</p>\n<p>");

                strPtr = strCur;
            }
        }
        strCur++;
    }
    result = str_concat(result, strPtr);
    return result;
}

typedef struct Post {
    char title[128];
    char author[128];
    int dateYear;
    int dateMonth;
    int dateDay;
    char **tags;
    char **categories;
    char series[128];
    int seriesPart;
    char *blurb;
    char *body;
} Post;

Post createPost(char* str) {
    Post post;
    while(*str != '\0') {
        if(*str == '\\') {
            str++;
            char *cmd;
            str = str_copyCmd(str, &cmd);
            printf("cmd: %s\n", cmd);
            if(str_equal(cmd, "title")) {
                str = getStrArg(str, &post.title);
                printf("post.title: %s\n", post.title);
                free(cmd);
            } else if(str_equal(cmd, "author")) {
                str = getStrArg(str, &post.author);
                printf("post.author: %s\n", post.author);
                free(cmd);
            } else if(str_equal(cmd, "date")) {
                str = getIntArg(str, &post.dateYear);
                printf("post.dateYear: %d\n", post.dateYear);
                str = getIntArg(str, &post.dateMonth);
                printf("post.dateMonth: %d\n", post.dateMonth);
                str = getIntArg(str, &post.dateDay);
                printf("post.dateDay: %d\n", post.dateDay);
            } else if(str_equal(cmd, "begin")) {
                char beginType[128];
                str = getStrArg(str, &beginType);
                printf("beginType: %s\n", beginType);
                str++;
                if(str_equal(beginType, "blurb")) {
                    str = getTextToEnd(str, "blurb", &post.blurb);
                    printf("post.blurb: %s\n", post.blurb);
                } else if(str_equal(beginType, "document")) {
                    str = getTextToEnd(str, "document", &post.body);
                    printf("post.body: %s\n", post.body);
                }
            }
        }
        str++;
    }
    return post;
}

char *get_title(char *str) {
    char title[256];
    while(*str != '\0') {
        if(*str == '\\') {
            str++;
            char *cmd;
            str = str_copyCmd(str, &cmd);
            printf("cmd: %s\n", cmd);
            if(str_equal(cmd, "title")) {
                str = getStrArg(str, &title);
                printf("title: %s\n", title);
                free(cmd);
                return str_copy(title);
            }
        }
        //TODO: If \series, add it?
    }
    return -1;
}

char *convert_body(char* str) {
    Post post = createPost(str);

    post.body = convertToHtml(post.body);

    char *html = str_copy("<h1>");
    html = str_concat(html, post.title);
    html = str_concat(html, "</h1>\n");

    if(!str_equal(post.title, "About")) {
        html = str_concat(html, "<h2>");
        html = str_concat(html, str_inttostr(post.dateYear));
        html = str_concat(html, "-");
        html = str_concat(html, str_inttostr(post.dateMonth));
        html = str_concat(html, "-");
        html = str_concat(html, str_inttostr(post.dateDay));
        html = str_concat(html, "</h2>\n");
    }
    html = str_concat(html, post.body);
    return html;
}
