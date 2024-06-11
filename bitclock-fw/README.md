# bitclock firmware

## Bootstrap

```fish
brew install cmake ninja dfu-util python@3.12
brew reinstall openssl
git clone -b v5.2.2 --recursive https://github.com/espressif/esp-idf.git
./install.fish esp32s3
```

## Pre-commit linting

```fish
brew install pre-commit
pre-commit install
```

## Entering environment

```fish
source ~/code/esp-idf/export.fish
```

## Build

```fish
cd target-rev2-esp32s3/
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
