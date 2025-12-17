#include "syscalls.h"
#include "kernel.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

static int spawned_pids[1024];
static int spawned_count = 0;

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

static void deletePid(int pid){
    for (int i = 0; i < spawned_count; i++) {
        if (spawned_pids[i] == pid) {
            spawned_pids[i] = spawned_pids[--spawned_count];
            break;
        }
    }
}

void exit(int status) {
    int my_pid = getpid();
    deletePid(my_pid);
    _exit(status);
}

int spawn(void* addr) {
    pid_t pid = fork();
    if (pid == 0) {
        ((void(*)())addr)();
        _exit(0);
    }
    if (pid > 0 && spawned_count < 1024) {
        spawned_pids[spawned_count++] = pid;
        return pid;
    }
    error(errno);
    return -1;
}

int wait(int pid) {
    int status;
    int result = waitpid(pid, &status, 0);
    if (result > 0) return WEXITSTATUS(status);
    error(errno);
    return -1;
}

int getpid(void) {
    return getpid();
}

int kill(int pid, int signal) {
    if (kill(pid, signal) == 0) {
        deletePid(pid);
        return 0;
    }
    error(errno);
    return -1;
}

int ps(int* pids, int max_count) {
    int count = 0;
    for (int i = 0; i < spawned_count && count < max_count; i++) {
        if (kill(spawned_pids[i], 0) == 0) {
            pids[count++] = spawned_pids[i];
        }
    }
    return count;
}
