#ifndef INCLUDED_KERNEL_H
#define INCLUDED_KERNEL_H
#include "syscalls.h"
// these are kernel only functions not available to user level programs

struct proc_info {
    int pid;
    int error_code;
    os_size memSize;
};
struct proc_info* get_proc_info(int pid); // get information about a process

void error(int err); // Set process' error_code retrievable by errnum() syscall

#endif
