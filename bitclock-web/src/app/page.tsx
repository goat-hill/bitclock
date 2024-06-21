"use client";

import {
  Box,
  Flex,
  Container,
  Text,
  Title,
  Button,
  ThemeIcon,
  SimpleGrid,
  rem,
  Table,
  Anchor,
} from "@mantine/core";
import {
  IconBluetooth,
  IconBrandGithub,
  IconClock,
  IconCpu,
  IconCube,
  IconWind,
} from "@tabler/icons-react";
import Link from "next/link";
import React from "react";
import Logo from "./logo";
import SlicerImage from "@/app/img/3d-print.jpg";
import PcbImage from "@/app/img/pcb.jpg";
import BitclockWhite from "@/app/img/bitclock-white.jpg";
import ConfiguratorImage from "@/app/img/configurator.png";
import BitclockImage from "@/app/img/bitclock.jpg";
import NextImage from "next/image";
import { Image } from "@mantine/core";

export default function Home() {
  const features = [
    {
      icon: IconWind,
      title: "Air quality + CO₂ alerts",
      body: "Maximize your comfort and productivity by measuring CO₂ levels, temperature, humidity, and VOC levels (volatile organic compounds).",
    },
    {
      icon: IconClock,
      title: "Customizable e-ink display",
      body: "Select between current time and today's weather forecast using the companion browser tool.",
    },
    {
      icon: IconBrandGithub,
      title: "Hackable + open source",
      body: "3D print your own case, or develop your own clockface app. It's unlocked, open source, and ready for hacking!",
    },
  ];
  return (
    <Container>
      <Box style={{ overflow: "clip" }}>
        <Box style={{ overflow: "hidden", x: 0, y: 0 }} pos="relative">
          <Flex
            px={{ base: "xs", md: "md" }}
            py="xl"
            align="center"
            justify="center"
            direction={{ base: "column", md: "row" }}
          >
            <Box style={{ width: rem(250) }}>
              <Logo />
            </Box>
            <Box flex={1} mih={20}></Box>
            <Box>
              <Title order={1}>An e-ink desk companion</Title>
              <Text c="dimmed">
                A 3D-printed air quality monitor, desk clock, and customizable
                e-ink gadget.
              </Text>
            </Box>
          </Flex>
        </Box>
      </Box>

      <Box style={{ position: "relative", display: "inline-block" }}>
        <Image
          component={NextImage}
          src={BitclockImage}
          w="100%"
          width={928}
          h="auto"
          alt="Two Bitclock devices on a desk"
          radius="md"
          fit="contain"
          priority
        />
        <Box
          h="22%"
          style={(style) => ({
            position: "absolute",
            bottom: 0,
            right: 0,
            left: 0,
            background:
              "linear-gradient(to top, rgba(245, 242, 240, 0.9), rgba(245, 242, 240, 0))",
            pointerEvents: "none", // Ensure the gradient does not block the overlay content
            borderRadius: style.radius.md,
            borderWidth: 0,
          })}
        />
        <Box
          style={{ position: "absolute", bottom: 0, right: 0 }}
          py="md"
          px="lg"
        >
          <Button
            variant="gradient"
            gradient={{ deg: 133, from: "blue", to: "cyan" }}
            size="lg"
            radius="md"
            mt="xl"
            component={Link}
            href="/order"
          >
            Get Bitclock
          </Button>
        </Box>
      </Box>

      <Flex direction="column" mt="md" align="center" gap="md">
        <Flex direction="row" gap="md">
          <Button
            component={Link}
            size="xs"
            href="https://github.com/goat-hill/bitclock"
            target="_blank"
            leftSection={<IconBrandGithub size={14} />}
            variant="outline"
          >
            Code on GitHub
          </Button>
          <Button
            component={Link}
            size="xs"
            href="/connect"
            variant="outline"
            color="blue"
            leftSection={<IconBluetooth size={14} />}
          >
            Connect to device
          </Button>
        </Flex>
      </Flex>

      <SimpleGrid cols={{ base: 1, sm: 3 }} mt={50} spacing="xl">
        {features.map((feature) => (
          <Box key={feature.title}>
            <ThemeIcon size={50} radius="md">
              <feature.icon
                style={{ width: rem(30), height: rem(30) }}
                stroke={1.5}
              />
            </ThemeIcon>
            <Text fz="lg" mt="sm" fw={500}>
              {feature.title}
            </Text>
            <Text c="dimmed">{feature.body}</Text>
          </Box>
        ))}
      </SimpleGrid>
      <Flex
        direction="column"
        gap={{ base: rem(60), md: rem(100) }}
        py={{ base: rem(60), md: rem(100) }}
      >
        <Flex
          direction="row"
          gap="xl"
          align="center"
          justify="center"
          wrap="wrap"
        >
          <Image
            component={NextImage}
            src={SlicerImage}
            w={{ base: "100%", sm: 400 }}
            width={400}
            h="auto"
            alt="Prusa slicer screenshot of enclosure 3D print"
            radius="md"
            fit="contain"
            bd="1px solid gray.6"
          />
          <Box flex={1}>
            <Title order={2}>Have a 3D printer?</Title>
            <Text mt="md">
              The Bitclock enclosure is a 3D printed design and you can
              optionally assemble a Bitclock yourself. Order the Bitclock PCB,
              insert it into your print, and snap it together. No other parts
              needed!
            </Text>
            <Flex direction="row" gap="md" mt="md">
              <Button
                component={Link}
                leftSection={<IconCube size={14} />}
                href="https://github.com/goat-hill/bitclock/tree/master/bitclock-enclosure"
                variant="outline"
                target="_blank"
              >
                STL files on GitHub
              </Button>
              <Button
                component={Link}
                leftSection={<IconCpu size={14} />}
                variant="gradient"
                gradient={{ from: "indigo", to: "cyan", deg: 217 }}
                href="/order"
              >
                Order PCB
              </Button>
            </Flex>
          </Box>
        </Flex>

        <Flex
          direction="row"
          gap="xl"
          align="center"
          justify="center"
          wrap="wrap"
        >
          <Image
            component={NextImage}
            src={PcbImage}
            w={{ base: "100%", sm: 400 }}
            h="auto"
            width={400}
            alt="Prusa slicer screenshot of enclosure 3D print"
            radius="md"
            fit="contain"
          />
          <Box flex={1}>
            <Title order={2}>Technical specs</Title>
            <Table verticalSpacing={rem(5)} horizontalSpacing={rem(5)} mt="xs">
              <Table.Tbody>
                <Table.Tr>
                  <Table.Td>
                    <Text fw="bold">Display</Text>
                  </Table.Td>
                  <Table.Td>
                    <Text size="sm">
                      2.71&quot; E-Ink display (264 x 176 px, 117 dpi)
                      <br />
                      Supports fast update (update pixels w/o screen flash)
                    </Text>
                  </Table.Td>
                </Table.Tr>
                <Table.Tr>
                  <Table.Td>
                    <Text fw="bold">Sensors</Text>
                  </Table.Td>
                  <Table.Td>
                    <Text size="sm">
                      Temperature + relative humdiity (SHT41)
                      <br />
                      CO₂ (SCD40)
                      <br />
                      VOC + NOx index (SGP41)
                    </Text>
                  </Table.Td>
                </Table.Tr>
                <Table.Tr>
                  <Table.Td>
                    <Text fw="bold">SoC</Text>
                  </Table.Td>
                  <Table.Td>
                    <Text size="sm">
                      ESP32-S3 with 2.4 GHz Wi-Fi and Bluetooth 5 (LE)
                      <br />
                      8MB Flash memory
                    </Text>
                  </Table.Td>
                </Table.Tr>
                <Table.Tr>
                  <Table.Td>
                    <Text fw="bold">Power</Text>
                  </Table.Td>
                  <Table.Td>
                    <Text size="sm">USB-C (cable not included)</Text>
                  </Table.Td>
                </Table.Tr>
                <Table.Tr>
                  <Table.Td>
                    <Text fw="bold">Configuration</Text>
                  </Table.Td>
                  <Table.Td>
                    <Text size="sm">
                      Via Bluetooth using{" "}
                      <Anchor component={Link} target="_blank" href="/connect">
                        bitclock.io/connect
                      </Anchor>
                      <br />
                      Google Chrome required
                    </Text>
                  </Table.Td>
                </Table.Tr>
                <Table.Tr>
                  <Table.Td>
                    <Text fw="bold">Debug</Text>
                  </Table.Td>
                  <Table.Td>
                    <Text size="sm">Flashing, logging, GDB over USB</Text>
                  </Table.Td>
                </Table.Tr>
                <Table.Tr>
                  <Table.Td fw="bold">
                    <Text fw="bold">Dimensions</Text>
                  </Table.Td>
                  <Table.Td>
                    <Text size="sm">
                      2.9&quot; height x 3.5&quot; width x 2.0&quot; depth
                    </Text>
                  </Table.Td>
                </Table.Tr>
              </Table.Tbody>
            </Table>
          </Box>
        </Flex>

        <Flex
          direction="row"
          gap="xl"
          align="center"
          justify="center"
          wrap="wrap"
        >
          <Image
            component={NextImage}
            src={ConfiguratorImage}
            w={{ base: "100%", sm: 400 }}
            width={400}
            h="auto"
            alt="Configurator screenshot showing sensor readings"
            radius="md"
            bd="1px solid gray.6"
            fit="contain"
          />
          <Box flex={1}>
            <Title order={2}>Product disclaimers</Title>
            <ul>
              <li>
                <strong>Google Chrome for macOS/Windows required</strong> for{" "}
                <Anchor component={Link} target="_blank" href="/connect">
                  configuration
                </Anchor>{" "}
                and viewing sensor readings via{" "}
                <Anchor
                  component={Link}
                  target="_blank"
                  href="https://developer.mozilla.org/en-US/docs/Web/API/Web_Bluetooth_API"
                >
                  Web Bluetooth
                </Anchor>
              </li>
              <li>
                <strong>USB-C cable required</strong> for power (not included)
              </li>
              <li>
                <strong>2.4 GHz Wi-Fi connection required</strong> for clock
                sync + weather forecast functionality
              </li>
              <li>
                Display <strong>does not have a backlight</strong> and requires
                ambient light to be visible
              </li>
              <li>
                Sensors are not perfectly accurate and can be impacted by device
                enclosure
              </li>
              <li>Device does not feature a PM2.5 particulate sensor</li>
              <li>Currently USA shipping only</li>
            </ul>
            <Text>
              See the{" "}
              <Anchor component={Link} href="/faq">
                FAQ
              </Anchor>{" "}
              for more information.
            </Text>
          </Box>
        </Flex>

        <Flex
          direction="row"
          gap="xl"
          align="center"
          justify="center"
          wrap="wrap"
        >
          <Image
            component={NextImage}
            src={BitclockWhite}
            w={{ base: "100%", sm: 400 }}
            h="auto"
            width={400}
            alt="Bitclock photo"
            radius="md"
            bd="1px solid gray.6"
            fit="contain"
          />
          <Box flex={1}>
            <Title order={2}>About the project</Title>
            <p>
              Bitclock is an exploration of designing hardware with 3D-printed
              components.
            </p>
            <p>
              The enclosure, PCB, and software are designed by{" "}
              <Anchor
                component={Link}
                target="_blank"
                href="https://x.com/bradysays"
              >
                Brady Law
              </Anchor>{" "}
              in San Francisco. Previously, Brady worked as a software engineer
              for Lyft and Apple.
            </p>
            <p>Reach out if you have any questions! brady@bitclock.io</p>
            <Button
              variant="gradient"
              gradient={{ deg: 133, from: "blue", to: "cyan" }}
              size="md"
              radius="md"
              mt="md"
              component={Link}
              href="/order"
            >
              Get Bitclock
            </Button>
          </Box>
        </Flex>
      </Flex>
    </Container>
  );
}
