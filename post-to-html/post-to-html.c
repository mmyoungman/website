#include "lib-mmy.h"

char *str_match(char *ptr, char *match) {
    int matchLen = str_len(match);
    for(int i = 0; i < matchLen; i++) {
        if(ptr[i] != match[i]) { return 0; }
    }
    return ptr + matchLen;
}

char *str_copyToPtr(char *start, char *end) {
    char *res = 0;
    assert(start < end);
    while(start != end) {
        arr_push(res, *start);
        start++;
    }
    return res;
}

char **process_cmd(char **str) {
    char **res = 0;
    char *cmd = 0;

    assert(**str == '\\');
    (*str)++;
    char *ptr = *str;
    while(*ptr != '{' && *ptr != ' ' && *ptr != '\0') {
        arr_push(cmd, *ptr);
        ptr++;
    }
    arr_push(cmd, '\0');
    dbg("cmd: %s", cmd);
    arr_push(res, cmd);

    while(*ptr == '{') {
        char *arg = 0;
        ptr++;
        while(*ptr != '}') {
            arr_push(arg, *ptr); 
            ptr++;
        }
        arr_push(arg, '\0');
        assert(*ptr == '}');
        arr_push(res, arg);
        dbg("arg: %s", arg);
        ptr++;
    }
    *str = ptr;
    return res;
}

char *str_copyCmd(char *str, char **cmd) {
    char *ptr = str;
    while(*ptr != '{' && *ptr != ' ') {
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
    if(*str != '{') { log_err("Cmd should be followed by '{'!"); assert(0); }
    str++;
    int len = 0;
    for(int i = 0; i < 128; i++) {
        if(str[i] == '}') { len = i; break; }
        if(str[i] == '\0') { log_err("str shouldn't end before cmd arg ends"); assert(0); }
        if(i == 127) { log_err("arg shouldn't be longer than 128 chars"); assert(0); }
    }
    
    for(int i = 0; i < len; i++) {
        arg[i] = *str;
        str++;
    }
    arg[len] = '\0';
    return str++;
}

char *getIntArg(char *str, int *arg) {
    if(*str != '{') { assert(0); }
    str++;
    int len = 0;
    for(int i = 0; i < 6; i++) {
        if(str[i] == '}') { len = i; break; }
        if(str[i] == '\0') { assert(0); }
        if(i == 5) { assert(0); }
    }
    
    char strArg[6];
    for(int i = 0; i < len; i++) {
        strArg[i] = *str;
        str++;
    }
    strArg[len] = '\0';
    *arg = str_toint(strArg);
    return ++str;
}

char *getTextToEnd(char *str, char *type, char **text) {
    char *strPtr = str; 
    while(*strPtr != '\0') {
        if(*strPtr == '\\') {
            char *strPtrBefore = strPtr; // Mark spot where text may end
            strPtr++;
            if(*strPtr == '_' || *strPtr == '\\') { continue; } // For escape underscores
            char *cmd; 
            strPtr = str_copyCmd(strPtr, &cmd);
            dbg("cmdHere: %s\n", cmd);
            if(str_equal(cmd, "end")) {
                char arg[128];
                strPtr = getStrArg(strPtr, arg);
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
    assert(0);
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
            dbg("cmd: %s\n", cmd);
            if(str_equal(cmd, "href")) {
                char link[2048];
                char linkText[1024];
                strCur = getStrArg(strCur, link);
                strCur++;
                strCur = getStrArg(strCur, linkText);
                strCur++;
                dbg("href-link: %s, text: %s\n", link, linkText);
                result = str_concat(result, "<a href=\"");
                result = str_concat(result, link);
                result = str_concat(result, "\">");
                result = str_concat(result, linkText);
                result = str_concat(result, "</a>");

                strPtr = strCur;
            } else if(str_equal(cmd, "includegraphics")) {
                char link[2048];
                strCur = getStrArg(strCur, link);
                strCur++;
                dbg("img-link: %s\n", link);
                result = str_concat(result, "<img src=\"");
                result = str_concat(result, link);
                result = str_concat(result, "\"><br><br>");

                strPtr = strCur;
            } else if(str_equal(cmd, "section")) {
                char sectionTitle[1024];
                strCur = getStrArg(strCur, sectionTitle);
                strCur++;
                dbg("sectionTitle: %s\n", sectionTitle);
                result = str_concat(result, "<br>\n<h3>");
                result = str_concat(result, sectionTitle);
                result = str_concat(result, "</h3>\n");

                strPtr = strCur;
            } else if(str_equal(cmd, "begin")) {
                char beginType[1280];
                strCur = getStrArg(strCur, beginType);
                strCur++;
                str_equal(beginType, "blockquote");
                dbg("insideBeginType: %s\n", beginType);
                if(str_equal(beginType, "blockquote")) {
                    result = str_concat(result, "<blockquote>\n");
                    char *quoteText;
                    strCur = getTextToEnd(strCur, "blockquote", &quoteText);
                    char *quoteTextStart = quoteText;
                    while(*quoteText != '\0') {
                        if(*quoteText == '\n') {
                            *quoteText = '\0';
                            result = str_concat(result, quoteTextStart);
                            result = str_concat(result, "<br>\n");
                            quoteTextStart = quoteText + 1;
                        }
                        quoteText++;
                    }
                    result = str_concat(result, quoteText);
                    result = str_concat(result, "\n</blockquote>\n");

                    strCur++;
                    strPtr = strCur;
                } else if(str_equal(beginType, "itemize")) {
                    // TODO
                }
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
            char **arr = process_cmd(&str);
            dbg("arr[0]: %s", arr[0]);
            if(str_beginswith(arr[0], "_")) {
                continue; 
            } else if(str_equal(arr[0], "title")) {
                assert(arr_len(arr) == 2);
                xmemcpy(post.title, arr[1], str_len(arr[1])+1); 
            } else if(str_equal(arr[0], "author")) {
                assert(arr_len(arr) == 2);
                xmemcpy(post.author, arr[1], str_len(arr[1])+1);
            } else if(str_equal(arr[0], "date")) {
                assert(arr_len(arr) == 4);
                post.dateYear = str_toint(arr[1]);
                post.dateMonth = str_toint(arr[2]);
                post.dateDay = str_toint(arr[3]);
            } else if(str_equal(arr[0], "begin")) {
                assert(arr_len(arr) == 2);
                if(str_equal(arr[1], "blurb")) {
                    str = getTextToEnd(str, "blurb", &post.blurb);
                    int i = 0;
                } else if(str_equal(arr[1], "document")) {
                    str = getTextToEnd(str, "document", &post.body);
                    int i = 0;
                }
            }
        } else {
            str++;
        }
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
            dbg("cmd: %s\n", cmd);
            if(str_equal(cmd, "title")) {
                str = getStrArg(str, title);
                dbg("title: %s\n", title);
                free(cmd);
                return str_copy(title);
            }
        }
        str++;
        //TODO: If \series, add it?
    }
    assert(0);
}

char *get_date(char *str) {
    char dateYear[128];
    char dateMonth[128];
    char dateDay[128];
    while(*str != '\0') {
        if(*str == '\\') {
            char **arr = process_cmd(&str);
            if(str_equal(arr[0], "date")) {
                assert(arr_len(arr) == 4);
                return date_nice(str_toint(arr[1]), str_toint(arr[2]), str_toint(arr[3]));;
            }
        } else {
            str++;
        }
    }
    assert(0);
}

char *convert_body(char* str) {
    Post post = createPost(str);

    post.body = convertToHtml(post.body);

    char *html = str_copy("<h1>");
    html = str_concat(html, post.title);
    html = str_concat(html, "</h1>\n");

    if(!str_equal(post.title, "About")) {
        html = str_concat(html, "<h2>");

        char *date = date_nice(post.dateYear, post.dateMonth, post.dateDay);
        html = str_concat(html, date);
        free(date);

        html = str_concat(html, "</h2>\n");
    }
    html = str_concat(html, post.body);
    return html;
}

int main() {
    //FILE* fp = fopen("../content/0005-behind-the-libraries-inquiring-into-interrupts.post", "r");
    FILE* fp = fopen("../content/0003-behind-the-libraries-plunge-into-pinmode.post", "r");

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(fsize + 1);
    fread(buffer, fsize, 1, fp);
    fclose(fp);

    //dbg("%s", get_date(buffer));
    //dbg("%s\n", convert_body(buffer));
    char *body = convert_body(buffer);
    for(int i = 0; i < 15224; i++) {
        printf("%c", *body);
        body++;
    }
}
