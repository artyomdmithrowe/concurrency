#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

bool write_all(int fd, const char* data, size_t size) {
    size_t written = 0;

    while (written < size) {
        ssize_t res = write(fd, data + written, size - written);

        if (res < 0) {
            if (errno == EINTR) {
                continue;
            }

            return false;
        }

        if (res == 0) {
            return false;
        }

        written += static_cast<size_t>(res);
    }

    return true;
}

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

    std::string line;

    while (std::getline(std::cin, line)) {
        line.push_back('\n');

        if (!write_all(fd, line.data(), line.size())) {
            std::cerr << "write failed\n";
            break;
        }

        char buffer[1024];

        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }

            std::cerr << "read failed: " << strerror(errno) << "\n";
            break;
        }

        if (n == 0) {
            std::cout << "server closed connection\n";
            break;
        }

        buffer[n] = '\0';
        std::cout << "server: " << buffer;
    }

    close(fd);
}

