#include "syscalls.h"
#include "kernel.h"
#include "url.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int netinfo(struct netconn* conns, int max_count) {
    (void) conns;
    (void) max_count;
    // TODO:
    return 0;
}

int open_(const char* url) {
    // TODO: create a structure to keep sock and protocol and return index instead
    // if protocol is not supported, return an error instead
    url_parts_t parts;
    if (parse_url(url, &parts) != 0) {
        error(errno);
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error(errno);
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(parts.port);
    inet_pton(AF_INET, parts.hostname, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) return sock;
    close(sock);
    error(errno);
    return -1;
}

int listen_(const char* url) {
    // TODO: create a structure to keep sock and protocol and return index instead
    // if protocol is not supported, return an error instead
    url_parts_t parts;
    if (parse_url(url, &parts) != 0) {
        error(errno);
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error(errno);
        return -1;
    }
    const int enable = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(parts.port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0 || listen(sock, 5) < 0) {
        close(sock);
        error(errno);
        return -1;
    }
    return sock;
}

int accept_(int handle) {
    int client = accept(handle, NULL, NULL);
    if (client >= 0) return client;
    error(errno);
    return -1;
}

int select_(int* handles, int count, int timeout) {
    fd_set fds;
    FD_ZERO(&fds);
    int max_fd = 0;
    
    for (int i = 0; i < count; i++) {
        FD_SET(handles[i], &fds);
        if (handles[i] > max_fd) max_fd = handles[i];
    }
    
    struct timeval tv = {timeout / 1000, (timeout % 1000) * 1000};
    int n = select(max_fd + 1, &fds, NULL, NULL, timeout >= 0 ? &tv : NULL);
    
    if (n > 0) {
        for (int i = 0; i < count; i++) {
            if (FD_ISSET(handles[i], &fds)) return handles[i];
        }
    }
    if (n == 0) return 0;
    error(errno);
    return -1;
}

int read_(int handle, void* buf, os_size len) {
    os_size n = read(handle, buf, len);
    if (n >= 0) return n;
    error(errno);
    return -1;
}

int write_(int handle, const void* data, os_size len) {
    os_size n = write(handle, data, len);
    if (n >= 0) return n;
    error(errno);
    return -1;
}

int getpeername_(int handle, char* addr, os_size addr_len) {
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    if (getpeername(handle, (struct sockaddr*)&peer, &len) < 0) {
        error(errno);
        return -1;
    }
    strncpy(addr, inet_ntoa(peer.sin_addr), addr_len - 1);
    addr[addr_len - 1] = '\0';
    return 0;
}

int close_(int handle) {
    if (close(handle) == 0) return 0;
    error(errno);
    return -1;
}
