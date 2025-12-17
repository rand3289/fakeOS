#ifndef INCLUDED_SYSCALLS_H
#define INCLUDED_SYSCALLS_H
// these are syscalls and data structures available in fakeOS to user processes

typedef unsigned long long size_t;
struct netconn { int pid; char addr[64]; };
struct memstat { size_t total; size_t used; };

// Process Management
void exit(int status);                                    // Terminate current process
int spawn(void* addr);                                    // Create new process with code at address
int wait(int pid);                                        // Wait for child process to terminate
int getpid(void);                                         // Get current process ID
int pkill(int pid, int signal);                            // Terminate another process
int ps(int* pids, int max_count);                         // List running processes
int errno(void);                                          // Get process' last error code

// Time Management
void sleep(int ms);                                       // Sleep for specified milliseconds
long gettime(void);                                       // Get current system time

// Memory Management
void* mmap(size_t size, int flags);                       // Allocate memory region
int munmap(void* addr, size_t size);                      // Free memory region
int meminfo(struct memstat* info);                        // Get memory usage statistics

// Network I/O
int open(const char* url);                                // Open connection to URL, returns handle
int read(int handle, void* buf, size_t len);              // Read data from connection
int write(int handle, const void* data, size_t len);      // Write data to connection
int listen(const char* url);                              // Listen for connections on URL, returns handle
int accept(int handle);                                   // Accept incoming connection, returns new handle
int select(int* handles, int count, int timeout);         // Wait for events on multiple handles
int getpeername(int handle, char* addr, size_t addr_len); // Get remote endpoint info
int close(int handle);                                    // Close connection
int netinfo(struct netconn* conns, int max_count);        // List all active network connections

// System Control
void shutdown(void);                                      // Graceful system shutdown
void reboot(void);                                        // System restart

#endif
