#ifndef INCLUDED_KERNEL_H
#define INCLUDED_KERNEL_H
// these are kernel only syscalls not available to user level programs

struct proc_info {
    int pid;
    int error_code;
};
struct proc_info* get_proc_info(int pid); // get information about a process

void error(int err); // Set process' error code retrievable by errno() syscall

#endif
