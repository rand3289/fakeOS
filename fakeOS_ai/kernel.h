#ifndef INCLUDED_KERNEL_H
#define INCLUDED_KERNEL_H
// these are kernel only syscalls not available to user level programs


void error(int err); // Set process' error code retrievable by errno() syscall

#endif
