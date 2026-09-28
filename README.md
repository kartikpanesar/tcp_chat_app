## tcp_chat_app

A multi-client TCP chat application written in C. A threaded server accepts multiple clients and relays every message a client sends to all the other connected clients.

## Features

- Multi-client server: one POSIX thread per connected client
- Message broadcast: a message from one client is delivered to every other client
- Full-duplex client: separate threads for sending (stdin) and receiving (stdout), so you can type and receive at the same time

## How it works

```
 client A ──┐                     ┌── client B
            ├──►  server (5050)  ─┤
 client C ──┘   broadcast to all  └── client D
                 except sender
```

1. The server creates a TCP socket, binds to port `5050`, and listens.
2. A listener thread accepts incoming connections and spawns a detached handler thread for each client.
3. Each handler thread `recv()`s data from its client and calls `broadcast()`, which sends the data to every other client in the shared client table.
4. The client connects to the server, then runs a sending thread (stdin → socket) and a receiving thread (socket → stdout).


## Requirements

- Linux or another POSIX system
- `gcc` (or any C compiler with pthread support)

## Build

```bash
gcc server.c -o server -pthread
gcc client.c -o client -pthread
```

## Usage

**1. Start the server**

```bash
./server
```

```
Server socket created.
Binding the server is completed.
Listening on Port 5050
```

**2. Start one or more clients** (each in its own terminal)

```bash
./client
```

**3. Chat**

Type a message and press Enter. It shows up in every other client's terminal.


## Concepts used

- BSD sockets: `socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`
- Name resolution with `getaddrinfo`
- POSIX threads (`pthread_create`, `pthread_detach`, `pthread_join`)
- Mutex-based synchronization (`pthread_mutex_t`)

## Future Improvements

- [ ] Add usernames so each client is identified in the chat
- [ ] Client-to-client direct messaging (private chats)
- [ ] Graceful handling of client disconnects and server shutdown
- [ ] Message timestamps
- [ ] Basic commands such as `/list` and `/quit`

## Author

[kartikpanesar](https://github.com/kartikpanesar)


