\
    // MultiClientServer.cpp — simple chat server (thread per client, broadcast)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #include <atomic>
    #include <cstring>
    #include <iostream>
    #include <mutex>
    #include <string>
    #include <thread>
    #include <unordered_map>
    #include <vector>

    struct ClientInfo {
        int fd;
        std::string name;
    };

    std::mutex g_mutex;
    std::unordered_map<int, ClientInfo> g_clients;
    std::atomic<bool> g_running{true};

    void broadcast(int from_fd, const std::string& msg) {
        std::lock_guard<std::mutex> lk(g_mutex);
        for (auto& [fd, ci] : g_clients) {
            if (fd == from_fd) continue;
            std::string wire = msg;
            send(fd, wire.c_str(), wire.size(), 0);
        }
    }

    void handle_client(int cfd) {
        char buf[4096];
        // First line is the client's name (optional). Default to fd string.
        std::string name = "user" + std::to_string(cfd);
        {
            // Try to read one line for name (non-blocking style)
            ssize_t n = recv(cfd, buf, sizeof(buf)-1, 0);
            if (n > 0) {
                buf[n] = '\0';
                std::string line(buf);
                auto pos = line.find('\n');
                if (pos != std::string::npos) line = line.substr(0, pos);
                if (!line.empty()) name = line;
            } else if (n == 0) { close(cfd); return; }
        }
        {
            std::lock_guard<std::mutex> lk(g_mutex);
            g_clients[cfd] = ClientInfo{cfd, name};
        }
        std::string joinMsg = name + " joined.\n";
        broadcast(cfd, joinMsg);

        while (g_running.load()) {
            ssize_t n = recv(cfd, buf, sizeof(buf)-1, 0);
            if (n <= 0) break;
            buf[n] = '\0';
            std::string line(buf);
            if (line == "Over\n" || line == "Over\r\n") break;
            std::string out = name + ": " + line;
            broadcast(cfd, out);
        }
        {
            std::lock_guard<std::mutex> lk(g_mutex);
            auto it = g_clients.find(cfd);
            std::string nm = (it != g_clients.end() ? it->second.name : name);
            g_clients.erase(cfd);
            std::string leave = nm + " left.\n";
            broadcast(cfd, leave);
        }
        close(cfd);
    }

    int main(int argc, char* argv[]) {
        int port = 4000;
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
        if (listen(server_fd, 16) < 0) {
            perror("listen"); close(server_fd); return 1;
        }
        std::cout << "Multi-client chat server listening on " << port << "...\n";

        std::vector<std::thread> threads;
        while (g_running.load()) {
            sockaddr_in caddr{};
            socklen_t clen = sizeof(caddr);
            int cfd = accept(server_fd, (sockaddr*)&caddr, &clen);
            if (cfd < 0) { perror("accept"); continue; }
            threads.emplace_back(handle_client, cfd);
            threads.back().detach(); // detach to avoid accumulating
        }

        close(server_fd);
        return 0;
    }
