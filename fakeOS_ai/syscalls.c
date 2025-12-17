#include "syscalls.h"
#include "kernel.h"
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct proc_errno {
    int pid;
    int error_code;
};

static struct proc_errno proc_errors[1024];
static int error_count = 0;

void error(int err) {
    int pid = getpid();
    for (int i = 0; i < error_count; i++) {
        if (proc_errors[i].pid == pid) {
            proc_errors[i].error_code = err;
            return;
        }
    }
    if (error_count < 1024) {
        proc_errors[error_count].pid = pid;
        proc_errors[error_count++].error_code = err;
    }
}

int errno(void) {
    int pid = getpid();
    for (int i = 0; i < error_count; i++) {
        if (proc_errors[i].pid == pid) {
            return proc_errors[i].error_code;
        }
    }
    return 0;
}

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
