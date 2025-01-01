#include <iostream>
#include <tclap/CmdLine.h>
#include <fstream>

#include "serial.h"

#define TEXTGREEN "\033[1;92m"
#define TEXTWHITE "\033[0m"

int main(int argc, char* argv[]) {
    try {
        TCLAP::CmdLine cmd("Transfer data to SST39SF0x0 chip");

        // input filename
        TCLAP::ValueArg<std::string> arg_input_filename("i","input","Input file (i.e. .BIN)",false,"DATA.BIN","filename");
        cmd.add(arg_input_filename);
        cmd.parse(argc, argv);

        //**************************************
        // Inform user about execution
        //**************************************
        // std::cout << "--------------------------------------------------------------" << std::endl;
        // std::cout << "Executing "<< PROGRAM_NAME << " v." << PROGRAM_VERSION << std::endl;
        // std::cout << "Author:  Ivo Filot <i.a.w.filot@tue.nl>" << std::endl;
        // std::cout << "Website: https://den2obj.imc-tue.nl" << std::endl;
        // std::cout << "Github:  https://github.com/ifilot/den2obj" << std::endl;
        // std::cout << "--------------------------------------------------------------" << std::endl;
        // std::cout << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
        // std::cout << "Git Hash: " << PROGRAM_GIT_HASH << std::endl;
        // std::cout << "--------------------------------------------------------------" << std::endl;
        auto ser = Serial();

        // open port
        const char* port_name = "/dev/ttyACM0";
        ser.open_serial_port(port_name);

        // configure port
        if (!ser.configure_serial_port(B115200)) {
            ser.close_serial_port();
            return 1;
        }

        std::cout << "Interfacing with: " << ser.read_device_info() << std::endl;
        std::cout << "Device ID: 0x" << std::hex << std::uppercase << ser.get_device_id() << std::endl;

        // std::cout << "Clearing chip";
        // unsigned int nriter = ser.erase_chip();
        // std::cout << " - Done (" << nriter << " polls)" << std::endl;

        // read data from file
        std::vector<uint8_t> data;
        std::ifstream infile(arg_input_filename.getValue(), std::ios::binary);
        if (infile) {
            infile.seekg(0, std::ios::end);
            std::streamsize size = infile.tellg();
            infile.seekg(0, std::ios::beg);

            // Resize the vector to fit the file content
            data.resize(size);

            // Read the file content into the vector
            if (infile.read(reinterpret_cast<char*>(data.data()), size)) {
                std::cout << "Reading " << arg_input_filename.getValue() << " (" 
                          << std::dec << size << " bytes)" << std::endl;
            } else {
                throw std::runtime_error("Error reading file.");
            }
        } else {
            throw std::runtime_error("Error opening file.");
        }

        unsigned int nrsectors = std::min((size_t)128, data.size() / 4096);
        std::cout << "Flashing " << nrsectors << " sectors, please wait..." << std::endl;
        auto chunk = std::vector<uint8_t>(1024 * 4);
        // for (unsigned int i = 0; i < nrsectors; i++) {
        //     std::copy(data.begin() + (i * 1024 * 4), data.begin() + ((i + 1) * 1024 * 4), chunk.begin());

        //     std::cout << std::dec << std::setw(3) << std::setfill('0') << (i+1) << " [" << TEXTGREEN;
        //     std::cout << std::hex << std::setw(4) << std::setfill('0') << ser.write_sector(i, chunk);
        //     std::cout << "] " << TEXTWHITE << std::flush;

        //     if((i+1) % 8 == 0) {
        //         std::cout << std::endl;
        //     } else if(i == nrsectors - 1) {
        //         std::cout << std::endl;
        //     }
        // }

        // verify integrity
        chunk.resize(1024 * 16);
        unsigned int nrbanks = nrsectors / 4;
        for(unsigned int i=0; i<nrbanks; i++) {
            ser.read_bank(i, chunk);

            if(std::equal(data.begin() + (i * 1024 * 16), data.begin() + ((i + 1) * 1024 * 16), chunk.begin())) {
                std::cout << "Bank " << i << " [" << TEXTGREEN << "OK" << TEXTWHITE << "]" << std::endl;
            } else {
                std::cout << "Bank " << i << " [" << TEXTGREEN << "FAIL" << TEXTWHITE << "]" << std::endl;
            }
        }

        ser.close_serial_port();
        return 0;
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return -1;
    }
}
