#include "syscalls.h"
#include "str.h"
#include "url.h"
#include "http.h" // downloadRun()

static long startTime; // for uptime
int clients[16];
int client_count = 0;

int remove_item(int* array, int count, int item) {
    for (int i = 0; i < count; i++) {
        if (array[i] == item) {
            array[i] = array[count - 1];
            --count;
        }
    }
    return count;
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

void uptime(int conn){
    long time = gettime();
    long upt = time - startTime;
    long s = upt % 60;
    long m = (upt/60) % 60;
    long h = (upt/(60*60)) % 24;
    long d = upt/(60*60*24);
    write_num(conn, d);
    write_str(conn, "d ");
    write_num(conn, h);
    write_str(conn, "h ");
    write_num(conn, m);
    write_str(conn, "m ");
    write_num(conn, s);
    write_str(conn, "s\n");
}

// TODO: missing commands: history
void handle_command(int conn, char* args[], int argc) {
    if (str_cmp(args[0], "help") == 0) {
        const char* msg = "help, sleep, shutdown, reboot, kill, time, ps, mem, netstat, run, quit, uptime\n";
        write_str(conn, msg);
    } else if (str_cmp(args[0], "quit") == 0) {
        close_(conn);
        client_count = remove_item(clients, client_count, conn);
    } else if (str_cmp(args[0], "uptime") == 0) {
        uptime(conn);
    } else if (str_cmp(args[0], "sleep") == 0 && argc>1) { // sleep 1000
        sleep_(str_to_num(args[1]));
    } else if (str_cmp(args[0], "wait") == 0 && argc>1) { // wait <pid>
        int pid = str_to_num(args[1]);
        pwait(pid);
    } else if (str_cmp(args[0], "shutdown") == 0) {
        shutdown_();
    } else if (str_cmp(args[0], "reboot") == 0) {
        reboot();
    } else if (str_cmp(args[0], "kill") == 0 && argc>1) { // kill <pid>
        pkill(str_to_num(args[1]), 9);
    } else if (str_cmp(args[0], "time") == 0) {
        write_num(conn, gettime());
        write_(conn, "\n", 1);
    } else if (str_cmp(args[0], "ps") == 0) {
        int pids[32];
        int count = ps(pids, 32);
        for (int i = 0; i < count; i++) {
            write_num(conn, pids[i]);
            write_(conn, "\n", 1);
        }
    } else if (str_cmp(args[0], "mem") == 0) {
        os_size used = meminfo(pid());
        write_str(conn, "Used: ");
        write_num(conn, used);
        write_(conn, "\n", 1);
    } else if (str_cmp(args[0], "netstat") == 0) {
        netstatus(conn);
    } else if (str_cmp(args[0], "run") == 0 && argc>1) { // run http://localhost:8080/myexe
        downloadRun(args[1], conn);
    } else {
        write_str(conn, "Invalid command\n");
    }
}

int main() {
    startTime = gettime(); // save start time
    int server = listen_("tcp://:2222");
    
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
        if (argc > 0 && *args[0]) { // at least one token plus args[0] does not point to an empty string
            handle_command(ready, args, argc);
        }
        write_str(ready, "$ ");
    }
}
