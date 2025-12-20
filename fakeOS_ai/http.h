#ifndef INCLUDED_HTTP_H
#define INCLUDED_HTTP_H
#include "syscalls.h" // os_size

void write_num(int handle, int num);
void write_str(int handle, const char* str);
os_size readAll(int conn, char* buff, os_size size);
void downloadRun(const char* url, int conn);

#endif // INCLUDED_HTTP_H