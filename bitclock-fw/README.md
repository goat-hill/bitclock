# bitclock firmware

## Bootstrap

### One-time esp-idf setup

[esp-idf](https://github.com/espressif/esp-idf) (the ESP32 development framework) needs to be installed separately before building the firmware.

Bash shell

```fish
brew install cmake ninja dfu-util python@3.12
brew reinstall openssl
git clone -b v5.2.2 --recursive https://github.com/espressif/esp-idf.git
./install.sh esp32s3
source export.sh
```

Fish shell

```fish
brew install cmake ninja dfu-util python@3.12
brew reinstall openssl
git clone -b v5.2.2 --recursive https://github.com/espressif/esp-idf.git
./install.fish esp32s3
source export.fish
```

### Entering esp-idf environment

Bash shell

```sh
source ~/path-to-esp-idf/export.sh
```

Fish shell

```fish
source ~/path-to-esp-idf/export.fish
```

## Target directories

In order to maintain older PCB revision targets, we have separate directories for each PCB revision. Despite most files being shared across targets (via symlinks) we do this because esp-idf doesn't handle multiple targets well. This gives each target it's own build configuration and directory.

```fish
cd target-rev2-esp32s3/
```

To avoid confusing your code editor with duplicate files across targets, it is recommended to open VSCode with a single target as the root directory.

```fish
code .
```

## Build

```fish
idf.py build
```

## Flash

```fish
idf.py flash
```

## Monitor

```fish
idf.py monitor
```

## Debug

```fish
idf.py openocd
```

Then go to VSCode and "Start Debugging" (F5)

## Developer tips

### Bluetooth GATT attributes

If you edit GATT attributes, the web configurator may stop working. This is doing some kind of caching of GATT attributes by service ID.

To resolve the issue, go into Bluetooth settings and "Forget Device", then re-pair it to refresh the GATT cache.
