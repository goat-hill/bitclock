import React from "react";

import { Title, Box, Select, Radio, Group } from "@mantine/core";
import { BluetoothConnection } from "./ble-connection";
import { stringToArrayBuffer } from "@/libs/util";
import * as gatt from "@/libs/gatt";
import { tzmap } from "@/libs/tz";

interface ClockBlockProps {
  bluetoothConnection: BluetoothConnection | null;
  timezone: string | null;
  setTimezone: (timezone: string) => void;
  clockFormat: number;
  setClockFormat: (clockFormat: number) => void;
}

export default function ClockBlock({
  bluetoothConnection,
  timezone,
  setTimezone,
  clockFormat,
  setClockFormat,
}: ClockBlockProps) {
  const sendPosixTimezone = async (timezone: string) => {
    if (!bluetoothConnection?.service) {
      return;
    }

    const characteristic = await bluetoothConnection.service.getCharacteristic(
      gatt.CHR_TIMEZONE_UUID,
    );
    await characteristic.writeValueWithResponse(stringToArrayBuffer(timezone));
  };

  const sendClockFormat = async (value: number) => {
    if (!bluetoothConnection?.service) {
      return;
    }
    let chr = await bluetoothConnection.service.getCharacteristic(
      gatt.CHR_CLOCK_FORMAT_UUID,
    );
    const sizeBuffer = new ArrayBuffer(1);
    const dataView = new DataView(sizeBuffer);
    dataView.setUint8(0, value);
    await chr.writeValueWithResponse(dataView);
  };

  return (
    <>
      <Title order={3}>Clock</Title>
      <Box maw={340} mt="sm">
        <Select
          label="Timezone"
          placeholder="Select your timezone"
          value={timezone}
          onSearchChange={setTimezone}
          onChange={async (tzindex: string | null) => {
            if (tzindex == null) {
              return;
            }
            let idx = parseInt(tzindex);
            let posixTimezone = tzmap[idx][1];
            setTimezone(tzindex);
            await sendPosixTimezone(posixTimezone);
          }}
          searchable
          data={tzmap.map(([label, value], index) => ({
            label: label,
            value: index.toString(),
          }))}
        />
      </Box>
      <Radio.Group
        name="hourMode"
        label="Display format"
        mt="md"
        value={clockFormat.toString()}
        onChange={async (valueString) => {
          let value = parseInt(valueString);
          await sendClockFormat(value);
          setClockFormat(value);
        }}
      >
        <Group mt="xs">
          <Radio
            value={gatt.CLOCK_FORMAT_VAL_12HR.toString()}
            label="12-hour (1:00)"
          />
          <Radio
            value={gatt.CLOCK_FORMAT_VAL_24HR.toString()}
            label="24-hour (13:00)"
          />
        </Group>
      </Radio.Group>
    </>
  );
}
