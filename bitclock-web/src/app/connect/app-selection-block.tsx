import React from "react";

import { Title, Radio, Group } from "@mantine/core";
import { BluetoothConnection } from "./ble-connection";
import * as gatt from "@/libs/gatt";

interface AppSelectionBlockProps {
  bluetoothConnection: BluetoothConnection | null;
  appSelection: number;
  setAppSelection: (value: number) => void;
}

export default function AppSelectionBlock({
  bluetoothConnection,
  appSelection,
  setAppSelection,
}: AppSelectionBlockProps) {
  const sendAppSelection = async (value: number) => {
    if (!bluetoothConnection?.service) {
      return;
    }
    let chr = await bluetoothConnection.service.getCharacteristic(
      gatt.CHR_APP_SELECTION_UUID,
    );
    const sizeBuffer = new ArrayBuffer(1);
    const dataView = new DataView(sizeBuffer);
    dataView.setUint8(0, value);
    await chr.writeValueWithResponse(dataView);
  };

  return (
    <>
      <Title order={3}>Display mode</Title>
      <Radio.Group
        name="appSelection"
        mt="md"
        value={appSelection.toString()}
        onChange={async (valueString) => {
          let value = parseInt(valueString);
          await sendAppSelection(value);
          setAppSelection(value);
        }}
      >
        <Group mt="xs">
          <Radio
            value={gatt.APP_SELECTION_VAL_CLOCK.toString()}
            label="Clock"
          />
          <Radio
            value={gatt.APP_SELECTION_VAL_WEATHER.toString()}
            label="Daily weather forecast"
          />
          <Radio
            value={gatt.APP_SELECTION_VAL_AQI.toString()}
            label="Air quality"
          />
        </Group>
      </Radio.Group>
    </>
  );
}
