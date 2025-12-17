#include "syscalls.h"
#include "str.h"

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
    write(handle, buf, len);
}

void handle_command(int conn, char* args[], int argc) {
    // missing commands: wait <pid>, uptime, help, history
    if (str_cmp(args[0], "sleep") == 0 && argc > 1) {
        sleep(str_to_num(args[1]));
    }
    else if (str_cmp(args[0], "shutdown") == 0) {
        shutdown();
    }
    else if (str_cmp(args[0], "reboot") == 0) {
        reboot();
    }
    else if (str_cmp(args[0], "kill") == 0 && argc > 1) {
        pkill(str_to_num(args[1]), 9);
    }
    else if (str_cmp(args[0], "time") == 0) {
        write_num(conn, gettime());
        write(conn, "\n", 1);
    }
    else if (str_cmp(args[0], "ps") == 0) {
        int pids[32];
        int count = ps(pids, 32);
        for (int i = 0; i < count; i++) {
            write_num(conn, pids[i]);
            write(conn, "\n", 1);
        }
    }
    else if (str_cmp(args[0], "mem") == 0) {
        struct memstat info;
        meminfo(&info);
        write(conn, "Total: ", 7);
        write_num(conn, info.total);
        write(conn, " Used: ", 7);
        write_num(conn, info.used);
        write(conn, "\n", 1);
    }
    else if (str_cmp(args[0], "netstat") == 0) {
        struct netconn conns[16];
        int count = netinfo(conns, 16);
        for (int i = 0; i < count; i++) {
            write_num(conn, conns[i].pid);
            write(conn, " ", 1);
            write(conn, conns[i].addr, str_len(conns[i].addr));
            write(conn, "\n", 1);
        }
    }
    else if (str_cmp(args[0], "run") == 0 && argc > 1) {
        int dl = open(args[1]);
        if (dl >= 0) {
            void* prog = mmap(65536, 0); // LOL... ai hacked it up by hardcoding the program size
            int size = read(dl, prog, 65536);
            close(dl);
            if (size > 0) {
                int pid = spawn(prog); // TODO: pass conn haneld to spawn() to allow the new process to communicate with user
                write_num(conn, pid);
                write(conn, "\n", 1);
                wait(pid); // TODO: munmap() first ???
            }
            munmap(prog, 65536);
        }
    }
    else {
        write(conn, "Invalid command\n", 16);
    }
}

int main() {
    int server = listen("tcp://:22");
    int clients[16];
    int client_count = 0;
    
    while (1) {
        clients[client_count] = server;
        int ready = select(clients, client_count + 1, 1000);
        if (ready < 0) continue;

        if (ready == server) {  // Check for new connections
            int new_client = accept(server);
            if (new_client >= 0 && client_count < 15) {
                clients[client_count++] = new_client;
                write(new_client, "$ ", 2);
            }
            continue;
        }

        char buf[256];
        int len = read(ready, buf, sizeof(buf) - 1);
        if (len <= 0) {
            close(ready);
            client_count = remove_item(clients, client_count, ready);
            continue;
        }
        buf[len] = 0;

        char* args[8];
        int argc = str_parse(buf, args);
        if (argc > 0) {
            handle_command(ready, args, argc);
        }
        write(ready, "$ ", 2);
    }
}
