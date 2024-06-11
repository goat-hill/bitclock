import React from "react";

import { Title, Text, Box, TextInput, Button, Anchor } from "@mantine/core";
import { BluetoothConnection } from "./ble-connection";
import Link from "next/link";
import { getNoaaForecastUrl } from "@/libs/weather";
import * as gatt from "@/libs/gatt";
import { useForm } from "@mantine/form";

interface WeatherBlockProps {
  bluetoothConnection: BluetoothConnection | null;
  weatherPath: string | null;
  setWeatherPath: (weatherPath: string) => void;
}

interface LocationFormValues {
  lat: string;
  lng: string;
}

export default function WeatherBlock({
  bluetoothConnection,
  weatherPath,
  setWeatherPath,
}: WeatherBlockProps) {
  const locationForm = useForm<LocationFormValues>({
    initialValues: {
      lat: "",
      lng: "",
    },
    validate: {
      lat: (value) => {
        const lat = parseFloat(value);
        return isNaN(lat) || lat < -90 || lat > 90
          ? "Invalid latitude. Must be between -90 and 90."
          : null;
      },
      lng: (value) => {
        const lng = parseFloat(value);
        return isNaN(lng) || lng < -180 || lng > 180
          ? "Invalid longitude. Must be between -180 and 180."
          : null;
      },
    },
  });

  const sendWeatherPath = async ({ lat, lng }: LocationFormValues) => {
    if (!bluetoothConnection?.service) {
      return;
    }

    const forecastUrl = await getNoaaForecastUrl(lat.trim(), lng.trim());
    let chr = await bluetoothConnection.service.getCharacteristic(
      gatt.CHR_WEATHER_PATH_UUID,
    );
    // Trim everything except the url path, using library functions but keep preceding slash
    const path = new URL(forecastUrl).pathname;
    const pathBuffer = new TextEncoder().encode(path);
    await chr.writeValueWithResponse(pathBuffer);
    setWeatherPath(path);
  };
  return (
    <>
      <Title order={3} mt="md" mb="sm">
        Weather forecast
      </Title>
      <Box maw={400}>
        <Box mb="sm">
          <Text size="sm">
            {weatherPath ? (
              <Anchor
                component={Link}
                href={`https://api.weather.gov/${weatherPath}`}
              >
                {`https://api.weather.gov${weatherPath}`}
              </Anchor>
            ) : (
              "No location set"
            )}
          </Text>
        </Box>

        <form onSubmit={locationForm.onSubmit(sendWeatherPath)}>
          <TextInput
            label="Latitude"
            key={locationForm.key("lat")}
            {...locationForm.getInputProps("lat")}
          />
          <TextInput
            label="Longitude"
            key={locationForm.key("lng")}
            {...locationForm.getInputProps("lng")}
          />
          <Button type="submit" mt="md">
            Update location
          </Button>
        </form>
      </Box>
    </>
  );
}
