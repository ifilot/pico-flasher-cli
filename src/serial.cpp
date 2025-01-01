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
    tty.c_iflag &= ~ICRNL;                      // disable CR-to-NL translation
    tty.c_lflag = 0;                            // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN] = 0;                         // read
    tty.c_cc[VTIME] = 5;                        // 0.5 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // no xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);            // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);          // no parity
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
 * Reads data from the serial port.
 * @param cmd Command to send to the serial port
 */
void Serial::send_command(const char* cmd) {
    // write to port
    if (this->write_to_serial_port(cmd, strlen(cmd)) < 0) {
        std::cerr << "Error writing to serial port: " << std::strerror(errno) << std::endl;
    }

    // read from port
    char buffer[16];
    int n = this->read_from_serial_port(buffer, 8);
    if (n < 0) {
        throw std::runtime_error(std::string("Error reading from serial port: ") + 
                                 std::string(std::strerror(errno)));
    } else {
        if(std::string(buffer, 8) != std::string(cmd,8)) {
            throw std::runtime_error("Error: Command not received correctly: " + std::string(buffer, 8));
        }
    }
}

std::string Serial::read_device_info() {
    // send command
    this->send_command("READINFO");

    char buffer[24];
    int n = this->read_from_serial_port(buffer, 16);
    if (n < 0) {
        throw std::runtime_error(std::string("Error reading from serial port: ") + 
                                 std::string(std::strerror(errno)));
    }

    return std::string(buffer, n);
}

/**
 * Reads the device ID from the serial port.
 * @return Device ID as a 16-bit unsigned integer.
 */
uint16_t Serial::get_device_id() {
    this->send_command("DEVIDSST");
    uint16_t val;
    int n = this->read_from_serial_port((char*)&val, 2);
    if (n < 0) {
        throw std::runtime_error(std::string("Error reading from serial port: ") + 
                                 std::string(std::strerror(errno)));
    }

    // swap the two bytes
    return (val >> 8) | (val << 8);
}

/**
 * Erases the chip.
 */
uint16_t Serial::erase_chip() {
    this->send_command("ERASEALL");
    uint16_t val;
    int n = this->read_from_serial_port((char*)&val, 2);
    if (n < 0) {
        throw std::runtime_error(std::string("Error reading from serial port: ") + 
                                 std::string(std::strerror(errno)));
    }

    // swap the two bytes
    return (val >> 8) | (val << 8);
}

/**
 * Write sector on flash chip.
 * @param sector which sector to write to
 * @param data data to write to sector
 * @return Number of bytes written, or -1 on error.
 */
uint16_t Serial::write_sector(uint16_t sector, const std::vector<uint8_t>& data) {
    if(data.size() != (1024 * 4)) {
        throw std::runtime_error("Error: Data size must be 4KB");
    }
    
    char cmd[9];
    sprintf(cmd, "WRSECT%02X", sector);
    this->send_command(cmd);
    this->write_to_serial_port((char*)data.data(), data.size());
    
    uint16_t val;
    this->read_from_serial_port((char*)&val, 2);

    return val;
}

/**
 * Reads data from sector on flash chip.
 * @param sector which sector to read from
 * @param data data to read from sector
 */
void Serial::read_bank(uint16_t bank, std::vector<uint8_t>& chunk) {
    if(chunk.size() != (1024 * 16)) {
        throw std::runtime_error("Error: Data size must be 16KB");
    }
    char cmd[9];
    sprintf(cmd, "RDBANK%02X", bank);
    this->send_command(cmd);
    unsigned int bytesread = 0;
    while(bytesread < 1024 * 16) {
        int n = this->read_from_serial_port((char*)chunk.data() + bytesread, 1024*16 - bytesread);
        if(n < 0) {
            throw std::runtime_error(std::string("Error reading from serial port: ") + 
                                     std::string(std::strerror(errno)));
        }
        bytesread += n;
    }
}

/**
 * Closes the serial port.
 */
void Serial::close_serial_port() { 
    close(this->fd); 
}