#include "syscalls.h"
#include "str.h"
#include "url.h"

int remove_item(int* array, int count, int item) {
    for (int i = 0; i < count; i++) {
        if (array[i] == item) {
            array[i] = array[count - 1];
            --count;
        }
    }
    return count;
}

void write_num(int handle, int num) {
    char buf[16];
    int len = str_from_num(buf, num);
    write_(handle, buf, len);
}

void write_str(int handle, const char* str){
    os_size len = str_len(str);
    write_(handle, str, len);
}

void netstatus(int conn){
    struct netconn conns[16];
    int count = netinfo(conns, 16);
    for (int i = 0; i < count; i++) {
        write_num(conn, conns[i].pid);
        write_(conn, " ", 1);
        write_str(conn, conns[i].addr);
        write_(conn, "\n", 1);
    }
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
    os_size outSize = read_(conn, buff, sizeof(buff));
    // TODO: parse buff and return Content-Length
    return 65535;
}

// read all data from socket conn into buff up to size
os_size readAll(int conn, char* buff, os_size size){
    // TODO: read all data from socket conn into buffer instead of a single read
    os_size outSize = read_(conn, buff, size);
}

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

void handle_command(int conn, char* args[], int argc) {
    // TODO: missing commands: uptime, history, quit(disconnect from shell)
    // static long startTime = gettime(); // for uptime
    if (str_cmp(args[0], "help") == 0) {
        const char* msg = "help, sleep, shutdown, reboot, kill, time, ps, mem, netstat, run\n";
        write_str(conn, msg);
    } else if (str_cmp(args[0], "sleep") == 0 && argc>1) { // sleep 1000
        sleep_(str_to_num(args[1]));
    } else if (str_cmp(args[0], "wait") == 0 && argc>1) { // wait <pid>
        int pid = str_to_num(args[1]);
        pwait(pid);

    } else if (str_cmp(args[0], "shutdown") == 0) {
        shutdown_();
    }
    else if (str_cmp(args[0], "reboot") == 0) {
        reboot();
    }
    else if (str_cmp(args[0], "kill") == 0 && argc>1) { // kill <pid>
        pkill(str_to_num(args[1]), 9);
    }
    else if (str_cmp(args[0], "time") == 0) {
        write_num(conn, gettime());
        write_(conn, "\n", 1);
    }
    else if (str_cmp(args[0], "ps") == 0) {
        int pids[32];
        int count = ps(pids, 32);
        for (int i = 0; i < count; i++) {
            write_num(conn, pids[i]);
            write_(conn, "\n", 1);
        }
    }
    else if (str_cmp(args[0], "mem") == 0) {
        os_size used = meminfo(pid());
        write_str(conn, "Used: ");
        write_num(conn, used);
        write_(conn, "\n", 1);
    }
    else if (str_cmp(args[0], "netstat") == 0) {
        netstatus(conn);
    }
    else if (str_cmp(args[0], "run") == 0 && argc>1) { // run http://localhost:8080/myexe
        downloadRun(args[1], conn);
    }
    else {
        write_str(conn, "Invalid command\n");
    }
}

int main() {
    int server = listen_("tcp://:2222");
    int clients[16];
    int client_count = 0;
    
    while (1) {
        clients[client_count] = server;
        int ready = select_(clients, client_count + 1, 1000);
        if (ready <= 0) continue;

        if (ready == server) {  // Check for new connections
            int new_client = accept_(server);
            if (new_client >= 0 && client_count < 15) {
                clients[client_count++] = new_client;
                write_str(new_client, "$ ");
            }
            continue;
        }

        char buf[256];
        int len = read_(ready, buf, sizeof(buf) - 1);
        if (len <= 0) {
            close_(ready);
            client_count = remove_item(clients, client_count, ready);
            continue;
        }
        buf[len] = 0;

        char* args[8];
        int argc = str_parse(buf, args);
        if (argc > 0 && *args[0]) { // at least one token and args[0] does not point to an empty string
            handle_command(ready, args, argc);
        }
        write_str(ready, "$ ");
    }
}
