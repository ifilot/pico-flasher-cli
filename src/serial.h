#pragma once

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <exception>
#include <string>
#include <stdint.h>
#include <vector>

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
     * Reads the device information from the serial port.
     * @return Device information as a string.
     */
    std::string read_device_info();

    /**
     * Reads the device ID from the serial port.
     * @return Device ID as a 16-bit unsigned integer.
     */
    uint16_t get_device_id();

    /**
     * Writes data to sector on flash chip.
     * @param sector which sector to write to
     * @param data data to write to sector
     * @return Number of bytes written, or -1 on error.
     */
    uint16_t write_sector(uint16_t sector, const std::vector<uint8_t>& data);

    /**
     * Reads data from sector on flash chip.
     * @param sector which sector to read from
     * @param data data to read from sector
     */
    void read_bank(uint16_t bank, std::vector<uint8_t>& data);

    /**
     * Erases the chip.
     */
    uint16_t erase_chip();

    /**
     * Closes the serial port.
     */
    void close_serial_port();

private:
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
     * Reads data from the serial port.
     * @param cmd Command to send to the serial port
     */
    void send_command(const char* cmd);

};