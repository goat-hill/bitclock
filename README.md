<img src="https://github.com/goat-hill/bitclock/assets/220799/5121ca8b-19c7-4db0-9a81-38fae36ac5df" width="250" alt="bitclock" />

# bitclock
Open source e-ink clock

https://bitclock.io

<img src="https://github.com/goat-hill/bitclock/assets/220799/4416cb05-0625-438e-961c-3917e9b5334f" width="500" alt="Device photo" />

## Firmware

Source code for ESP32 using the ESP-IDF development framework.

See [bitclock-fw/](bitclock-fw/)

## Website

Next.js web app for [homepage](https://bitclock.io) and [device configurator](https://bitclock.io/connect).

See [bitclock-web/](bitclock-web/)

## Enclosure

OnShape design, STEP files, and instructions for 3D printing.

See [bitclock-enclosure/](bitclock-enclosure/)

## PCB

KiCad schematic, board, and JLCPCB ordering files.

See [bitclock-pcb/](bitclock-pcb/)


# Contributing

## Pre-commit linting

A single pre-commit config is created for this monorepo. Set up the hooks to get linting on every commit.

```fish
brew install pre-commit
pre-commit install
```
