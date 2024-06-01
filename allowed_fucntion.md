## `execve`
- **Library**: `unistd.h`
- **Description**: Executes a program.
- **Arguments**:
	- `const char *pathname`: Path to the executable.
	- `char *const argv[]`: Argument list for the executable.
	- `char *const envp[]`: Environment variables for the executable.

## `dup`
- **Library**: `unistd.h`
- **Description**: Duplicates a file descriptor.
- **Arguments**:
	- `int oldfd`: The file descriptor to duplicate.

## `dup2`
- **Library**: `unistd.h`
- **Description**: Duplicates a file descriptor to a specific descriptor.
- **Arguments**:
	- `int oldfd`: The file descriptor to duplicate.
	- `int newfd`: The file descriptor to duplicate to.

## `pipe`
- **Library**: `unistd.h`
- **Description**: Creates a pair of file descriptors for inter-process communication.
- **Arguments**:
	- `int pipefd[2]`: Array to hold the two file descriptors.

## `strerror`
- **Library**: `string.h`
- **Description**: Returns a string describing the error code.
- **Arguments**:
	- `int errnum`: The error number.

## `gai_strerror`
- **Library**: `netdb.h`
- **Description**: Returns a string describing the `getaddrinfo()` error code.
- **Arguments**:
	- `int errcode`: The error code.

## `errno`
- **Library**: `errno.h`
- **Description**: Global variable indicating the error number of the last system call.

## `fork`
- **Library**: `unistd.h`
- **Description**: Creates a new process by duplicating the calling process.
- **Arguments**: None.

## `socketpair`
- **Library**: `sys/socket.h`
- **Description**: Creates a pair of connected sockets.
- **Arguments**:
	- `int domain`: Communication domain (e.g., `AF_UNIX`).
	- `int type`: Communication type (e.g., `SOCK_STREAM`).
	- `int protocol`: Protocol to be used (usually 0).
	- `int sv[2]`: Array to hold the two socket descriptors.

## `htons`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 16-bit number from host to network byte order.
- **Arguments**:
	- `uint16_t hostshort`: The number to convert.

## `htonl`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 32-bit number from host to network byte order.
- **Arguments**:
	- `uint32_t hostlong`: The number to convert.

## `ntohs`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 16-bit number from network to host byte order.
- **Arguments**:
	- `uint16_t netshort`: The number to convert.

## `ntohl`
- **Library**: `arpa/inet.h`
- **Description**: Converts a 32-bit number from network to host byte order.
- **Arguments**:
	- `uint32_t netlong`: The number to convert.

## `select`
- **Library**: `sys/select.h`
- **Description**: Monitors multiple file descriptors to see if any of them are ready for I/O.
- **Arguments**:
	- `int nfds`: Highest-numbered file descriptor + 1.
	- `fd_set *readfds`: Set of file descriptors to monitor for reading.
	- `fd_set *writefds`: Set of file descriptors to monitor for writing.
	- `fd_set *exceptfds`: Set of file descriptors to monitor for exceptions.
	- `struct timeval *timeout`: Timeout interval.

## `poll`
- **Library**: `poll.h`
- **Description**: Monitors multiple file descriptors to see if any of them are ready for I/O.
- **Arguments**:
	- `struct pollfd *fds`: Array of pollfd structures.
	- `nfds_t nfds`: Number of file descriptors.
	- `int timeout`: Timeout interval in milliseconds.

## `epoll_create`
- **Library**: `sys/epoll.h`
- **Description**: Creates an epoll instance.
- **Arguments**:
	- `int size`: Size hint for the number of file descriptors.

## `epoll_ctl`
- **Library**: `sys/epoll.h`
- **Description**: Controls the epoll instance.
- **Arguments**:
	- `int epfd`: Epoll file descriptor.
	- `int op`: Operation to perform (e.g., `EPOLL_CTL_ADD`).
	- `int fd`: Target file descriptor.
	- `struct epoll_event *event`: Event to associate with the file descriptor.

## `epoll_wait`
- **Library**: `sys/epoll.h`
- **Description**: Waits for events on an epoll file descriptor.
- **Arguments**:
	- `int epfd`: Epoll file descriptor.
	- `struct epoll_event *events`: Array to store the events.
	- `int maxevents`: Maximum number of events.
	- `int timeout`: Timeout interval in milliseconds.

## `kqueue`
- **Library**: `sys/event.h`
- **Description**: Creates a new kernel event queue.
- **Arguments**: None.

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

## `socket`
- **Library**: `sys/socket.h`
- **Description**: Creates an endpoint for communication and returns a file descriptor.
- **Arguments**:
	- `int domain`: Communication domain (e.g., `AF_INET`).
	- `int type`: Communication type (e.g., `SOCK_STREAM`).
	- `int protocol`: Protocol to be used (usually 0).

## `accept`
- **Library**: `sys/socket.h`
- **Description**: Accepts a connection on a socket.
- **Arguments**:
	- `int sockfd`: Listening socket file descriptor.
	- `struct sockaddr *addr`: Pointer to a structure where the address of the connecting socket will be stored.
	- `socklen_t *addrlen`: Size of the address structure.

## `listen`
- **Library**: `sys/socket.h`
- **Description**: Marks the socket as a passive socket to accept incoming connections.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `int backlog`: Number of connections allowed on the incoming queue.

## `send`
- **Library**: `sys/socket.h`
- **Description**: Sends data on a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `const void *buf`: Buffer containing the data.
	- `size_t len`: Length of the data.
	- `int flags`: Flags to modify the behavior of the function.

## `recv`
- **Library**: `sys/socket.h`
- **Description**: Receives data from a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `void *buf`: Buffer to store the received data.
	- `size_t len`: Length of the buffer.
	- `int flags`: Flags to modify the behavior of the function.

## `chdir`
- **Library**: `unistd.h`
- **Description**: Changes the current working directory.
- **Arguments**:
	- `const char *path`: Path to the new directory.

## `bind`
- **Library**: `sys/socket.h`
- **Description**: Assigns an address to a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `const struct sockaddr *addr`: Address to assign.
	- `socklen_t addrlen`: Size of the address structure.

## `connect`
- **Library**: `sys/socket.h`
- **Description**: Initiates a connection on a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `const struct sockaddr *addr`: Address to connect to.
	- `socklen_t addrlen`: Size of the address structure.

## `getaddrinfo`
- **Library**: `netdb.h`
- **Description**: Translates hostnames and service names to socket addresses.
- **Arguments**:
	- `const char *node`: Hostname or IP address.
	- `const char *service`: Service name or port number.
	- `const struct addrinfo *hints`: Hints for the type of socket the caller supports.
	- `struct addrinfo **res`: Pointer to a linked list of results.

## `freeaddrinfo`
- **Library**: `netdb.h`
- **Description**: Frees the memory allocated by `getaddrinfo`.
- **Arguments**:
	- `struct addrinfo *res`: Pointer to the linked list to free.

## `setsockopt`
- **Library**: `sys/socket.h`
- **Description**: Sets options on a socket.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `int level`: Level at which the option is defined.
	- `int optname`: Option name.
	- `const void *optval`: Pointer to the option value.
	- `socklen_t optlen`: Size of the option value.

## `getsockname`
- **Library**: `sys/socket.h`
- **Description**: Retrieves the current address to which the socket is bound.
- **Arguments**:
	- `int sockfd`: Socket file descriptor.
	- `struct sockaddr *addr`: Buffer to store the address.
	- `socklen_t *addrlen`: Size of the address buffer.

## `getprotobyname`
- **Library**: `netdb.h`
- **Description**: Retrieves protocol information corresponding to a protocol name.
- **Arguments**:
	- `const char *name`: Protocol name.

## `fcntl`
- **Library**: `fcntl.h`
- **Description**: Performs various operations on a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor.
	- `int cmd`: Command to perform (e.g., `F_SETFL`).
	- `...`: Additional arguments depending on the command.

## `close`
- **Library**: `unistd.h`
- **Description**: Closes a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor to close.

## `read`
- **Library**: `unistd.h`
- **Description**: Reads data from a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor to read from.
	- `void *buf`: Buffer to store the data.
	- `size_t count`: Number of bytes to read.

## `write`
- **Library**: `unistd.h`
- **Description**: Writes data to a file descriptor.
- **Arguments**:
	- `int fd`: File descriptor to write to.
	- `const void *buf`: Buffer containing the data.
	- `size_t count`: Number of bytes to write.

## `waitpid`
- **Library**: `sys/wait.h`
- **Description**: Waits for a specific child process to change state.
- **Arguments**:
	- `pid_t pid`: Process ID to wait for.
	- `int *status`: Pointer to store the exit status.
	- `int options`: Options to modify the behavior of the function.

## `kill`
- **Library**: `signal.h`
- **Description**: Sends a signal to a process or a group of processes.
- **Arguments**:
	- `pid_t pid`: Process ID or process group ID.
	- `int sig`: Signal to send.

## `signal`
- **Library**: `signal.h`
- **Description**: Sets a signal handler for a specific signal.
- **Arguments**:
	- `int signum`: Signal number.
	- `void (*handler)(int)`: Signal handler function.

## `access`
- **Library**: `unistd.h`
- **Description**: Checks a user's permissions for a file.
- **Arguments**:
	- `const char *pathname`: Path to the file.
	- `int mode`: Mode to check (e.g., `R_OK` for read permission).

## `stat`
- **Library**: `sys/stat.h`
- **Description**: Retrieves information about a file.
- **Arguments**:
	- `const char *pathname`: Path to the file.
	- `struct stat *statbuf`: Buffer to store the file information.

## `open`
- **Library**: `fcntl.h`
- **Description**: Opens a file and returns a file descriptor.
- **Arguments**:
	- `const char *pathname`: Path to the file.
	- `int flags`: Flags to modify the behavior of the function.
	- `...`: Additional arguments depending on the flags.

## `opendir`
- **Library**: `dirent.h`
- **Description**: Opens a directory stream.
- **Arguments**:
	- `const char *name`: Path to the directory.

## `readdir`
- **Library**: `dirent.h`
- **Description**: Reads a directory entry from a directory stream.
- **Arguments**:
	- `DIR *dirp`: Directory stream.

## `closedir`
- **Library**: `dirent.h`
- **Description**: Closes a directory stream.
- **Arguments**:
	- `DIR *dirp`: Directory stream.