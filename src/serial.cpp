#include "serial.h"

/**
 * Constructor for the Serial class.
 */
Serial::Serial() {
    this->fd = -1;
}

/**
 * Opens the serial port with the given name.
 * @param port_name Name of the serial port.
 */
void Serial::open_serial_port(const char* port_name) {
    this->fd = open(port_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (this->fd < 0) {
        std::cerr << "Error opening " << port_name << ": " << std::strerror(errno) << std::endl;
    }
}

/**
 * Configures the serial port with the given file descriptor and speed.
 * @param speed Baud rate for the serial communication.
 * @return True if configuration is successful, false otherwise.
 */
bool Serial::configure_serial_port(int speed) {
    struct termios tty;
    if (tcgetattr(this->fd, &tty) != 0) {
        std::cerr << "Error from tcgetattr: " << std::strerror(errno) << std::endl;
        return false;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
    tty.c_iflag &= ~IGNBRK;                     // disable break processing
    tty.c_lflag = 0;                            // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN] = 0;                         // read doesn't block
    tty.c_cc[VTIME] = 5;                        // 0.5 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);            // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);          // shut off parity
    tty.c_cflag &= ~CSTOPB;                     // one stop bit
    tty.c_cflag &= ~CRTSCTS;                    // no hardware flow control 

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr: " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

/**
 * Reads data from the serial port.
 * @param buffer Buffer to store the read data.
 * @param size Number of bytes to read.
 * @return Number of bytes read, or -1 on error.
 */
int Serial::read_from_serial_port(char* buffer, size_t size) {
    return read(this->fd, buffer, size);
}

/**
 * Writes data to the serial port.
 * @param buffer Buffer containing the data to write.
 * @param size Number of bytes to write.
 * @return Number of bytes written, or -1 on error.
 */
int Serial::write_to_serial_port(const char* buffer, size_t size) {
    return write(this->fd, buffer, size);
}

/**
 * Closes the serial port.
 */
void Serial::close_serial_port() { 
    close(this->fd); 
}