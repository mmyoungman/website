#include "lib-mmy.h"

char *str_copyToPtr(char *start, char *end) {
    char *res = 0;
    //assert(start < end); // Doesn't work? UB?
    while(start != end) {
        arr_push(res, *start);
        start++;
    }
    arr_push(res, '\0');
    return res;
}

char **process_cmd(char **str) {
    char **res = 0;
    char *cmd = 0;

    assert(**str == '\\');
    (*str)++;
    char *ptr = *str;

    // TODO: This is broke
    if(*ptr == '_' || *ptr == '\\') { // escaped chars
        arr_push(cmd, *ptr);
        arr_push(res, cmd);
        (*str)++;
        return res;
    }

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

char *getTextToEnd(char *str, char *type, char **text) {
    char *strPtr = str; 
    while(*strPtr != '\0') {
        if(*strPtr == '\\') {
            char *endCmdStart = strPtr;
            char **arr = process_cmd(&strPtr);
            if(str_beginswith(arr[0], "_")) {
                continue;
            }
            // TODO: Handle double backslash
            if(str_equal(arr[0], "end")) {
                assert(arr_len(arr) == 2);
                if(str_equal(arr[1], type)) {
                    *text = str_copyToPtr(str, endCmdStart);
                    arr_free(arr);
                    return strPtr;
                }
            } 
        } else {
            strPtr++;
        }
    }
    log_err("\\end{%s} not found after \\begin{%s}!", type, type);
    assert(0);
}

char *convertToHtml(char *str) {
    char *result = str_copy(""); 
    char *strPtr = str;
    char *strCur = str;

    while(*strCur != '\0') {
        if(*strCur == '\\') {
            char *prevText = str_copyToPtr(strPtr, strCur);
            result = str_concat(result, prevText);
            char **arr = process_cmd(&strCur);

            if(str_equal(arr[0], "href")) {
                assert(arr_len(arr) == 3);
                char link[2048];
                char linkText[1024];
                dbg("href-link: %s, text: %s\n", arr[1], arr[2]);
                result = str_concat(result, "<a href=\"");
                result = str_concat(result, arr[1]);
                result = str_concat(result, "\">");
                result = str_concat(result, arr[2]);
                result = str_concat(result, "</a>");
            } else if(str_equal(arr[0], "includegraphics")) {
                assert(arr_len(arr) == 2);
                char link[2048];
                dbg("img-link: %s\n", arr[1]);
                result = str_concat(result, "<img src=\"");
                result = str_concat(result, arr[1]);
                result = str_concat(result, "\"><br><br>");
            } else if(str_equal(arr[0], "section")) {
                assert(arr_len(arr) == 2);
                char sectionTitle[1024];
                dbg("sectionTitle: %s\n", arr[1]);
                result = str_concat(result, "<br>\n<h3>");
                result = str_concat(result, arr[1]);
                result = str_concat(result, "</h3>\n");
            } else if(str_equal(arr[0], "begin")) {
                assert(arr_len(arr) == 2);
                if(str_equal(arr[1], "blockquote")) {
                    result = str_concat(result, "<blockquote>\n");
                }
            } else if(str_equal(arr[0], "end")) {
                assert(arr_len(arr) == 2);
                if(str_equal(arr[1], "blockquote")) {
                    result = str_concat(result, "</blockquote>\n");
                }
            } else if(str_equal(arr[0], "newline")) {
                log_info("NEWLINE!");
                assert(arr_len(arr) == 1);
                result = str_concat(result, "<br>");
            } else if(str_equal(arr[0], "tab")) {
                log_info("TAB!");
                assert(arr_len(arr) == 1);
                result = str_concat(result, "&nbsp;&nbsp;");
            } else {
                // TODO: Unknown command
            }
            strPtr = strCur;
        } else if(*strCur == '\n') {
            char *prevText = str_copyToPtr(strPtr, strCur);
            result = str_concat(result, prevText);
            result = str_concat(result, "<br>");
            strPtr = strCur;
        }
        strCur++;
    }
    result = str_concat(result, strPtr);
    return result;
}

typedef struct Post {
    char title[1024];
    char author[128];
    int dateYear;
    int dateMonth;
    int dateDay;
    char series[1024];
    char seriesPart[128];
    char **tags;
    char **categories;
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
                xmemcpy(post.title, arr[1], str_len(arr[1])); 
                post.title[str_len(arr[1])] = '\0';
            } else if(str_equal(arr[0], "author")) {
                assert(arr_len(arr) == 2);
                xmemcpy(post.author, arr[1], str_len(arr[1]));
                post.author[str_len(arr[1])] = '\0';
            } else if(str_equal(arr[0], "date")) {
                assert(arr_len(arr) == 4);
                post.dateYear = str_toint(arr[1]);
                post.dateMonth = str_toint(arr[2]);
                post.dateDay = str_toint(arr[3]);
            } else if(str_equal(arr[0], "series")) {
                assert(arr_len(arr) == 3);
                xmemcpy(post.series, arr[1], str_len(arr[1]));
                post.series[str_len(arr[1])] = '\0';
                xmemcpy(post.seriesPart, arr[2], str_len(arr[2]));
                post.seriesPart[str_len(arr[2])] = '\0';
            } else if(str_equal(arr[0], "begin")) {
                assert(arr_len(arr) == 2);
                if(str_equal(arr[1], "blurb")) {
                    str = getTextToEnd(str, "blurb", &post.blurb);
                } else if(str_equal(arr[1], "document")) {
                    str = getTextToEnd(str, "document", &post.body);
                }
            }
        } else {
            str++;
        }
    }
    dbg("post.body:\n %s \n post.body end", post.body);
    return post;
}

char *get_title(char *str) {
    while(*str != '\0') {
        if(*str == '\\') {
            char **arr = process_cmd(&str);
            dbg("cmd: %s\n", arr[0]);
            if(str_equal(arr[0], "title")) {
                assert(arr_len(arr) == 2);
                dbg("title: %s\n", arr[1]);
                return arr[1];
            }
        } else {
            str++;
        }
    }
    log_err("No \\title{} found!");
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
    if(str_isint(post.seriesPart)) {
        html = str_concat(html, " - ");
        html = str_concat(html, post.series);
        html = str_concat(html, " - Part ");
        html = str_concat(html, post.seriesPart);
    }
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

// For debugging
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
    dbg("%s\n", convert_body(buffer));

    //char *body = convert_body(buffer);
    //for(int i = 0; i < 43164; i++) {
    //    printf("%c", *body);
    //    body++;
    //}
}
