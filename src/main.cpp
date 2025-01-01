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

    std::string resp = ser.read_device_info();
    std::cout << resp << std::endl;

    ser.close_serial_port();
    return 0;
}
