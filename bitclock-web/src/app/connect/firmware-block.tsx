import React from "react";

import {
  Group,
  Title,
  Badge,
  Flex,
  Button,
  FileButton,
  Box,
  Progress,
  Text,
  Loader,
} from "@mantine/core";
import { useState, useRef } from "react";

import * as gatt from "@/libs/gatt";
import { BluetoothConnection } from "./ble-connection";

interface FirmwareBlockProps {
  bluetoothConnection: BluetoothConnection | null;
}

export default function FirmwareBlock({
  bluetoothConnection,
}: FirmwareBlockProps) {
  const [file, setFile] = useState<File | null>(null);
  const [otaProgress, setOtaProgress] = useState(0);
  const cancelOtaRef = useRef<boolean>(false);

  const locale =
    typeof navigator !== "undefined" ? navigator.language : "en-US";
  const numberFormatter = new Intl.NumberFormat(locale);

  const startFirmwareUpdate = async (file: File | null) => {
    setFile(file);

    if (!file) {
      return;
    }

    const btService = bluetoothConnection?.service;
    if (!btService) {
      return;
    }

    let otaSizeCharacteristic = await btService.getCharacteristic(
      gatt.CHR_OTA_SIZE_UUID,
    );
    let otaDataCharacteristic = await btService.getCharacteristic(
      gatt.CHR_OTA_DATA_UUID,
    );

    const sizeBuffer = new ArrayBuffer(4);
    const dataView = new DataView(sizeBuffer);
    dataView.setUint32(0, file.size, true); // true for little-endian
    await otaSizeCharacteristic.writeValueWithResponse(dataView);

    const reader = new FileReader();
    reader.onload = async function (event) {
      const arrayBuffer = event.target!.result as ArrayBuffer;
      const chunkSize = 512; // Adjust chunk size as needed
      for (let i = 0; i < arrayBuffer.byteLength; i += chunkSize) {
        if (cancelOtaRef.current) {
          cancelOtaRef.current = false;
          break;
        }
        const chunk = arrayBuffer.slice(i, i + chunkSize);
        console.log(
          `Uploading part (${i} / ${file.size}): ${Math.round(((1.0 * i) / file.size) * 100)}%`,
        );

        // FIXME: Final chunk throws an error on reboot
        await otaDataCharacteristic.writeValueWithResponse(
          new Uint8Array(chunk),
        );
        setOtaProgress(Math.floor((i / file.size) * 100));
      }
      setFile(null);
    };
    await reader.readAsArrayBuffer(file);
  };

  const cancelFirmwareUpdate = () => {
    cancelOtaRef.current = true;
  };

  return (
    <>
      <Group>
        <Title order={3}>Firmware</Title>
        <Badge variant="outline">v1.0.0</Badge>
        <Badge variant="outline" color="green">
          Official
        </Badge>
      </Group>

      {!file && (
        <FileButton
          onChange={startFirmwareUpdate}
          accept="application/octet-stream"
        >
          {(props) => (
            <Button {...props} variant="outline" mt="md">
              Upload custom build
            </Button>
          )}
        </FileButton>
      )}
      {file && (
        <>
          <Flex
            gap="sm"
            justify="flex-start"
            align="center"
            direction="row"
            mt="md"
          >
            <Loader />
            <Text size="sm">Update in progress...</Text>
          </Flex>
          <Flex
            gap="md"
            justify="flex-start"
            align="center"
            direction="row"
            wrap="wrap"
            mt="md"
          >
            <Box>
              <Text size="sm" fw="bold">
                {file.name}
              </Text>
              <Text size="sm">{numberFormatter.format(file.size)} bytes</Text>
            </Box>
            <Box w={{ base: 200, sm: 400 }}>
              <Progress size="xl" value={otaProgress} striped animated />
            </Box>
            <Box>{otaProgress}%</Box>
          </Flex>
          <Button
            mt="md"
            color="red"
            variant="outline"
            onClick={cancelFirmwareUpdate}
          >
            Cancel
          </Button>
        </>
      )}
    </>
  );
}
