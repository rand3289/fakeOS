#include "syscalls.h"
#include "kernel.h"
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void shutdown(void) {
// TODO: log to system console
    exit(0); // exit this "kernel" process
}

void reboot(void) {
// TODO: kill all running processes, and start shell
}

void sleep(int ms) {
    usleep(ms * 1000);
}

long gettime(void) {
    return time(NULL);
}
