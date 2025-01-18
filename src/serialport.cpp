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

#include "serialport.h"

SerialPort::SerialPort() {}

/**
 * List all serial ports and their IDs
 * @return vector of pairs with device path and device ID
 */
std::vector<std::pair<std::string, std::string>> SerialPort::list_serial_ports_with_ids() {
    std::vector<std::pair<std::string, std::string>> devices;

    // Create a udev object
    struct udev *udev = udev_new();
    if (!udev) {
        std::cerr << "Failed to create udev context" << std::endl;
        return devices;
    }

    // Create an enumerator to scan the devices
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    if (!enumerate) {
        std::cerr << "Failed to create udev enumerator" << std::endl;
        udev_unref(udev);
        return devices;
    }

    // Look for devices in the "tty" subsystem
    udev_enumerate_add_match_subsystem(enumerate, "tty");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices_list = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    // Iterate over all tty devices
    udev_list_entry_foreach(entry, devices_list) {
        const char *syspath = udev_list_entry_get_name(entry);
        struct udev_device *dev = udev_device_new_from_syspath(udev, syspath);

        if (dev) {
            const char *devname = udev_device_get_sysname(dev);

            // Only consider devices that start with "ttyACM"
            if (std::string(devname).find("ttyACM") == 0) {
                // Get the parent USB device
                struct udev_device *parent = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
                if (parent) {
                    const char *idVendor = udev_device_get_sysattr_value(parent, "idVendor");
                    const char *idProduct = udev_device_get_sysattr_value(parent, "idProduct");

                    if (idVendor && idProduct) {
                        std::ostringstream vendor_product;
                        vendor_product << idVendor << ":" << idProduct;
                        devices.emplace_back(devname, vendor_product.str());
                    }
                }
            }

            udev_device_unref(dev);
        }
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    return devices;
}