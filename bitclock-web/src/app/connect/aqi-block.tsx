import React from "react";

import { Title, Card, SimpleGrid, Text } from "@mantine/core";
import * as gatt from "@/libs/gatt";

function roundToPlace(input: number, place: number): string {
  let multiple = 10 ** place;
  return (Math.round(input * multiple) / multiple).toFixed(place);
}

interface AqiBlockProps {
  temperature: number;
  humidity: number;
  co2: number;
  vocIndex: number;
  noxIndex: number;
  temperatureUnit: number;
}

export default function AqiBlock(props: AqiBlockProps) {
  return (
    <>
      <Title order={3}>Air quality</Title>
      <SimpleGrid cols={2} mt="sm" mb="lg">
        <Card shadow="sm" padding="lg">
          <Title order={3}>Temperature</Title>
          <Text size="xl">
            {props.temperatureUnit == gatt.TEMP_UNIT_VAL_FAHRENHEIT
              ? props.temperature
                ? `${roundToPlace(props.temperature * 1.8 + 32, 1)} °F`
                : "... °F"
              : props.temperature
                ? `${roundToPlace(props.temperature, 1)} °C`
                : "... °C"}
          </Text>
        </Card>
        <Card shadow="sm" padding="lg">
          <Title order={3}>Relative humidity</Title>
          <Text size="xl">{Math.round(props.humidity)}%</Text>
        </Card>
        <Card shadow="sm" padding="lg">
          <Title order={3}>
            CO<sub>2</sub>
          </Title>
          <Text size="xl">{props.co2} ppm</Text>
        </Card>
        <Card shadow="sm" padding="lg">
          <Title order={3}>VOC index</Title>
          <Text size="xl">VOC {props.vocIndex}</Text>
          <Text size="xl">
            NO<sub>x</sub> {props.noxIndex}
          </Text>
        </Card>
      </SimpleGrid>
    </>
  );
}
