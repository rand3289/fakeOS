# fakeOS

The goal of this project is to find an API (a set of system calls) needed to implement a minimal modern server operating system for running services in a Service Oriented Architecture.  Programs will have a timer, memory management and network support but will not have things like local disk, user interface or device access.  Everything will be done through the network.  It will be impossible to modify any settings in the OS by running a program from the network.  All programs, program data and settings will be stored on network file systems.  Later shared access to GPU will need to be added. 

To a user the OS will be similar to SSH access.  Except instead of command names user will supply network URLs of the programs he wants to run.  The programs will be cached from the network to provide faster future load times.

fakeOS will run on top of an existing operating system. At this point the goal is to find the minimal interface needed.  Not to provide a working implementation of the OS.

Instead of threads and IPC, an optimized network stack provides a local exchange of buffers among processes.  Possibly by implementing a reliable UDP protocol.  This way programs can avoid copying data from a stream to a buffer as would occur with TCP. An ability to broadcast a UDP packet might be important. A program needs to be able to request a buffer from an OS (by requesting a page using the memory manger).

The main idea is to replace syscalls with fast datagram (one memory page) "reads/writes" to local services.  These service can also be transparently substituted for remote services.  For example a program can create a process by connecting (or sending a datagram) to fakeOS and requesting to run another program ( by supplying a URL).

TODO:

* pass URL and other parameters (PID, API key) to the process upon startup (via argv[]???)
* Should we change "char* url" parameter of link() to Page?
* What facilities should we add for crash handling?  Upload core/stack/parameter values/IP to URL of the program?

PROBLEMS:

* when creating a link, it's impossible to wait for a client or a server connection using select()

