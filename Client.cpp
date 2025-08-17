\
    // Client.cpp — connects to server, sends stdin lines, prints responses
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #include <cstring>
    #include <iostream>
    #include <string>

    int main(int argc, char* argv[]) {
        std::string host = "127.0.0.1";
        int port = 3999;
        if (argc >= 2) host = argv[1];
        if (argc >= 3) port = std::stoi(argv[2]);

        int sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) { perror("socket"); return 1; }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
            std::cerr << "Invalid address: " << host << "\n";
            return 1;
        }

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("connect");
            return 1;
        }
        std::cout << "Connected to " << host << ":" << port << "\n";
        std::cout << "Type lines to send. Type 'Over' to quit.\n";

        std::string line;
        char buf[4096];
        while (std::getline(std::cin, line)) {
            line.push_back('\n');
            if (send(sock, line.c_str(), line.size(), 0) < 0) { perror("send"); break; }
            ssize_t n = recv(sock, buf, sizeof(buf)-1, 0);
            if (n <= 0) { std::cout << "Server closed connection.\n"; break; }
            buf[n] = '\0';
            std::cout << "[server] " << buf;
            if (line == "Over\n") break;
        }
        close(sock);
        return 0;
    }
