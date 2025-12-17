#ifndef INCLUDED_SYSCALLS_H
#define INCLUDED_SYSCALLS_H
// these are syscalls and data structures available in fakeOS to user processes

typedef unsigned long long os_size;
struct netconn { int pid; char addr[64]; };

// Process Management
void exit(int status);                                     // Terminate current process
int spawn(void* addr);                                     // Create new process with code at address
int pwait(int pid);                                        // Wait for child process to terminate
int pid(void);                                             // Get current process ID
int pkill(int pid, int signal);                            // Terminate another process
int ps(int* pids, int max_count);                          // List running processes
int errnum(void);                                          // Get process' last error code

// Memory Management
void* mmap_(os_size size, int flags);                      // Allocate memory region
int munmap_(void* addr, os_size size);                     // Free memory region
os_size meminfo(int pid);                                  // Get memory usage statistics

// Network I/O
int open_(const char* url);                                // Open connection to URL, returns handle
int read_(int handle, void* buf, os_size len);             // Read data from connection
int write_(int handle, const void* data, os_size len);     // Write data to connection
int listen_(const char* url);                              // Listen for connections on URL, returns handle
int accept_(int handle);                                   // Accept incoming connection, returns new handle
int select_(int* handles, int count, int timeout);         // Wait for events on multiple handles
int close_(int handle);                                    // Close connection
int getpeername_(int handle, char* addr, os_size addr_len); // Get remote endpoint info
int netinfo(struct netconn* conns, int max_count);         // List all active network connections

// Time Related
void sleep_(int ms);                                       // Sleep for specified milliseconds
long gettime(void);                                        // Get current system time

// System Control
void shutdown_(void);                                      // Graceful system shutdown
void reboot(void);                                         // System restart

#endif
