#pragma once

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <exception>

class Serial {

private:
    int fd; // File descriptor of the serial port.

public:
    /**
     * Constructor for the Serial class.
     */
    Serial();

    /**
     * Opens the serial port with the given name.
     * @param port_name Name of the serial port.
     */
    void open_serial_port(const char* port_name);

    /**
     * Configures the serial port with the given file descriptor and speed.
     * @param speed Baud rate for the serial communication.
     * @return True if configuration is successful, false otherwise.
     */
    bool configure_serial_port(int speed);

    /**
     * Reads data from the serial port.
     * @param buffer Buffer to store the read data.
     * @param size Number of bytes to read.
     * @return Number of bytes read, or -1 on error.
     */
    int read_from_serial_port(char* buffer, size_t size);

    /**
     * Writes data to the serial port.
     * @param buffer Buffer containing the data to write.
     * @param size Number of bytes to write.
     * @return Number of bytes written, or -1 on error.
     */
    int write_to_serial_port(const char* buffer, size_t size);

    /**
     * Closes the serial port.
     */
    void close_serial_port();
};