#include <iostream>

#include "serial.h"

int main() {
    auto ser = Serial();

    // open port
    const char* port_name = "/dev/ttyACM0";
    ser.open_serial_port(port_name);

    // configure port
    if (!ser.configure_serial_port(B115200)) {
        ser.close_serial_port();
        return 1;
    }

    // write to port
    const char* message = "READINFO";
    if (ser.write_to_serial_port(message, strlen(message)) < 0) {
        std::cerr << "Error writing to serial port: " << std::strerror(errno) << std::endl;
    }

    // read from port
    char buffer[100];
    int n = ser.read_from_serial_port(buffer, 8);
    if (n < 0) {
        std::cerr << "Error reading from serial port: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "Read from serial port: " << std::string(buffer, n) << std::endl;
        std::cout << n << std::endl;
    }

    n = ser.read_from_serial_port(buffer, 16);
    if (n < 0) {
        std::cerr << "Error reading from serial port: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "Read from serial port: " << std::string(buffer, n) << std::endl;
        std::cout << n << std::endl;
    }

    ser.close_serial_port();
    return 0;
}
