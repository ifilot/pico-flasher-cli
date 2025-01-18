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

#include <iostream>
#include <tclap/CmdLine.h>
#include <cmath>

#include "config.h"
#include "flasher.h"
#include "serialport.h"

int main(int argc, char* argv[]) {
    try {
        TCLAP::CmdLine cmd("Transfer data to SST39SF0x0 chip");

        // input filename
        TCLAP::ValueArg<std::string> arg_input_filename("i","input","Input file (i.e. .BIN)",false,"DATA.BIN","filename");
        cmd.add(arg_input_filename);

        // output filename
        TCLAP::ValueArg<std::string> arg_output_filename("o","output","Output file (i.e. .BIN)",false,"DATA.BIN","filename");
        cmd.add(arg_output_filename);

        // operation modes
        TCLAP::SwitchArg arg_erase("e","erase","Erase chip",false);
        TCLAP::SwitchArg arg_write("w","write","Write data to chip",false);
        TCLAP::SwitchArg arg_read("r","read","Read data from chip",false);
        TCLAP::SwitchArg arg_verify("v","verify","Verify data on chip",false);
        TCLAP::ValueArg<unsigned int> arg_bank("b", "bank", "Bank number", false, 0, "bank");
        cmd.add(arg_erase);
        cmd.add(arg_write);
        cmd.add(arg_read);
        cmd.add(arg_verify);
        cmd.add(arg_bank);

        cmd.parse(argc, argv);

        // **************************************
        // Inform user about execution
        // **************************************
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Executing "<< PROGRAM_NAME << " v." << PROGRAM_VERSION << std::endl;
        std::cout << "Author:  Ivo Filot <ivo@ivofilot.nl>" << std::endl;
        std::cout << "Github:  https://github.com/ifilot/pico-flasher-cli" << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Git Hash: " << PROGRAM_GIT_HASH << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        
        // get operation mode
        unsigned int modes = arg_erase.getValue() + arg_write.getValue() + arg_read.getValue() + arg_verify.getValue();
        if(modes != 1) {
            std::cerr << "Error: Please select one operation mode." << std::endl;
            std::cerr << "Select one of the following modes: -e, -w, -r, -v" << std::endl;
            return 1;
        }
        
        // loop over all serial devices and look for the signature of a RASPBERRY PI PICO
        SerialPort sp;
        auto devices = sp.list_serial_ports_with_ids();
        std::string dev;
        std::cout << "Listing serial devices:" << std::endl;
        for(const auto& device : devices) {
            std::cout << device.second << " " << device.first << std::endl;
            if(device.second == "2e8a:0009") {
                dev = "/dev/" + device.first;
                std::cout << "Autoselecting: " << dev << std::endl;
                continue;
            }
        }

        // throw an error if no suitable device was found
        if(dev.empty()) {
            throw std::runtime_error("Error: No valid serial device found. Did you connect the PICO Flasher?");
        }

        Flasher flasher(dev);
        uint16_t devid = flasher.read_chip_id();
        size_t romsize = 0;
        switch(devid) {
            case 0xBFB7:
                romsize = 512 * 1024;
            break;
            case 0xBFB6:
                romsize = 256 * 1024;
            break;
            case 0xBFB5:
                romsize = 128 * 1024;
            break;
            default:
                throw std::logic_error("Error: Unknown device ID.");
                return 1;
            break;
        }

        if(arg_erase.getValue()) {
            flasher.erase_chip();
        } else if(arg_write.getValue()) {
            std::vector<uint8_t> data;
            flasher.read_file(arg_input_filename.getValue(), data);
            
            if(arg_bank.isSet()) {
                unsigned int bank = arg_bank.getValue();
                unsigned int max_bank = 8 * std::pow(2, (devid - 0xBFB5));

                // check if data size is appropriate
                if(data.size() != 0x4000) {
                    throw std::runtime_error("Error: Data size must be 16KB");
                }
                
                // check if bank number is appropriate
                std::cout << "Flashing ROM bank: " << bank << std::endl;
                if(bank >= max_bank) {
                    throw std::runtime_error("Error: Bank number must be between 0 and " + std::to_string(max_bank-1) + ".");
                }

                flasher.write_bank(data, bank);
                flasher.verify_bank(data, bank);
            } else {
                if(data.size() > romsize) {
                    throw std::runtime_error("Error: File size too large.");
                } else if(romsize > data.size()) {
                    std::cout << "Resizing file to match chip size, appending zeros." << std::endl;
                    data.resize(romsize, 0);
                }

                flasher.erase_chip();
                flasher.write_chip(data);
                flasher.verify_chip(data);
            }
        } else if(arg_read.getValue()) {
            std::vector<uint8_t> data(romsize, 0);
            flasher.read_chip(data);
            flasher.write_file(arg_output_filename.getValue(), data);
        } else if(arg_verify.getValue()) {
            std::vector<uint8_t> data;
            flasher.read_file(arg_input_filename.getValue(), data);

            // choose whether to verify the whole chip or just a single bank
            if(arg_bank.isSet()) {
                unsigned int bank = arg_bank.getValue();
                unsigned int max_bank = 8 * std::pow(2, (devid - 0xBFB5));

                // check if data size is appropriate
                if(data.size() != 1024 * 16) {
                    throw std::runtime_error("Error: Data size must be 16KB");
                }
                
                // check if bank number is appropriate
                std::cout << "Checking ROM bank: " << bank << std::endl;
                if(bank >= max_bank) {
                    throw std::runtime_error("Error: Bank number must be between 0 and " + std::to_string(max_bank-1) + ".");
                }
                
                flasher.verify_bank(data, bank);
            } else {
                if(data.size() != romsize) {
                    throw std::runtime_error("Error: File size does not match chip size.");
                }
                
                flasher.verify_chip(data);
            }
        }

        std::cout << "All done!" << std::endl;

        return 0;
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return -1;
    }
}
