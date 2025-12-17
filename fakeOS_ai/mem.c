#include "syscalls.h"
#include "kernel.h"
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

struct proc_mem {
    int pid;
    size_t allocated;
};

static struct proc_mem proc_memory[1024];
static int proc_count = 0;

static struct proc_mem* get_proc_mem(int pid) {
    for (int i = 0; i < proc_count; i++) {
        if (proc_memory[i].pid == pid) return &proc_memory[i];
    }
    if (proc_count < 1024) {
        proc_memory[proc_count].pid = pid;
        proc_memory[proc_count].allocated = 0;
        return &proc_memory[proc_count++];
    }
    return NULL;
}

void* mmap(size_t size, int flags) {
    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr != MAP_FAILED) {
        struct proc_mem* pm = get_proc_mem(pid());
        if (pm) pm->allocated += size;
        return addr;
    }
    error(errno);
    return NULL;
}

int munmap(void* addr, size_t size) {
    if (munmap(addr, size) == 0) {
        struct proc_mem* pm = get_proc_mem(pid());
        if (pm) pm->allocated -= size;
        return 0;
    }
    error(errno);
    return -1;
}

int meminfo(struct memstat* info) {
    struct proc_mem* pm = get_proc_mem(pid());
    if (pm) {
        info->used = pm->allocated;
        info->total = pm->allocated + 1024*1024; // Assume 1MB available
        return 0;
    }
    error(ESRCH);
    return -1;
}
