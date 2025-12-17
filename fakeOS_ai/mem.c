#include "syscalls.h"
#include "kernel.h"
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>

void* mmap(size_t size, int flags) {
    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr != MAP_FAILED) {
        struct proc_info* pi = get_proc_info(pid());
        if (pi) pi->memSize += size;
        return addr;
    }
    error(errno);
    return NULL;
}

int munmap(void* addr, size_t size) {
    if (munmap(addr, size) == 0) {
        struct proc_info* pi = get_proc_info(pid());
        if (pi) pi->memSize -= size;
        return 0;
    }
    error(errno);
    return -1;
}

size_t meminfo(int pid) {
    struct proc_info* pi = get_proc_info(pid);
    if (pi) {
        return pi->memSize;
    }
    error(ESRCH);
    return -1;
}
