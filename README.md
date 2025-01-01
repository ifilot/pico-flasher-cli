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

```bash
./picoflash -i <PATH-TO-SOURCE>
```