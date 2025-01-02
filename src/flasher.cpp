/**************************************************************************
 *                                                                        *
 *   Author: Ivo Filot <ivo@ivofilot.nl>                                  *
 *                                                                        *
 *   PICOFLASH is free software:                                          *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   PICOFLASH is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "flasher.h"

/**
 * Constructor for the Flasher class.
 */
Flasher::Flasher() {
    this->serial = std::make_unique<Serial>();
    const char* port_name = "/dev/ttyACM0";
    this->serial->open_serial_port(port_name);

    // configure port
    if (!this->serial->configure_serial_port(B19200)) {
        this->serial->close_serial_port();
        throw std::runtime_error("Error configuring serial port.");
    }
}

/**
 * Reads the device ID from the serial port.
 */
void Flasher::read_chip_id() {
    std::cout << "Interfacing with: " << this->serial->read_device_info() << std::endl;
    std::cout << "Device ID: 0x" << std::hex << std::uppercase << this->serial->get_device_id() << std::endl;
}

/**
 * Erases the chip.
 */
void Flasher::erase_chip() {
    std::cout << "Clearing chip";
    unsigned int nriter = this->serial->erase_chip();
    std::cout << " - Done (" << nriter << " polls)" << std::endl;
}

/**
 * Reads data from the chip.
 * @param data Data read from the chip.
 */
void Flasher::read_chip(std::vector<uint8_t>& data) {

}

/**
 * Writes data to the chip.
 * @param data Data to write to the chip.
 */
void Flasher::write_chip(const std::vector<uint8_t>& data) {
    unsigned int nrsectors = std::min((size_t)128, data.size() / 4096);
    std::cout << "Flashing " << nrsectors << " sectors, please wait..." << std::endl;
    auto chunk = std::vector<uint8_t>(1024 * 4);
    for (unsigned int i = 0; i < nrsectors; i++) {
        std::copy(data.begin() + (i * 1024 * 4), data.begin() + ((i + 1) * 1024 * 4), chunk.begin());

        std::cout << std::dec << std::setw(3) << std::setfill('0') << (i+1) << " [" << TEXTGREEN;
        std::cout << std::hex << std::setw(4) << std::setfill('0') << this->serial->write_sector(i, chunk);
        std::cout << "] " << TEXTWHITE << std::flush;

        if((i+1) % 8 == 0) {
            std::cout << std::endl;
        } else if(i == nrsectors - 1) {
            std::cout << std::endl;
        }
    }
}

/**
 * Verifies the data on the chip.
 * @param data Data to verify on the chip.
 */
void Flasher::verify_chip(const std::vector<uint8_t>& data) {
    std::cout << "Verifying data:" << std::endl;

    // verify integrity
    auto chunk = std::vector<uint8_t>(1024 * 16);
    unsigned int nrbanks = data.size() / (1024 * 16);
    for(unsigned int i=0; i<nrbanks; i++) {
        std::vector<uint8_t> read_chunk(1024 * 16);
        this->serial->read_bank(i, read_chunk);

        std::cout << std::dec << std::setw(2) << std::setfill('0') << (i+1) << " [";

        if (std::equal(data.begin() + (i * 1024 * 16), data.begin() + ((i + 1) * 1024 * 16), read_chunk.begin())) {
            std::cout << TEXTGREEN << "PASS";
        } else {
            std::cout << TEXTRED << "FAIL";
        }

        std::cout << TEXTWHITE << "] " << std::flush;

        if((i+1) % 8 == 0) {
            std::cout << std::endl;
        } else if(i == nrbanks - 1) {
            std::cout << std::endl;
        }
    }
}

/**
 * Reads data from a file.
 * @param filename Name of the file to read.
 * @param data Data read from the file.
 */
void Flasher::read_file(const std::string& filename, std::vector<uint8_t>& data) {
    std::ifstream infile(filename, std::ios::binary);
    if (infile) {
        infile.seekg(0, std::ios::end);
        std::streamsize size = infile.tellg();
        infile.seekg(0, std::ios::beg);

        // Resize the vector to fit the file content
        data.resize(size);

        // Read the file content into the vector
        if (infile.read(reinterpret_cast<char*>(data.data()), size)) {
            std::cout << "Reading " << filename << " (" 
                        << std::dec << data.size() << " bytes)" << std::endl;
        } else {
            throw std::runtime_error("Error reading file.");
        }
    } else {
        throw std::runtime_error("Error opening file.");
    }
}