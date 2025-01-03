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
        cmd.add(arg_erase);
        cmd.add(arg_write);
        cmd.add(arg_read);
        cmd.add(arg_verify);

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
        
        SerialPort sp;
        auto ports = sp.list_serial_ports_with_ids();
        if(ports.size() != 1) {
            std::cerr << "Error: No device found." << std::endl;
            return 1;
        } else {
            std::cout << "Autoselecting: " << ports[0].device_path << std::endl;
        }

        Flasher flasher(ports[0].device_path);
        flasher.read_chip_id();

        if(arg_erase.getValue()) {
            flasher.erase_chip();
        } else if(arg_write.getValue()) {
            std::vector<uint8_t> data;
            flasher.read_file(arg_input_filename.getValue(), data);
            flasher.erase_chip();
            flasher.write_chip(data);
            flasher.verify_chip(data);
        } else if(arg_read.getValue()) {
            std::vector<uint8_t> data;
            flasher.read_chip(data);
        } else if(arg_verify.getValue()) {
            std::vector<uint8_t> data;
            flasher.read_file(arg_input_filename.getValue(), data);
            flasher.verify_chip(data);
        }

        std::cout << "All done!" << std::endl;

        return 0;
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return -1;
    }
}
