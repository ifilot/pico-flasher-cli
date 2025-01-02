# PICO FLASHER

Command-line tool to interface with the [Pico SST39SF0x0 Programmer](https://github.com/ifilot/pico-sst39sf0x0-programmer)

## Compilation instructions

### Dependencies

Ensure you have the required dependencies on your system

```bash
sudo apt update && sudo apt install -y build-essential cmake libtclap-dev pkg-config
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

### Verify

```bash
./picoflash -i <BINFILE> -v
```

### Erase

```bash
./picoflash -e
```