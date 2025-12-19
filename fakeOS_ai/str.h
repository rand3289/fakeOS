#ifndef INCLUDED_STR_H
#define INCLUDED_STR_H

int  str_cmp(const char* a, const char* b);
int  str_len(const char* str);
int  str_to_num(const char* str);
int  str_from_num(char* str, int num);
int  str_parse(char* str, char* parts[]);
void str_reverse(char* str, int len);
char* str_find(const char* str, const char* substr);

#endif