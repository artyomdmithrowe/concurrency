#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        std::cerr << "socket failed: " << strerror(errno) << "\n";
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(11000);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        std::cerr << "bad address\n";
        close(fd);
        return 1;
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "connect failed: " << strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    const char* message = "hello from client\n";

    write(fd, message, strlen(message));

    char buffer[1024];

    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "server replied: " << buffer;
    }

    close(fd);
}

