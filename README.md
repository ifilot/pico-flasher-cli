# PICO FLASHER

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/pico-flasher-cli?label=version)
[![build](https://github.com/ifilot/pico-flasher-cli/actions/workflows/build.yml/badge.svg)](https://github.com/ifilot/pico-flasher-cli/actions/workflows/build.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

Command-line tool to interface with the [Pico SST39SF0x0 Programmer](https://github.com/ifilot/pico-sst39sf0x0-programmer).

## Compilation instructions

### Dependencies

Ensure you have the required dependencies on your system

```bash
sudo apt update && sudo apt install -y build-essential cmake libcurl4-openssl-dev libopenssl-dev libtclap-dev pkg-config
```

### Compilation

```bash
mkdir build
cd build
cmake ../src
make -j
```

## Usage

Pico Flasher has 4 operational modes:

* Read: Read data from chip and write to binary file
* Write: Write data to chip from binary file
* Verify: Verify data on chip using binary file
* Erase: Erase all data on chip (set everything to `0xFFFF`)

### Read

```bash
./picoflash -o <BINFILE> -r
```

### Write

```bash
./picoflash -i <BINFILE> -w
```

> [!NOTE]
> You can supply either a local path or a URL to `-i`. When passing a URL,
> **Pico Flasher** will automatically try to retrieve the file over the internet.

### Verify

```bash
./picoflash -i <BINFILE> -v
```

> [!NOTE]
> You can supply either a local path or a URL to `-i`. When passing a URL,
> **Pico Flasher** will automatically try to retrieve the file over the internet.

### Erase

```bash
./picoflash -e
```
