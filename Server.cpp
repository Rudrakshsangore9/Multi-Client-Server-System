\
    // Server.cpp — single‑client echo server (blocking)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #include <cstring>
    #include <iostream>
    #include <string>

    int main(int argc, char* argv[]) {
        int port = 3999;
        if (argc >= 2) port = std::stoi(argv[1]);

        int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) { perror("socket"); return 1; }

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind"); close(server_fd); return 1;
        }
        if (listen(server_fd, 1) < 0) {
            perror("listen"); close(server_fd); return 1;
        }
        std::cout << "Server listening on port " << port << "...\n";

        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &len);
        if (client_fd < 0) { perror("accept"); close(server_fd); return 1; }
        std::cout << "Client connected.\n";

        constexpr size_t BUF_SZ = 4096;
        char buf[BUF_SZ];
        while (true) {
            ssize_t n = recv(client_fd, buf, BUF_SZ - 1, 0);
            if (n <= 0) break;
            buf[n] = '\0';
            std::string line(buf);
            // Basic protocol: if client sends "Over\n", close.
            if (line == "Over\n" || line == "Over\r\n") {
                std::string bye = "Goodbye!\n";
                send(client_fd, bye.c_str(), bye.size(), 0);
                break;
            }
            // Echo back
            send(client_fd, line.c_str(), line.size(), 0);
        }

        close(client_fd);
        close(server_fd);
        std::cout << "Server shut down.\n";
        return 0;
    }
