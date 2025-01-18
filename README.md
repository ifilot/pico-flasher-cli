# PICO FLASHER

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/pico-flasher-cli?label=version)
[![build](https://github.com/ifilot/pico-flasher-cli/actions/workflows/build.yml/badge.svg)](https://github.com/ifilot/pico-flasher-cli/actions/workflows/build.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![Raspberry Pi](https://img.shields.io/badge/-Raspberry%20Pi-C51A4A?logo=Raspberry-Pi)

Command-line tool to interface with the [Pico SST39SF0x0 Programmer](https://github.com/ifilot/pico-sst39sf0x0-programmer).

> [!NOTE]
> Although the tool is designed for Linux machines in general, testing of
> the tool is primarily done on the Raspberry Pi.

## Compilation instructions

### Dependencies

Ensure you have the required dependencies on your system

```bash
sudo apt update && \
sudo apt install -y build-essential cmake libcurl4-openssl-dev \
libopenssl-dev libtclap-dev pkg-config libudev-dev
```

### Compilation

```bash
mkdir build
cd build
cmake ../src
make -j
```

To install **picoflash** systemwide, run

```bash
sudo make install
```

## Usage

Pico Flasher has 4 operational modes:

* Read: Read data from chip and write to binary file
* Write: Write data to chip from binary file
* Verify: Verify data on chip using binary file
* Erase: Erase all data on chip (set everything to `0xFF`)

Without any further parameters, these modes operate **on the whole chip**. For the
*write* and *verify* operations, it is also possible to execute these for a single
16 KiB bank.

**Read**

```bash
picoflash -o <BINFILE> -r
```

* `-o`: Output files
* `-r`: Read mode

**Write**

```bash
picoflash -i <BINFILE> -w [-b <bank>]
```

* `-i`: Input file: Can be either local file or URL. If a URL is supplied, the
  data is automatically grabbed from the internet via an internal CURL routine.
* `-w`: Write mode
* *(optional) `-b`: Bank to write to. Input file has to be strictly 16 KiB for this mode.

**Verify**

```bash
picoflash -i <BINFILE> -v
```

* `-i`: Input file: Can be either local file or URL. If a URL is supplied, the
  data is automatically grabbed from the internet via an internal CURL routine.
* `-v`: Verify mode
* *(optional) `-b`: Bank to write to. Input file has to be strictly 16 KiB for this mode.

**Erase**

```bash
picoflash -e
```

* `-e`: Erase mode

## Testing

There is also a test mode which will perform a number of operations on the chip,
basically checking both the working of the Pico Flasher as well as of the
SST39SF0x0 chip. To use this mode, simply run

```bash
picoflash -t
```

> [!IMPORTANT]
> This will irrevocably remove all data on the chip. If this is not your intention,
> make sure you make a copy of the chip's contents first using the *read* operation.