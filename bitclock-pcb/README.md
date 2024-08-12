# bitclock PCB

Each directory includes gerber + BOM files used for ordering from a PCBA service like JLCPCB.

Latest PCB version is [rev3a](rev3a/)

## JLCPCB ordering scripts

See https://github.com/babldev/kicad-jlcpcb-scripts

## Revision history

#### rev3a
First version sold on https://bitclock.io/

It does include a few extraneous components that were an attempt to support the unreleased [BMV080](https://www.bosch-sensortec.com/products/environmental-sensors/particulate-matter-sensor/bmv080/) particulate sensor. I may have misinterpreted the preliminary datasheet so feel free to omit that connector and supporting capacitors. It also designs for an option side-USB port that I may omit in the future due to added routing complexity and it is unused.
