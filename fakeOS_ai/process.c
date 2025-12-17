#include "syscalls.h"
#include "kernel.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

struct proc_info {
    int pid;
    int error_code;
};

static struct proc_info processes[1024];
static int proc_count = 0;

static struct proc_info* get_proc_info(int pid) {
    for (int i = 0; i < proc_count; i++) {
        if (processes[i].pid == pid) return &processes[i];
    }
    if (proc_count < 1024) {
        processes[proc_count].pid = pid;
        processes[proc_count].error_code = 0;
        return &processes[proc_count++];
    }
    return NULL;
}

void error(int err) {
    struct proc_info* pi = get_proc_info(getpid());
    if (pi) pi->error_code = err;
}

int errno(void) {
    struct proc_info* pi = get_proc_info(getpid());
    return pi ? pi->error_code : 0;
}

static void deletePid(int pid){
    for (int i = 0; i < proc_count; i++) {
        if (processes[i].pid == pid) {
            processes[i] = processes[--proc_count];
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
    if (pid > 0) {
        struct proc_info* pi = get_proc_info(pid);
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

int pkill(int pid, int signal) {
    if (kill(pid, signal) == 0) {
        deletePid(pid);
        return 0;
    }
    error(errno);
    return -1;
}

int ps(int* pids, int max_count) {
    int count = 0;
    for (int i = 0; i < proc_count && count < max_count; i++) {
        if (kill(processes[i].pid, 0) == 0) {
            pids[count++] = processes[i].pid;
        }
    }
    return count;
}
