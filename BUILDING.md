# Building & Running

## Requirements
- A C++17 compiler (g++/clang++)
- POSIX sockets (Linux/macOS). For Windows, use WSL/MSYS2 or adapt accordingly.

## Build everything
```sh
make
```

## Run single‑client echo server
```sh
./server 3999
```

## Run client (connect to server)
```sh
./client 127.0.0.1 3999
# type lines and press Enter; Ctrl+D (Unix) to end
```

## Run multi‑client chat server
```sh
./multiserver 4000
```
Then run multiple clients (the above `client` works for testing).

## Threading example
```sh
./threads
```
