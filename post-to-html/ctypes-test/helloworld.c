#include "lib-mmy.h"

char *helloworld(char *name, int len) {
    char *result = str_copy("Hello ");
    str_concat(result, name);
    str_concat(result, "!");
    return result;
}
