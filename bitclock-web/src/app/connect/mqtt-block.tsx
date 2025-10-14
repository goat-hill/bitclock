
import { CHR_MQTT_URL_UUID } from "@/libs/gatt";
import { Anchor, Box, Button, TextInput, Title } from "@mantine/core";
import { useForm } from "@mantine/form";
import Link from "next/link";
import type { BluetoothConnection } from "./ble-connection";

interface MqttBlockProps {
  bluetoothConnection: BluetoothConnection | null;
  mqttUrl: string | null;
  setMqttUrl: (mqttUrl: string) => void;
}

interface MqttUrlFormValues {
  url: string;
}

export default function MqttBlock({
  bluetoothConnection,
  mqttUrl,
  setMqttUrl,
}: MqttBlockProps) {
  const urlForm = useForm<MqttUrlFormValues>({
    initialValues: {
      url: mqttUrl || "",
    },
    validate: {
      url: (value) => {
        return !value
          ? "Invalid URL"
          : null;
      },
    },
  });

  const sendMqttUrl = async ({ url }: MqttUrlFormValues) => {
    if (!bluetoothConnection?.service) {
      return;
    }

    const urlBuffer = new TextEncoder().encode(url);
    const chr = await bluetoothConnection.service.getCharacteristic(
      CHR_MQTT_URL_UUID,
    );
    await chr.writeValueWithResponse(urlBuffer);
    console.log("MQTT URL sent", url);
    setMqttUrl(url);
  };
  return (
    <>
      <Title order={3} mt="md" mb="sm">
        MQTT URL
      </Title>
      <Box maw={400}>
        <Box mb="sm">
            If set, Bitclock sensor data will be published as Home Assistant sensor data to MQTT server at this URL.<br />
            The sensors will be discovered via <Anchor component={Link} href="https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery">MQTT Discovery</Anchor><br />
        </Box>
        <form onSubmit={urlForm.onSubmit(sendMqttUrl)}>
          <TextInput
            label="URL"
            placeholder="mqtt://user:password@mqtt.example.com:1883"
            key={urlForm.key("url")}
            value={mqttUrl || ""}
            {...urlForm.getInputProps("url")}
          />
          <Button type="submit" mt="md">
            Update URL
          </Button>
        </form>
      </Box>
    </>
  );
}
