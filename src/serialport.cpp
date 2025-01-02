SerialPort::SerialPort() {}

// Function to list all serial ports and their IDs
std::vector<serial_port_info> SerialPort::list_serial_ports_with_ids() {
    std::vector<serial_port_info> serial_ports;
    const std::string dev_path = "/dev";
    DIR *dir = opendir(dev_path.c_str());

    if (!dir) {
        perror("Failed to open /dev directory");
        return serial_ports;
    }

    struct dirent *entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;

        // Check if filename matches serial port patterns
        if (std::regex_match(filename, std::regex("tty(S|USB|ACM)[0-9]+"))) {
            std::string full_path = dev_path + "/" + filename;

            // Verify it's a character device
            if (stat(full_path.c_str(), &file_stat) == 0 && S_ISCHR(file_stat.st_mode)) {
                serial_port_info port_info;
                port_info.device_path = full_path;

                // Locate sysfs information
                std::string tty_path = "/sys/class/tty/" + filename + "/device";
                char resolved_path[PATH_MAX];
                if (realpath(tty_path.c_str(), resolved_path)) {
                    std::string device_path = resolved_path;
                    size_t usb_pos = device_path.find("/usb");
                    if (usb_pos != std::string::npos) {
                        std::string usb_device_path = device_path.substr(0, usb_pos + 4);

                        // Read idVendor and idProduct
                        std::ifstream id_vendor_file(usb_device_path + "/idVendor");
                        std::ifstream id_product_file(usb_device_path + "/idProduct");

                        if (id_vendor_file.is_open()) {
                            id_vendor_file >> port_info.vendor_id;
                            id_vendor_file.close();
                        }
                        if (id_product_file.is_open()) {
                            id_product_file >> port_info.product_id;
                            id_product_file.close();
                        }
                    }
                }

                serial_ports.push_back(port_info);
            }
        }
    }

    closedir(dir);
    return serial_ports;
}
}
