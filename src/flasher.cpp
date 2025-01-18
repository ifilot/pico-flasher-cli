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
Flasher::Flasher(const std::string& path) {
    this->serial = std::make_unique<Serial>();
    this->serial->open_serial_port(path.c_str());

    // configure port
    if (!this->serial->configure_serial_port(B19200)) {
        this->serial->close_serial_port();
        throw std::runtime_error("Error configuring serial port.");
    }
}

/**
 * Reads the device ID from the serial port.
 * @return Device ID of the chip -if valid-.
 */
uint16_t Flasher::read_chip_id() {
    std::cout << "Interfacing with: " << this->serial->read_device_info() << std::endl;
    uint16_t devid = this->serial->get_device_id();

    std::string devname;
    switch(devid) {
        case 0xBFB7:
            devname = "SST39SF040";
        break;
        case 0xBFB6:
            devname = "SST39SF020";
        break;
        case 0xBFB5:
            devname = "SST39SF010";
        break;
        default:
            char buffer[7];
            sprintf(buffer, "0x%04X", devid);
            throw std::runtime_error("Unknown device ID (" + std::string(buffer) 
                                    + "): Cannot recognize SST39SF0x0 chip.");
        break;
    }
    std::cout << "Device ID: " << TEXTGREEN << "0x" << std::hex << std::uppercase
              << devid << TEXTWHITE << " (" << devname << ")" << std::endl;
    
    return devid;
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
    std::cout << "Reading data:" << std::endl;

    // read data
    auto chunk = std::vector<uint8_t>(1024 * 16);
    unsigned int nrbanks = data.size() / (1024 * 16);
    for(unsigned int i=0; i<nrbanks; i++) {
        std::vector<uint8_t> read_chunk(1024 * 16);
        this->serial->read_bank(i, read_chunk);

        std::cout << std::dec << std::setw(2) << std::setfill('0') << (i+1) << " [" << TEXTBLUE;
        std::cout << std::hex << std::setw(4) << std::setfill('0') << this->crc16_xmodem(read_chunk) << TEXTWHITE << "] " << std::flush;

        if((i+1) % 8 == 0) {
            std::cout << std::endl;
        } else if(i == nrbanks - 1) {
            std::cout << std::endl;
        }
        std::copy(read_chunk.begin(), read_chunk.end(), data.begin() + (i * 1024 * 16));
    }
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

        // calculate checksum
        uint16_t crc16 = this->crc16_xmodem(chunk);

        // perform transfer
        uint16_t checksum = this->serial->write_sector(i, chunk);

        std::cout << std::hex << std::setw(2) << std::setfill('0') << (i+1) << " [";
        if(checksum  == crc16) {
            std::cout << TEXTGREEN;
        } else {
            std::cout << TEXTRED;
        }
        std::cout << std::hex << std::setw(4) << std::setfill('0') << checksum << TEXTWHITE;
        std::cout << "] " << std::flush;

        if((i+1) % 8 == 0) {
            std::cout << std::endl;
        } else if(i == nrsectors - 1) {
            std::cout << std::endl;
        }
    }
}

/**
 * Writes data to a bank of the chip.
 * @param data Data to write to the chip.
 * @param bank Bank to write the data to.
 */
void Flasher::write_bank(const std::vector<uint8_t>& data, unsigned int bank) {
    unsigned int nrsectors = 4;//std::min((size_t)128, data.size() / 4096);
    std::cout << "Flashing " << nrsectors << " sectors, please wait..." << std::endl;
    auto chunk = std::vector<uint8_t>(1024 * 4);
    for (unsigned int i = 0; i < nrsectors; i++) {
        std::copy(data.begin() + (i * 1024 * 4), data.begin() + ((i + 1) * 1024 * 4), chunk.begin());

        // calculate checksum
        uint16_t crc16 = this->crc16_xmodem(chunk);

        // perform transfer
        uint16_t checksum = this->serial->write_sector(i, chunk);

        std::cout << std::hex << std::setw(2) << std::setfill('0') << (i+1) << " [";
        if(checksum  == crc16) {
            std::cout << TEXTGREEN;
        } else {
            std::cout << TEXTRED;
        }
        std::cout << std::hex << std::setw(4) << std::setfill('0') << checksum << TEXTWHITE;
        std::cout << "] " << std::flush;

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
 * Verifies the data on a bank of the chip.
 * @param data Data to verify on the chip.
 * @param bank Bank to verify the data on.
 * @throws std::runtime_error if the data does not match the chip.
 */
void Flasher::verify_bank(const std::vector<uint8_t>& data, unsigned int bank) {
    std::cout << "Verifying data:" << TEXTBLUE;

    // verify integrity
    auto chunk = std::vector<uint8_t>(1024 * 16);
    this->serial->read_bank(bank, chunk);

    std::cout << "Bank " << std::dec << std::setw(2) << std::setfill('0') << bank << TEXTWHITE << " [";

    if (std::equal(data.begin(), data.begin() + 1024 * 16, chunk.begin())) {
        std::cout << TEXTGREEN << "PASS";
    } else {
        std::cout << TEXTRED << "FAIL";
    }

    std::cout << TEXTWHITE << "] " << std::endl;
}

/**
 * Reads data from a file.
 * @param filename Name of the file to read.
 * @param data Data read from the file.
 */
void Flasher::read_file(const std::string& filename, std::vector<uint8_t>& data) {
    if(filename.find("https://") == 0 || filename.find("http://") == 0) {
        CURL* curl;
        CURLcode res;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, filename.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &this->curl_write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                throw std::runtime_error(std::string(curl_easy_strerror(res)));
            } else {
                std::cout << "Retrieving " << TEXTBLUE << filename << TEXTWHITE << " (" 
                            << std::dec << data.size() << " bytes)" << std::endl;
            }
            curl_easy_cleanup(curl);
        }

        if(data.size() == 0) {
            throw std::runtime_error("Error retrieving file.");
        } else {

        }

    } else {
        std::ifstream infile(filename, std::ios::binary);
        if (infile) {
            infile.seekg(0, std::ios::end);
            std::streamsize size = infile.tellg();
            infile.seekg(0, std::ios::beg);

            // Resize the vector to fit the file content
            data.resize(size);

            // Read the file content into the vector
            if (infile.read(reinterpret_cast<char*>(data.data()), size)) {
                std::cout << "Reading " << TEXTBLUE << filename << TEXTWHITE << " (" 
                            << std::dec << data.size() << " bytes)" << std::endl;
            } else {
                throw std::runtime_error("Error reading file.");
            }
        } else {
            throw std::runtime_error("Error opening file.");
        }
    }

    // calculate md5 checksum and output it
    std::string md5sum = this->calculate_md5(data);
    std::cout << "MD5: " << TEXTBLUE << md5sum << TEXTWHITE << std::endl;
}

/**
 * Writes data to a file.
 * @param filename Name of the file to write.
 * @param data Data to write to the file.
 */
void Flasher::write_file(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream outfile(filename, std::ios::binary);
    if (outfile) {
        outfile.write(reinterpret_cast<const char*>(data.data()), data.size());
        std::cout << "Writing " << TEXTBLUE << filename << TEXTWHITE << " (" 
                    << std::dec << data.size() << " bytes)" << std::endl;
        std::cout << "MD5: " << TEXTBLUE << this->calculate_md5(data) << TEXTWHITE << std::endl;
    } else {
        throw std::runtime_error("Error opening file.");
    }
}

/**
 * Calculates the CRC16 checksum of the given data.
 * @param data Data to calculate the checksum for.
 * @return CRC16 checksum of the data.
 */
uint16_t Flasher::crc16_xmodem(const std::vector<uint8_t>& data) {
    uint32_t crc = 0;
    static const uint16_t poly = 0x1021;

    for(uint16_t i=0; i<data.size(); i++) {
      crc = crc ^ (data[i] << 8);
      for (uint8_t j=0; j<8; j++) {
        crc = crc << 1;
        if (crc & 0x10000) {
            crc = (crc ^ poly) & 0xFFFF;
        }
      }
    }

    return (uint16_t)crc;
}

/**
 * Calculates the MD5 checksum of the given data.
 * @param data Data to calculate the checksum for.
 * @return MD5 checksum of the data.
 */
std::string Flasher::calculate_md5(const std::vector<uint8_t>& data) {
    // Create an EVP context
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize the context with the MD5 algorithm
    if (EVP_DigestInit_ex(ctx, EVP_md5(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize MD5 digest");
    }

    // Pass the data to the context
    if (EVP_DigestUpdate(ctx, data.data(), data.size()) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update MD5 digest");
    }

    // Finalize the digest
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    if (EVP_DigestFinal_ex(ctx, digest, &digest_len) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize MD5 digest");
    }

    // Free the context
    EVP_MD_CTX_free(ctx);

    // Convert the digest to a hexadecimal string
    std::ostringstream md5String;
    for (unsigned int i = 0; i < digest_len; ++i) {
        md5String << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }

    return md5String.str();
}

/**
 * Write callback function of CURL
 * @param ptr Pointer to the data to write.
 * @param size Size of the data to write.
 * @param nmemb Number of members to write.
 * @param userdata Userdata to pass to the callback.
 */
size_t Flasher::curl_write_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t total_size = size * nmemb;
    std::vector<uint8_t>* data = reinterpret_cast<std::vector<uint8_t>*>(userdata);
    data->insert(data->end(), reinterpret_cast<uint8_t*>(ptr), reinterpret_cast<uint8_t*>(ptr) + total_size);
    return total_size;
}