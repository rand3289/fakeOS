#ifndef INCLUDED_URL_H
#define INCLUDED_URL_H

typedef struct {
    char protocol[16];
    char hostname[256];
    int  port;
    char path[512];
} url_parts_t;

int parse_url(const char* url, url_parts_t* parts);

#endif // #ifndef INCLUDED_URL_H
