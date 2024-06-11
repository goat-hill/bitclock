<img src="https://github.com/goat-hill/bitclock/assets/220799/5121ca8b-19c7-4db0-9a81-38fae36ac5df" width="250" alt="bitclock" />

# bitclock
Open source e-ink desk companion, clock, and air quality monitor.

https://bitclock.io

<img src="https://github.com/goat-hill/bitclock/assets/220799/4416cb05-0625-438e-961c-3917e9b5334f" width="500" alt="Device photo" />

Visit https://bitclock.io/connect to configure your existing device

## Firmware

Source code for ESP32 using the ESP-IDF development framework.

See [bitclock-fw/](bitclock-fw/)

## Website

Next.js web app for [homepage](https://bitclock.io) and [device configurator](https://bitclock.io/connect).

See [bitclock-web/](bitclock-web/)

<img src="https://github.com/goat-hill/bitclock/assets/220799/9b72ab4b-d259-4e13-9049-c8f1ba85664d" width="500" alt="Web configurator screenshot" />

## Enclosure

OnShape design, STEP files, and instructions for 3D printing.

See [bitclock-enclosure/](bitclock-enclosure/)

<img src="https://github.com/goat-hill/bitclock/assets/220799/b83af06f-92e7-4f3a-9e44-477fad33d226" width="500" alt="Device CAD screenshot" />

## PCB

KiCad schematic, board, and JLCPCB ordering files.

See [bitclock-pcb/](bitclock-pcb/)

<img src="https://github.com/goat-hill/bitclock/assets/220799/4e667f21-f8f4-497b-bb93-be5a299950ad" width="500" alt="KiCAD device PCB screenshot" />

# Contributing

## Pre-commit linting

A single pre-commit config is created for this monorepo. Set up the hooks to get linting on every commit.

```fish
brew install pre-commit
pre-commit install
```
