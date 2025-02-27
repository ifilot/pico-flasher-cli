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

#include <iostream>
#include <vector>
#include <string>
#include <libudev.h>
#include <sstream>

// Structure to store serial port information
struct serial_port_info {
    std::string device_path;
    std::string vendor_id;
    std::string product_id;
};

class SerialPort {
public:
    SerialPort();

    /**
     * List all serial ports and their IDs
     * @return vector of pairs with device path and device ID
     */
    std::vector<std::pair<std::string, std::string>> list_serial_ports_with_ids();
};
