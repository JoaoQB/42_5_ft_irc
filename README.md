# Useful info on new concepts:

## Sockaddr

`sockaddr` is a **generic** socket address structure used in socket APIs.

Functions like `bind()`, `connect()`, and `accept()` expect a pointer of type `struct sockaddr*`.

Therefore, when working with IPv4 (`sockaddr_in`), you need to **cast** it to the generic form:

```cpp
(struct sockaddr *)&addr
```

This casting allows the socket APIs to use a **common interface**, regardless of whether you're using:

  * IPv4 (`sockaddr_in`)
  * IPv6 (`sockaddr_in6`)
  * Unix domain sockets (`sockaddr_un`)

## Sockaddr_in

`sockaddr_in` is a struct object that holds important information about an **IPv4 socket address**, including:

  * IP address
  * Port number
  * Address family (`AF_INET`)

It is a specialized member of the `sockaddr` family, specifically designed for **IPv4 communication**.

```cpp
struct sockaddr_in {
	sa_family_t		sin_family;
	in_port_t		sin_port;
	struct in_addr	sin_addr;
	char			sin_zero[8];
}
```

```cpp
struct in_addr {
	in_addr_t	s_addr;
}
```

`sin_family`:
An integer representing the address family. For IPv4 this is typically set to `AF_INET`.

`sin_port`:
A 16-bit integer representing the port number.
This value is stored in network byte order (big-endian).
Therefore we use `htons()` function to convert a 16-bit unsigned short integer from host byte order to network byte order.

`sin_addr`:
A structure `in_addr` containing the **IPv4** address. This struct typically has a single member, representing the IPv45 address in network byte order.
`INADDR_ANY` represents "any" IP address, meaning the socket will be bound to all available network interfaces on the host (localhost, Ethernet, Wi-Fi, etc).

`sin_zero`:
This field is byte padding to make the structure the same size as `struct sockaddr`, for compatibility reasons. Itś tipically unused and filled with zeros.

## Pollfd

`pollfd` is a structure used for monitoring file descriptors for I/O events.
It's commonly employed with the `poll()` system call to perform multiplexed I/O, allowing a program to efficiently wait for events on multiple fds simultaneously without having to resort to blocking I/O operations.

```cpp
struct pollfd {
 int     fd;		// file descriptor
 short   events;	// requested events
 short   revents;	// returned events
};
```

`fd`:
The file descriptor to be monitored.

`events`:
A bitmask specifieds the events to monitor. Common events like `POLLIN` or `POLLHUP`.

`revents`:
A bitmask indicating which events that occurred for the given fd. This member is tipically filled in by the `poll()` function upon return and indicates the events that triggered the poll.

### I/O events

Refer to notifications from the operating system that a file descriptor is ready to perform a specific I/O operation without blocking.

In the context of **networking programming** and system calls like `poll()`, `select()` or `epoll()` these events are conditions that trigger when:

`POLLIN`: Data is available to read.
`POLLOUT`: Socket is ready for writing.
`POLLERR`: An error occurred on the fd.
`POLLHUP`: The other side hung up (disconnected).
`POLLNVAL`: The fd is invalid or not open.

In non-blocking, event-driven servers like our **IRC Server** we:
1. Use `poll()` or `select()` to wait for I/O events across multiple clients.
2. Only act on fds that are ready.
3. Avoid blocking reads/writes.

---

**EXAMPLE**

```cpp
struct pollfd pfd;
pfd.fd = clientFD;
pfd.events = POLLIN | POLLOUT;
```

Then `poll()` tells us if:
  * The client has sent data (`POLLIN`)
  * We can write to the client (`POLLOUT`)
  * Something went wrong (`POLLERR`, `POLLHUP`)

---

### What a is non-blocking event-driven server?

It is a server architecture designed to handle many simultaneous I/O operations efficiently without getting stuck waiting (blocking) on any single operation.

A **non-blocking event-driven server** is a server architecture designed to handle many simultaneous I/O operations efficiently without getting stuck waiting (blocking) on any single operation.

#### 1. **Blocking vs Non-blocking**

* **Blocking I/O** means when you perform a read/write on a socket or file descriptor, the call waits (blocks) until data is ready or the operation completes.

  * Example: `read()` blocks until data arrives.
  * Drawback: Server can get stuck waiting on one client, unable to serve others.

* **Non-blocking I/O** means read/write calls return immediately:

  * If no data is available, `read()` returns with an error like `EAGAIN` or `EWOULDBLOCK`.
  * Server keeps running and checks again later.
  * Allows server to serve multiple clients without waiting on any one.

---

#### 2. **Event-driven**

The server uses an **event notification mechanism** (`poll()`, `select()`, `epoll()`, `kqueue()`) to:

  * Monitor many file descriptors at once.
  * Be **notified when a socket is ready** for reading or writing.
  * Instead of continuously polling or waiting blindly, the server **reacts to events** (incoming data, ready to write, connection closed).


## Socket creation

```cpp
void Server::serverSocketCreate()
```

Let's look at the `socket()` function:

```cpp
int socket(int domain, int type, int protocol);
```

This function is a system call used to create a new socket of a specific type (such as stream or datagram) and returns a file descriptor that can be used to refer to that socket in subsequent system calls.

`domain`:
Specifies the comunication domain or address family.

`type`:
Specifies the type of communication semantics for the socket.
Common valus include `SOCK_STREAM` for **TCP** sockets and `SOCK_DGRAM` for **UDP** sockets.

`protocol`:
Specifies the specific protocol to be used with the socket.
For most socket types this argument is set to 0, indicating the system should choose an appropriate protocol based on the specified domain and type.

---

Now that we have our socket fd we should **bind it with the address**.
But before we need to set some important options.

We start by seting the `SO_REUSEADDR` option for the socket:

```cpp
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

This function is used to set options on a socket.
It allows us to configure various socket-level options to control the behavior of the socket. In this case it is being used to set `SO_REUSEADDR` option.

`level`:
Used to indicate the protocol level at which the option resides.
For socket-level options, setting the `level`to `SOL_SOCKET`tells the function that the option being set is a socket-level option and should be applied to the socket itself.
Other values correspond to specific protocol families such as `IPPROTO_TCP` for **TCP-specific** options or `IPPROTO_IP` for **IP-specific** options.

`optname`:
Sets the `SO_REUSEADDR` option allowing for the immediate reuse of local addresses and ports. This is useful when a server needs to bind to the same address and port it was previously using, without waiting for the default `TIME_WAIT` state to expire.
In **TCP** when a server stops running the port and address are typically reserved for a duration called the `TIME_WAIT` state, which lasts for twice the **Maximum Segment Lifetime** (`2MSL`) - often 1 MSL = 2 minutes.
During this time delayed packets related to the previous connection are managed.
However using `SO_REUSEADDR` enables the socket to bypass this reservation period and reuse the port and address right away.

`optval`:
This parameter is a pointer to the value that needs to be set for the option.
In our case it refers to a pointer to the `en` variable.

`optlen`:
Represents the size of the option value in bytes.
Setting the `en` value to 1 indicates the option is enabled.

---

We then use the `fcntl()` function:

```cpp
int fcntl(int fd, int cmd, ... /* arg */ );
```

This function performs various control operation on the file descriptor.
In our code it is being used to set the `O_NONBLOCK` flag on the server socket file descriptor.

`fd`:
The file descriptor on which to operate.

`cmd`:
The operation to perform.
In our case it's `F_SETFL`, indicating that we want to set the file status flags.

By seting the `O_NONBLOCK` on the server socket fd, this flag sets our socket to **non-blocking** mode, which means that operations such as `read()` or `write()` return immediately, even if there is no data available to read or the write operation cannot be completed immediately.
This provides a flexible and efficient mechanism for handling I/O operations asynchronously without blocking the program’s execution.

---

**EXAMPLE**

Let's imagine we are using **blocking operations**. We have a server running and a client is connected to it using the **Netcat(nc)** tool.
Netcat is often is used to manually send and recieve data over network sockets.

If we type some characters into the netcat terminal on the client side but don't press Enter(send), this characters are buffered locally on the client but not sent to the server.
Meanwhile the server is performing a blocking read operation on that client's socket.
Because no data has actually arrived, the server's read() call blocks.
If we try to shut down the server, the server's thread or process is blocked inside the read call, waiting indefinitely for the client to sent. It cant's proceed to clean up resources or close sockets until the operation completes or times out.

---

After setting the `SO_REUSEADDR` option and the `O_NONBLOCK` flag we can finally bind our socket fd to our socket address.
We have our socket fd returned by the `socket()` function and our `sockaddr_in` structure defining the network IP, port and family that socket is associated with.

---
**EXAMPLE**

```cpp
struct sockaddr_in address;
struct pollfd newPoll;

// Set the address family to ipv4
address.sin_family = AF_INET;
// Convert the port to network byte order (big endian)
address.sin_port = htons(this->serverPort);
// Set the address to any local machine address
address.sin_addr.s_addr = INADDR_ANY;

// Create the server socket
this->serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
if (this->serverSocketFd == -1) {
	throw std::runtime_error("failed to create socket");
}
```

---

We are ready to use `bind()`:

```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

When creating a socket using the `socket()` function, it's just a communication endpoint without any specific address or port associated.

The `bind()` function allows us to assign a specific address and port to a socket, giving it a unique identity on the network. By associating a socket with an address and port, we enable other processes or clients to communicate with it over the network.

The address can be an IP address (IPv4 or IPv6) or a hostname, while the port is a numerical identifier that specifies a particular service or application running on the host.

We can bind our socket to any port within the valid range. However, if we bind to port 0, the operating system automatically selects an available port from the dynamic/private port range (49152 to 65535). This is useful when the exact port number is not important and we want the system to assign one for us.

The `bind()` function takes three parameters:

* The socket file descriptor (`sockfd`),
* A pointer to a `struct sockaddr` containing the address information (`addr`),
* The size of the address structure (`addrlen`).

We might wonder why the `addr` parameter uses the generic `struct sockaddr` type rather than a specific address structure. This is because sockets can use different address families, such as IPv4 or IPv6. Instead of having separate `bind()` functions for each address family, the generic `struct sockaddr` acts as a common interface that can represent multiple types of socket addresses.

Typically, we create an address structure specific to the address family—like `struct sockaddr_in` for IPv4 or `struct sockaddr_in6` for IPv6—and then cast its pointer to `(struct sockaddr *)` when calling `bind()`. This allows the system to interpret the address correctly while letting us access the family-specific fields (like IP and port) through the appropriate structure.

To accept incoming connections, we first need to make the socket passive. But before diving into that, let's clarify the difference between passive and active sockets in the context of network communication.

### Passive vs Active sockets

