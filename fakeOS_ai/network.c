#include "syscalls.h"
#include "kernel.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int netinfo(struct netconn* conns, int max_count) {
    // Simplified - would need to parse /proc/net/tcp
    return 0;
}

int open(const char* url) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error(errno);
        return -1;
    }
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) return sock;
    close(sock);
    error(errno);
    return -1;
}

int read(int handle, void* buf, size_t len) {
    ssize_t n = read(handle, buf, len);
    if (n >= 0) return n;
    error(errno);
    return -1;
}

int write(int handle, const void* data, size_t len) {
    ssize_t n = write(handle, data, len);
    if (n >= 0) return n;
    error(errno);
    return -1;
}

int listen(const char* url) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error(errno);
        return -1;
    }
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0 || listen(sock, 5) < 0) {
        close(sock);
        error(errno);
        return -1;
    }
    return sock;
}

int accept(int handle) {
    int client = accept(handle, NULL, NULL);
    if (client >= 0) return client;
    error(errno);
    return -1;
}

int select(int* handles, int count, int timeout) {
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

int getpeername(int handle, char* addr, size_t addr_len) {
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

int close(int handle) {
    if (close(handle) == 0) return 0;
    error(errno);
    return -1;
}
