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

#pragma once

#include <memory>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <openssl/evp.h>
#include <curl/curl.h>

#include "serial.h"

#define TEXTGREEN "\033[1;92m"
#define TEXTWHITE "\033[0m"
#define TEXTRED "\033[1;91m"
#define TEXTBLUE "\033[1;94m"

class Flasher {
private:
    std::unique_ptr<Serial> serial;

public:
    /**
     * Constructor for the Flasher class.
     */
    Flasher(const std::string& path);

    /**
     * Reads the device ID from the serial port.
     * @return Device ID of the chip -if valid-.
     */
    uint16_t read_chip_id();

    /**
     * Erases the chip.
     */
    void erase_chip();

    /**
     * Reads data from the chip.
     * @param data Data read from the chip.
     */
    void read_chip(std::vector<uint8_t>& data);

    /**
     * Writes data to the chip.
     * @param data Data to write to the chip.
     */
    void write_chip(const std::vector<uint8_t>& data);

    /**
     * Verifies the data on the chip.
     * @param data Data to verify on the chip.
     */
    void verify_chip(const std::vector<uint8_t>& data);

    /**
     * Verifies the data on a bank of the chip.
     * @param data Data to verify on the chip.
     * @param bank Bank to verify the data on.
     * @throws std::runtime_error if the data does not match the chip.
     */
    void verify_bank(const std::vector<uint8_t>& data, unsigned int bank);

    /**
     * Reads data from a file.
     * @param filename Name of the file to read.
     * @param data Data read from the file.
     */
    void read_file(const std::string& filename, std::vector<uint8_t>& data);

    /**
     * Writes data to a file.
     * @param filename Name of the file to write.
     * @param data Data to write to the file.
     */
    void write_file(const std::string& filename, const std::vector<uint8_t>& data);

private:
    /**
     * Calculates the CRC16 checksum of the given data.
     * @param data Data to calculate the checksum for.
     * @return CRC16 checksum of the data.
     */
    uint16_t crc16_xmodem(const std::vector<uint8_t>& data);

    /**
     * Calculates the MD5 checksum of the given data.
     * @param data Data to calculate the checksum for.
     * @return MD5 checksum of the data.
     */
    std::string calculate_md5(const std::vector<uint8_t>& data);

    /**
     * Write callback function of CURL
     * @param ptr Pointer to the data to write.
     * @param size Size of the data to write.
     * @param nmemb Number of members to write.
     * @param userdata Userdata to pass to the callback.
     */
    static size_t curl_write_callback(void* ptr, size_t size, size_t nmemb, void* userdata);
};