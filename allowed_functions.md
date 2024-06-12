## `execve`
- **Library**: `unistd.h`
- **Description**: Executes a program.
- **Arguments**:
	- `const char *pathname`: Path to the executable.
	- `char *const argv[]`: Argument list for the executable.
	- `char *const envp[]`: Environment variables for the executable.
- **Return**:
	- `On success, execve() does not return, on error -1 is returned, and errno is set.`

## `dup`
- **Library**: `unistd.h`
- **Description**: Duplicates a file descriptor.
- **Arguments**:
	- `int oldfd`: The file descriptor to duplicate.
- **Return**:
	- `On success, this system call returns the new file descriptor.  On error, -1 is returned, and errno is set.`

## `dup2`
- **Library**: `unistd.h`
- **Description**: Duplicates a file descriptor to a specific descriptor.
- **Arguments**:
	- `int oldfd`: The file descriptor to duplicate.
	- `int newfd`: The file descriptor to duplicate to.
- **Return**:
	- `On success, this system call returns the new file descriptor.  On error, -1 is returned, and errno is set.`

## `pipe`
- **Library**: `unistd.h`
- **Description**: Creates a pair of file descriptors for inter-process communication.
- **Arguments**:
	- `int pipefd[2]`: Array to hold the two file descriptors.
- **Return**:
	- `On success, zero is returned.  On error, -1 is returned, errno is set, and pipefd is left unchanged.`

## `strerror`
- **Library**: `string.h`
- **Description**: Returns a string describing the error code.
- **Arguments**:
	- `int errnum`: The error number.
- **Return**:
	- `The appropriate error description string, or an "Unknown error nnn" message if the error number is unknown.`

## `gai_strerror`
- **Library**: `netdb.h`
- **Description**: Returns a string describing the `getaddrinfo()` error code.
- **Arguments**:
	- `int errcode`: The error code.
- **Return**:
	- `Translation of getaddrinfo() error codes to a human readable string, suitable for error reporting.`

## `errno`
- **Library**: `errno.h`
- **Description**: Global variable indicating the error number of the last system call.

## `fork`
- **Library**: `unistd.h`
- **Description**: Creates a new process by duplicating the calling process.
- **Arguments**: None.
- **Return**:
	- `On success, the PID of the child process is returned in the parent, and 0 is returned in the child. On failure, -1 is returned in the parent, no child process is created, and errno is set.`

## `socketpair`
- **Library**: `sys/socket.h`
- **Description**: Creates a pair of connected sockets.
- **Arguments**:
	- `int domain`: Communication domain (e.g., `AF_UNIX`).
	- `int type`: Communication type (e.g., `SOCK_STREAM`).
	- `int protocol`: Protocol to be used (usually 0).
	- `int sv[2]`: Array to hold the two socket descriptors.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, errno is set, and sv is left unchanged`

## `htons`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 16-bit number from host to network byte order.
- **Arguments**:
	- `uint16_t hostshort`: The number to convert.
- **Return**:
	- `uint16_t`: The converted number

## `htonl`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 32-bit number from host to network byte order.
- **Arguments**:
	- `uint32_t hostlong`: The number to convert.
- **Return**:
	- `uint32_t`: The converted number

## `ntohs`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 16-bit number from network to host byte order.
- **Arguments**:
	- `uint16_t netshort`: The number to convert.
- **Return**:
	- `uint16_t`: The converted number

## `ntohl`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 32-bit number from network to host byte order.
- **Arguments**:
	- `uint32_t netlong`: The number to convert.
- **Return**:
	- `uint32_t`: The converted number

## `select`
- **Library**: `sys/select.h`
- **Description**: Monitors multiple file descriptors to see if any of them are ready for I/O.
- **Arguments**:
	- `int nfds`: Highest-numbered file descriptor + 1.
	- `fd_set *readfds`: Set of file descriptors to monitor for reading.
	- `fd_set *writefds`: Set of file descriptors to monitor for writing.
	- `fd_set *exceptfds`: Set of file descriptors to monitor for exceptions.
	- `struct timeval *timeout`: Timeout interval.
- **Return**:
	- `On success the number of file descriptors contained in the three returned descriptor sets (that is, the total number of bits that are set in readfds, writefds, exceptfds).  The return value may be zero if the timeout expired before any file descriptors became ready.`

## `poll`
- **Library**: `poll.h`
- **Description**: Monitors multiple file descriptors to see if any of them are ready for I/O.
- **Arguments**:
	- `struct pollfd *fds`: Array of pollfd structures.
	- `nfds_t nfds`: Number of file descriptors.
	- `int timeout`: Timeout interval in milliseconds.
- **Return**:
	- `On success a nonnegative value which is the number of elements in the pollfds whose revents fields have been set to a nonzero value (indicating an event or an error). A return value of zero indicates that the system call timed out before any file descriptors became read.`

## `epoll_create`
- **Library**: `sys/epoll.h`
- **Description**: Creates an epoll instance.
- **Arguments**:
	- `int size`: Size hint for the number of file descriptors.
- **Return**:
	- `On success, a file descriptor (a nonnegative integer). On error, -1 is returned, and errno is set`

## `epoll_ctl`
- **Library**: `sys/epoll.h`
- **Description**: Controls the epoll instance.
- **Arguments**:
	- `int epfd`: Epoll file descriptor.
	- `int op`: Operation to perform (e.g., `EPOLL_CTL_ADD`).
	- `int fd`: Target file descriptor.
	- `struct epoll_event *event`: Event to associate with the file descriptor.
- **Return**:
	- `On success zero. On error -1 and errno is set.`

## `epoll_wait`
- **Library**: `sys/epoll.h`
- **Description**: Waits for events on an epoll file descriptor.
- **Arguments**:
	- `int epfd`: Epoll file descriptor.
	- `struct epoll_event *events`: Array to store the events.
	- `int maxevents`: Maximum number of events.
	- `int timeout`: Timeout interval in milliseconds.
- **Return**:
	- `On success the number of file descriptors ready for the requested I/O, or zero if no file descriptor became ready during the requested timeout milliseconds.  On error -1 and errno is set.`

## `kqueue`
- **Library**: `sys/event.h`
- **Description**: Creates a new kernel event queue.
- **Arguments**: None.
- **Return**:
	- `A file descriptor. If there was an error creating the kernel event queue, a	value of -1 is returned	and errno is set.`

## `kevent`
- **Library**: `sys/event.h`
- **Description**: Registers events with the kernel event queue.
- **Arguments**:
	- `int kq`: Kernel event queue file descriptor.
	- `const struct kevent *changelist`: Array of events to register.
	- `int nchanges`: Number of events to register.
	- `struct kevent *eventlist`: Array to store the triggered events.
	- `int nevents`: Number of events to retrieve.
	- `const struct timespec *timeout`: Timeout interval.
- **Return**:
	- `The number of events placed in the eventlist, up to the value given by nevents. If an error occurs while processing an element of the changelist and there is enough room in the eventlist, then the event will be placed in the eventlist with EV_ERROR set in flags and the system error in data. Otherwise, -1 will be returned, and errno will be set. If the time limit expires, then kevent() returns 0.`

## `socket`
- **Library**: `sys/socket.h`
- **Description**: Creates an endpoint for communication and returns a file descriptor.
- **Arguments**:
	- `int domain`: Communication domain (e.g., `AF_INET`).
	- `int type`: Communication type (e.g., `SOCK_STREAM`).
	- `int protocol`: Protocol to be used (usually 0).
- **Return**:
	- `On success, a file descriptor for the new socket is returned. On error, -1 is returned, and errno is set.`

## `accept`
- **Library**: `sys/socket.h`
- **Description**: Accepts a connection on a socket.
- **Arguments**:
	- `int sockfd`: Listening socket file descriptor.
	- `struct sockaddr *addr`: Pointer to a structure where the address of the connecting socket will be stored.
	- `socklen_t *addrlen`: Size of the address structure.
- **Return**:
	- `On success, a file descriptor for the accepted socket (a nonnegative integer). On error, -1 is returned, errno is set, and addrlen is left unchanged.`

## `listen`
- **Library**: `sys/socket.h`
- **Description**: Marks the socket as a passive socket to accept incoming connections.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `int backlog`: Number of connections allowed on the incoming queue.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set.`

## `send`
- **Library**: `sys/socket.h`
- **Description**: Sends data on a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `const void *buf`: Buffer containing the data.
	- `size_t len`: Length of the data.
	- `int flags`: Flags to modify the behavior of the function.
- **Return**:
	- `On success, the number of bytes sent. On error, -1 is returned, and errno is set.`

## `recv`
- **Library**: `sys/socket.h`
- **Description**: Receives data from a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `void *buf`: Buffer to store the received data.
	- `size_t len`: Length of the buffer.
	- `int flags`: Flags to modify the behavior of the function.
- **Return**:
	- `On success, the number of bytes recieved. On error, -1 is returned, and errno is set. When a stream socket peer has performed an orderly shutdown, the return value will be 0 (the traditional "end-of-file" return). Datagram sockets in various domains (e.g., the UNIX and Internet domains) permit zero-length datagrams.  When such a datagram is received, the return value is 0. The value 0 may also be returned if the requested number of bytes to receive from a stream socket was 0.`

## `chdir`
- **Library**: `unistd.h`
- **Description**: Changes the current working directory.
- **Arguments**:
	- `const char *path`: Path to the new directory.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set.`

## `bind`
- **Library**: `sys/socket.h`
- **Description**: Assigns an address to a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `const struct sockaddr *addr`: Address to assign.
	- `socklen_t addrlen`: Size of the address structure.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set`.

## `connect`
- **Library**: `sys/socket.h`
- **Description**: Initiates a connection on a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `const struct sockaddr *addr`: Address to connect to.
	- `socklen_t addrlen`: Size of the address structure.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set`.

## `getaddrinfo`
- **Library**: `netdb.h`
- **Description**: Translates hostnames and service names to socket addresses.
- **Arguments**:
	- `const char *node`: Hostname or IP address.
	- `const char *service`: Service name or port number.
	- `const struct addrinfo *hints`: Hints for the type of socket the caller supports.
	- `struct addrinfo **res`: Pointer to a linked list of results.
- **Return**:
	- `On success, zero is returned. On error, an error code is returned that can be tranlated into readable using gai_strerror()`.

## `freeaddrinfo`
- **Library**: `netdb.h`
- **Description**: Frees the memory allocated by `getaddrinfo`.
- **Arguments**:
	- `struct addrinfo *res`: Pointer to the linked list to free.
- **Return**:
	- `void`.

## `setsockopt`
- **Library**: `sys/socket.h`
- **Description**: Sets options on a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `int level`: Level at which the option is defined.
	- `int optname`: Option name.
	- `const void *optval`: Pointer to the option value.
	- `socklen_t optlen`: Size of the option value.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set. Netfilter allows the programmer to define custom socket options with associated handlers, for such options, the return value on success is the value returned by the handler.`.

## `getsockname`
- **Library**: `sys/socket.h`
- **Description**: Retrieves the current address to which the socket is bound.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `struct sockaddr *addr`: Buffer to store the address.
	- `socklen_t *addrlen`: Size of the address buffer.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set`.

## `getprotobyname`
- **Library**: `netdb.h`
- **Description**: Retrieves protocol information corresponding to a protocol name.
- **Arguments**:
	- `const char *name`: Protocol name.
- **Return**:
	- `A pointer to a statically allocated protoent structure, or a null pointer if an error occurs or the end of the file is reached.`.

## `fcntl`
- **Library**: `fcntl.h`
- **Description**: Performs various operations on a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor.
	- `int cmd`: Command to perform (e.g., `F_SETFL`).
	- `...`: Additional arguments depending on the command.
- **Return**:
	- `For a successful call, the return value depends on the operation: F_DUPFD The new file descriptor; F_GETFD Value of file descriptor flags; F_GETFL Value of file status flags; F_GETLEASE Type of lease held on file descriptor; F_GETOWN Value of file descriptor owner; F_GETSIG Value of signal sent when read or write becomes possible; or zero for traditional SIGIO behavior; F_GETPIPE_SZ; F_SETPIPE_SZ The pipe capacity; F_GET_SEALS A bit mask identifying the seals that have been set for the inode referred to by fd; All other commands Zero. On error, -1 is returned, and errno is set.`.

## `close`
- **Library**: `unistd.h`
- **Description**: Closes a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor to close.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set`.

## `read`
- **Library**: `unistd.h`
- **Description**: Reads data from a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor to read from.
	- `void *buf`: Buffer to store the data.
	- `size_t count`: Number of bytes to read.
- **Return**:
	- `On success, a non-negative integer indicating the number of bytes actually read. Otherwise, the functions shall return -1 and errno is set`

## `write`
- **Library**: `unistd.h`
- **Description**: Writes data to a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor to write to.
	- `const void *buf`: Buffer containing the data.
	- `size_t count`: Number of bytes to write.
- **Return**:
	- `On success, a non-negative integer indicating the number of bytes actually written. Otherwise, the functions shall return -1 and errno is set.`

## `waitpid`
- **Library**: `sys/wait.h`
- **Description**: Waits for a specific child process to change state.
- **Arguments**:
	- `pid_t pid`: Process ID to wait for.
	- `int *status`: Pointer to store the exit status.
	- `int options`: Options to modify the behavior of the function.
- **Return**:
	- `on success, returns the process ID of the child whose state has changed; if WNOHANG was specified and one or more child(ren) specified by pid exist, but have not yet changed state, then 0 is returned. On error, -1 is returned.`

## `kill`
- **Library**: `signal.h`
- **Description**: Sends a signal to a process or a group of processes.
- **Arguments**:
	- `pid_t pid`: Process ID or process group ID.
	- `int sig`: Signal to send.
- **Return**:
	- `Upon successful completion, 0 shall be returned. Otherwise, -1 shall be returned and errno set.`

## `signal`
- **Library**: `signal.h`
- **Description**: Sets a signal handler for a specific signal.
- **Arguments**:
	- `int signum`: Signal number.
	- `void (*handler)(int)`: Signal handler function.
- **Return**:
	- `The previous value of the signal handler, or SIG_ERR on error. In the event of an error, errno is set`

## `access`
- **Library**: `unistd.h`
- **Description**: Checks a user's permissions for a file.
- **Arguments**:
	- `const char *pathname`: Path to the file.
	- `int mode`: Mode to check (e.g., `R_OK` for read permission).
- **Return**:
	- `Upon successful completion, these functions shall return 0. Otherwise, these functions shall return -1 and set errno.`

## `stat`
- **Library**: `sys/stat.h`
- **Description**: Retrieves information about a file.
- **Arguments**:
	- `const char *pathname`: Path to the file.
	- `struct stat *statbuf`: Buffer to store the file information.
- **Return**:
	- `On success, zero is returned. On error, -1 is returned, and errno is set.`

## `open`
- **Library**: `fcntl.h`
- **Description**: Opens a file and returns a file descriptor.
- **Arguments**:
	- `const char *pathname`: Path to the file.
	- `int flags`: Flags to modify the behavior of the function.
	- `...`: Additional arguments depending on the flags.
- **Return**:
	- `Upon successful completion, these functions shall open the file and return a non-negative integer representing the file descriptor.  Otherwise, these functions shall return -1 and set errno to indicate the error. If -1 is returned, no files shall be created or modified.`

## `opendir`
- **Library**: `dirent.h`
- **Description**: Opens a directory stream.
- **Arguments**:
	- `const char *name`: Path to the directory.
- **Return**:
	- `A pointer to the directory stream. On error, NULL is returned, and errno is set.`

## `readdir`
- **Library**: `dirent.h`
- **Description**: Reads a directory entry from a directory stream.
- **Arguments**:
	- `DIR *dirp`: Directory stream.
- **Return**:
	- `A pointer to a dirent structure. If the end of the directory stream is reached, NULL is returned and errno is not changed. If an error, NULL is returned and errno is set appropriately. To distinguish end of stream from an error, set errno to zero before calling readdir() and then check the value of errno if NULL is returned.`

## `closedir`
- **Library**: `dirent.h`
- **Description**: Closes a directory stream.
- **Arguments**:
	- `DIR *dirp`: Directory stream.
- **Return**:
	- `0 on success. On error, -1 is returned, and errno is set.`
