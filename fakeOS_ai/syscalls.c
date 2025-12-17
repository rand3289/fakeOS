#include "syscalls.h"
#include <unistd.h>
#include <time.h>

void shutdown_(void) {
// TODO: log to system console
    exit(0); // exit this "kernel" process
}

void reboot(void) {
// TODO: kill all running processes, and start shell
}

void sleep_(int ms) {
// TODO:    usleep(ms * 1000);
}

long gettime(void) {
    return time(NULL);
}
