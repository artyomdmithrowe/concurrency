#include <arpa/inet.h>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <iostream>

void print_u16(const char *name, uint16_t value) {
  std::cout << name << "\n";
  std::cout << "  decimal: " << value << "\n";
  std::cout << "  hex:     0x" << std::hex << std::setw(4) << std::setfill('0')
            << value << std::dec << std::setfill(' ') << "\n";
  std::cout << "  bits:    " << std::bitset<16>(value) << "\n";
}

int main() {
  uint16_t host_port = 8080;
  uint16_t network_port = htons(host_port);
  uint16_t restored_port = ntohs(network_port);

  print_u16("host value", host_port);
  print_u16("htons(host value)", network_port);
  print_u16("ntohs(network value)", restored_port);

  std::cout << "\n";
  std::cout << "Use in sockaddr_in:\n";
  std::cout << "  addr.sin_port = htons(" << host_port << ");\n";
  std::cout << "  port = ntohs(addr.sin_port);\n";
}
