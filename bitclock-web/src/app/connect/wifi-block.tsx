import React from "react";
import { Box, TextInput, Button, Group, Title, Badge } from "@mantine/core";
import { useForm } from "@mantine/form";
import { stringToArrayBuffer } from "@/libs/util";
import * as gatt from "@/libs/gatt";
import { BluetoothConnection } from "./ble-connection";

interface WifiFormValues {
  ssid: string;
  password: string;
}

interface WifiBlockProps {
  bluetoothConnection: BluetoothConnection | null;
  wifiStatus: gatt.WifiStatus | null;
}

export default function WifiBlock({
  bluetoothConnection,
  wifiStatus,
}: WifiBlockProps) {
  const wifiForm = useForm<WifiFormValues>({
    mode: "uncontrolled",
    initialValues: {
      ssid: "",
      password: "",
    },

    validate: {
      ssid: (value) =>
        value.length < 1 || value.length > 32 ? "Invalid SSID" : null,
      password: (value) => (value.length > 64 ? "Invalid password" : null),
    },
  });

  const sendWifiCredentials = async (values: WifiFormValues) => {
    if (!bluetoothConnection?.service) {
      return;
    }

    const characteristic = await bluetoothConnection.service.getCharacteristic(
      gatt.CHR_WIFI_UUID,
    );
    await characteristic.writeValueWithResponse(
      stringToArrayBuffer(`${values.ssid}|${values.password}`),
    );
  };

  return (
    <form onSubmit={wifiForm.onSubmit(sendWifiCredentials)}>
      <Group>
        <Title order={3}>Wi-Fi</Title>
        <Badge
          variant="outline"
          color={
            wifiStatus?.isConnected
              ? "green"
              : wifiStatus?.isStarted
                ? "yellow"
                : "gray"
          }
        >
          {wifiStatus?.isConnected
            ? "Connected"
            : wifiStatus?.isStarted
              ? "Connecting"
              : "Disconnected"}
        </Badge>
      </Group>
      <Box maw={400}>
        <TextInput
          mt="sm"
          label="SSID"
          key={wifiForm.key("ssid")}
          {...wifiForm.getInputProps("ssid")}
        />
        <TextInput
          mt="md"
          label="Password"
          type="password"
          key={wifiForm.key("password")}
          {...wifiForm.getInputProps("password")}
        />
      </Box>
      <Button type="submit" mt="md">
        Update credentials
      </Button>
    </form>
  );
}
