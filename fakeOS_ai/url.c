#include "url.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* str_dup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}


int parse_url(const char* url, url_parts_t* parts) {
    if (!url || !parts) return -1;
    
    // Initialize
    memset(parts, 0, sizeof(url_parts_t));
    strcpy(parts->path, "/");
    
    char* temp = str_dup(url);
    char* ptr = temp;
    
    // Extract protocol
    char* proto_end = strstr(ptr, "://");
    if (!proto_end) {
        free(temp);
        return -1;
    }
    *proto_end = '\0';
    strncpy(parts->protocol, ptr, sizeof(parts->protocol) - 1);
    ptr = proto_end + 3;
    
    // Find path start
    char* path_start = strchr(ptr, '/');
    if (path_start) {
        strncpy(parts->path, path_start, sizeof(parts->path) - 1);
        *path_start = '\0';
    }
    
    // Extract hostname and port
    char* port_start = strchr(ptr, ':');
    if (port_start) {
        *port_start = '\0';
        parts->port = atoi(port_start + 1);
    } else {
        parts->port = (strcmp(parts->protocol, "https") == 0) ? 443 : 80;
    }
    
    strncpy(parts->hostname, ptr, sizeof(parts->hostname) - 1);
    free(temp);
    return 0;
}
