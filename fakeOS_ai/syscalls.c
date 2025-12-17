#include "syscalls.h"
#include "kernel.h"
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void shutdown(void) {
    sync();
    system("shutdown -h now");
}

void reboot(void) {
    sync();
    system("reboot");
}

void sleep(int ms) {
    usleep(ms * 1000);
}

long gettime(void) {
    return time(NULL);
}
