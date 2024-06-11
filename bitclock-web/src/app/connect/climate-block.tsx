import React from "react";

import { Title, Radio, Group } from "@mantine/core";
import { BluetoothConnection } from "./ble-connection";
import * as gatt from "@/libs/gatt";

interface ClimateBlockProps {
  bluetoothConnection: BluetoothConnection | null;
  temperatureUnit: number;
  setTemperatureUnit: (value: number) => void;
}

export default function ClimateBlock({
  bluetoothConnection,
  temperatureUnit,
  setTemperatureUnit,
}: ClimateBlockProps) {
  const sendTemperatureUnit = async (value: number) => {
    if (!bluetoothConnection?.service) {
      return;
    }
    let chr = await bluetoothConnection.service.getCharacteristic(
      gatt.CHR_TEMPERATURE_UNIT_UUID,
    );
    const sizeBuffer = new ArrayBuffer(1);
    const dataView = new DataView(sizeBuffer);
    dataView.setUint8(0, value);
    await chr.writeValueWithResponse(dataView);
  };

  return (
    <>
      <Title order={3}>Air quality</Title>
      <Radio.Group
        name="tempDisplay"
        label="Temperature units"
        value={temperatureUnit.toString()}
        mt="sm"
        onChange={async (valueString) => {
          let value = parseInt(valueString);
          await sendTemperatureUnit(value);
          setTemperatureUnit(value);
        }}
      >
        <Group mt="xs">
          <Radio
            value={gatt.TEMP_UNIT_VAL_CELSIUS.toString()}
            label="Celsius"
          />
          <Radio
            value={gatt.TEMP_UNIT_VAL_FAHRENHEIT.toString()}
            label="Fahrenheit"
          />
        </Group>
      </Radio.Group>
    </>
  );
}
