#include "http.h"
#include "url.h"
#include "str.h"

void write_num(int handle, int num) {
    char buf[16];
    int len = str_from_num(buf, num);
    write_(handle, buf, len);
}

void write_str(int handle, const char* str){
    os_size len = str_len(str);
    write_(handle, str, len);
}

// Typical http response:
// HTTP/1.0 200 OK\r\nServer: SimpleHTTP/0.6 Python/3.13.9\r\n
// Date: Fri, 19 Dec 2025 02:10:12 GMT\r\n
// Content-type: text/x-python\r\n
// Content-Length: 279\r\n
// Last-Modified: Thu, 18 Dec 2025 08:38:17 GMT\r\n
// \r\n

// read HTTP 1.1 response from conn socket and return Content-Length
os_size parseHttpResponse(int conn){
    char buff[1024];
    os_size outSize = read_(conn, buff, sizeof(buff)-1);
    buff[outSize] = 0;
    
    char* pos = str_find(buff, "Content-Length: ");
    if (pos) {
        pos += 16; // skip "Content-Length: "
        return str_to_num(pos);
    }
    return -1;
}

// read all data from socket conn into buff up to size
os_size readAll(int conn, char* buff, os_size size){
    os_size total = 0;
    while (total < size) {
        os_size bytes = read_(conn, buff + total, size - total);
        if (bytes <= 0) break;
        total += bytes;
    }
    return total;
}

// TODO: break thiss proc into two (download and run)
void downloadRun(const char* url, int conn){
    url_parts_t parts;
    int dl = open_(url);
    if (dl >= 0 && parse_url(url, &parts) == 0) { // TODO: could leak handles???
        write_str(dl,"GET ");
        write_str(dl, parts.path);
        write_str(dl, " HTTP/1.1\r\n\r\n");
        os_size size = parseHttpResponse(dl);
        void* prog = mmap_(size, 0);
        os_size rdSize = readAll(dl, prog, size);
        close_(dl);
        if (size == rdSize) {
            int pid = spawn(prog); // TODO: pass conn handle to spawn() to allow the new process to communicate with user
            write_num(conn, pid);
            write_(conn, "\n", 1);
            pwait(pid); // TODO: munmap() first ???
        }
        munmap_(prog, size);
    }
}
