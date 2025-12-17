#include "syscalls.h"
#define _GNU_SOURCE // make usleep() available
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
    usleep(ms * 1000);
}

long gettime(void) {
    return time(NULL);
}
