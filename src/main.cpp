// serial_posix.cpp
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
using namespace std;

// Function to open the serial port
int openSerialPort(const char* portname)
{
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::cerr << "Error opening " << portname << ": "
                  << std::strerror(errno) << std::endl;
        return -1;
    }
    return fd;
}

// Function to configure the serial port
bool configureSerialPort(int fd, int speed)
{
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error from tcgetattr: " << std::strerror(errno)
                  << std::endl;
        return false;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag
        = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0; // no signaling chars, no echo, no
                     // canonical processing
    tty.c_oflag = 0; // no remapping, no delays
    tty.c_cc[VMIN] = 0; // read doesn't block
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag
        |= (CLOCAL | CREAD); // ignore modem controls,
                             // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr: " << std::strerror(errno)
                  << std::endl;
        return false;
    }
    return true;
}

// Function to read data from the serial port
int readFromSerialPort(int fd, char* buffer, size_t size)
{
    return read(fd, buffer, size);
}

// Function to write data to the serial port
int writeToSerialPort(int fd, const char* buffer,
                      size_t size)
{
    return write(fd, buffer, size);
}

// Function to close the serial port
void closeSerialPort(int fd) { close(fd); }

int main()
{
    // Replace with your serial port name
    const char* portname = "/dev/ttyACM0";
    int fd = openSerialPort(portname);
    if (fd < 0)
        return 1;

    if (!configureSerialPort(fd, B115200)) {
        closeSerialPort(fd);
        return 1;
    }

    const char* message = "READINFO";
    if (writeToSerialPort(fd, message, strlen(message))
        < 0) {
        cerr << "Error writing to serial port: "
             << strerror(errno) << endl;
    }

    char buffer[100];
    int n = readFromSerialPort(fd, buffer, 8);
    if (n < 0) {
        cerr << "Error reading from serial port: "
             << strerror(errno) << endl;
    }
    else {
        cout << "Read from serial port: "
             << std::string(buffer, n) << endl;
        cout << n << endl;
    }

    n = readFromSerialPort(fd, buffer, 16);
    if (n < 0) {
        cerr << "Error reading from serial port: "
             << strerror(errno) << endl;
    }
    else {
        cout << "Read from serial port: "
             << std::string(buffer, n) << endl;
        cout << n << endl;
    }

    closeSerialPort(fd);
    return 0;
}
