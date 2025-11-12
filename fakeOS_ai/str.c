#include "str.h"

int str_cmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return *a - *b;
}

int str_len(const char* str) {
    int len = 0;
    while (*str++) len++;
    return len;
}

void str_reverse(char* str, int len) {
    for (int j = 0; j < len / 2; j++) {
        char temp = str[j];
        str[j] = str[len - 1 - j];
        str[len - 1 - j] = temp;
    }
}

int str_to_num(const char* str) {
    int result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

int str_from_num(char* buf, int num) {
    int i = 0;
    if (num == 0) { buf[i++] = '0'; }
    else {
        while (num > 0) {
            buf[i++] = '0' + (num % 10);
            num /= 10;
        }
    }
    buf[i] = 0;
    str_reverse(buf, i);
    return i;
}

int str_parse(char* buf, char* args[]) {
    int argc = 0;
    char* p = buf;
    
    while (*p && argc < 7) {
        while (*p == ' ') p++;
        if (!*p) break;
        args[argc++] = p;
        while (*p && *p != ' ') p++;
        if (*p) *p++ = 0;
    }
    return argc;
}
