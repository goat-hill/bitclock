"use client";

import React from "react";
import {
  Badge,
  Box,
  Button,
  Paper,
  Container,
  Divider,
  Group,
} from "@mantine/core";
import { tzmap } from "@/libs/tz";
import { useState } from "react";
import Logo from "../logo";
import * as gatt from "@/libs/gatt";
import WifiBlock from "./wifi-block";
import { BluetoothConnection } from "./ble-connection";
import FirmwareBlock from "./firmware-block";
import WeatherBlock from "./weather-block";
import ClimateBlock from "./climate-block";
import ClockBlock from "./clock-block";
import AqiBlock from "./aqi-block";
import AppSelectionBlock from "./app-selection-block";
import { IconBluetooth } from "@tabler/icons-react";

type GattAttributeParser<T> = {
  attributeId: number | string;
  parse: (value: DataView) => T;
  setter: (value: T) => void;
};

export default function Home() {
  const [bluetoothConnection, setBluetoothConnection] =
    useState<BluetoothConnection | null>(null);
  const [temperature, setTemperature] = useState(0);
  const [humidity, setHumidity] = useState(0);
  const [co2, setCo2] = useState(0);
  const [vocIndex, setVocIndex] = useState(0);
  const [noxIndex, setNoxIndex] = useState(0);
  const [temperatureUnit, setTemperatureUnit] = useState(0);
  const [timezone, setTimezone] = useState<string | null>(null);
  const [clockFormat, setClockFormat] = useState(0);
  const [weatherPath, setWeatherPath] = useState<string | null>(null);
  const [wifiStatus, setWifiStatus] = useState<gatt.WifiStatus | null>(null);
  const [appSelection, setAppSelection] = useState(0);

  const decoder = new TextDecoder();

  const timezoneDecoder = (value: DataView): string | null => {
    let posixTimezone = decoder.decode(value);
    let matchedTzIndex = tzmap.findIndex(
      ([label, posix]) => posix == posixTimezone,
    );
    if (matchedTzIndex >= 0) {
      return matchedTzIndex.toString();
    }
    return null;
  };

  const attributeReadOnce: GattAttributeParser<any>[] = [
    {
      attributeId: gatt.CHR_TEMPERATURE_UNIT_UUID,
      parse: (value: DataView) => value.getUint8(0),
      setter: setTemperatureUnit,
    },
    {
      attributeId: gatt.CHR_CLOCK_FORMAT_UUID,
      parse: (value: DataView) => value.getUint8(0),
      setter: setClockFormat,
    },
    {
      attributeId: gatt.CHR_WEATHER_PATH_UUID,
      parse: (value: DataView) => decoder.decode(value),
      setter: setWeatherPath,
    },
    {
      attributeId: gatt.CHR_TIMEZONE_UUID,
      parse: timezoneDecoder,
      setter: setTimezone,
    },
    {
      attributeId: gatt.CHR_APP_SELECTION_UUID,
      parse: (value: DataView) => value.getUint8(0),
      setter: setAppSelection,
    },
  ];
  const attributeObservers: GattAttributeParser<any>[] = [
    {
      attributeId: gatt.CHR_TEMPERATURE_UUID,
      parse: (value: DataView) => value.getFloat32(0, true),
      setter: setTemperature,
    },
    {
      attributeId: gatt.CHR_HUMIDITY_UUID,
      parse: (value: DataView) => value.getFloat32(0, true),
      setter: setHumidity,
    },
    {
      attributeId: gatt.CHR_CO2_UUID,
      parse: (value: DataView) => value.getUint16(0, true),
      setter: setCo2,
    },
    {
      attributeId: gatt.CHR_VOC_INDEX_UUID,
      parse: (value: DataView) => value.getInt32(0, true),
      setter: setVocIndex,
    },
    {
      attributeId: gatt.CHR_NOX_INDEX_UUID,
      parse: (value: DataView) => value.getInt32(0, true),
      setter: setNoxIndex,
    },
    {
      attributeId: gatt.CHR_WIFI_STATUS_UUID,
      parse: (value: DataView) => gatt.parseWifiStatus(value),
      setter: setWifiStatus,
    },
  ];

  const connectToDevice = async () => {
    const device = await navigator.bluetooth.requestDevice({
      filters: [{ services: [gatt.SERVICE_UUID] }],
    });
    device.addEventListener("gattserverdisconnected", () => {
      setBluetoothConnection(null);
    });
    if (!device?.gatt) {
      console.log("no device found");
      return;
    }
    const server = await device.gatt.connect();
    const service = await server.getPrimaryService(gatt.SERVICE_UUID);
    setBluetoothConnection({
      device: device,
      server: server,
      service: service,
    });

    // You need to read at least one value before calling startNoficiations()
    // in order to transition to an authorized session.
    for (let parser of [...attributeReadOnce, ...attributeObservers]) {
      try {
        let chr = await service.getCharacteristic(parser.attributeId);
        let val = await chr.readValue();
        if (val != undefined) {
          parser.setter(parser.parse(val));
        }
      } catch (err) {
        let attrIdStr =
          typeof parser.attributeId === "number"
            ? parser.attributeId.toString(16)
            : parser.attributeId;
        console.error(`Failed to read characteristic: ${attrIdStr}: ${err}`);
      }
    }

    for (let parser of attributeObservers) {
      try {
        let chr = await service.getCharacteristic(parser.attributeId);
        (await chr.startNotifications()).addEventListener(
          "characteristicvaluechanged",
          (event) => {
            let target = event.target as BluetoothRemoteGATTCharacteristic;
            let val = target?.value;
            if (val != undefined) {
              parser.setter(parser.parse(val));
            }
          },
        );
      } catch (err) {
        let attrIdStr =
          typeof parser.attributeId === "number"
            ? parser.attributeId.toString(16)
            : parser.attributeId;
        console.error(`Failed to observe characteristic: ${attrIdStr}: ${err}`);
      }
    }
  };

  const disconnect = async () => {
    if (!bluetoothConnection?.server) {
      return;
    }

    await bluetoothConnection.server.disconnect();
    setBluetoothConnection(null);
  };

  return (
    <Container py="md">
      <Group justify="space-between" align="middle">
        <Group>
          <Box style={{ width: 150 }}>
            <Logo />
          </Box>
          {bluetoothConnection ? (
            <Badge color="green" variant="outline">
              Connected
            </Badge>
          ) : (
            <Badge color="red" variant="outline">
              Disconnected
            </Badge>
          )}
        </Group>
        {bluetoothConnection ? (
          <Button size="md" onClick={disconnect} variant="outline">
            Disconnect
          </Button>
        ) : (
          <Button
            size="md"
            onClick={connectToDevice}
            variant="gradient"
            gradient={{ from: "indigo", to: "cyan", deg: 217 }}
            leftSection={<IconBluetooth size={20} />}
          >
            Connect to device
          </Button>
        )}
      </Group>
      {!bluetoothConnection ? (
        <Box ta="center" mt="xl">
          <Paper shadow="xs" p="md" maw={600} ml="auto" mr="auto">
            <p>
              Make sure Bitclock is powered (via USB) and within Bluetooth
              range.
            </p>
            <p>Google Chrome for macOS/Windows required for Web Bluetooth.</p>
          </Paper>
        </Box>
      ) : (
        <Box pt="md">
          <AqiBlock
            temperature={temperature}
            humidity={humidity}
            co2={co2}
            vocIndex={vocIndex}
            noxIndex={noxIndex}
            temperatureUnit={temperatureUnit}
          />

          <Divider my="md" />

          <AppSelectionBlock
            bluetoothConnection={bluetoothConnection}
            appSelection={appSelection}
            setAppSelection={setAppSelection}
          />

          <Divider my="md" />

          <ClockBlock
            bluetoothConnection={bluetoothConnection}
            timezone={timezone}
            setTimezone={setTimezone}
            clockFormat={clockFormat}
            setClockFormat={setClockFormat}
          />

          <Divider my="md" />

          <ClimateBlock
            bluetoothConnection={bluetoothConnection}
            temperatureUnit={temperatureUnit}
            setTemperatureUnit={setTemperatureUnit}
          />

          <Divider my="md" />

          <WeatherBlock
            bluetoothConnection={bluetoothConnection}
            weatherPath={weatherPath}
            setWeatherPath={setWeatherPath}
          />

          <Divider my="md" />

          <WifiBlock
            bluetoothConnection={bluetoothConnection}
            wifiStatus={wifiStatus}
          />

          <Divider my="md" />

          <FirmwareBlock bluetoothConnection={bluetoothConnection} />
        </Box>
      )}
    </Container>
  );
}
