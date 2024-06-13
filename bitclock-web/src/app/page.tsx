import { Box, Flex, Container, Text, Button } from "@mantine/core";
import { IconBluetooth, IconBrandGithub } from "@tabler/icons-react";
import Link from "next/link";
import React from "react";
import Logo from "./logo";

export default function Home() {
  return (
    <Container py="md">
      <Box style={{ marginLeft: "auto", marginRight: "auto" }}>
        <Logo />
        <Flex direction="column"
        mt="md"
        align="center"
        gap="md">
          <Flex direction="row" gap="md">
            <Button component={Link} href="https://github.com/goat-hill/bitclock" target="_blank" leftSection={<IconBrandGithub size={14} />}>Code on GitHub</Button>
            <Button component={Link} href="/connect" variant="gradient" gradient={{ from: "indigo", to: "cyan", deg: 217 }} leftSection={<IconBluetooth size={14} />}>Connect to device</Button>
          </Flex>
          <Text ta="center">Shipping <strong>July 2024</strong>. Join mailing list to stay updated!</Text>
          <Box w={{ base: '100%', xs: 480, md: 600 }}>
          <iframe src="https://docs.google.com/forms/d/e/1FAIpQLSd2UAob-NJfPU_3VcjZiAaslMEGd6qegJ8XPgLOtxQYYuO0Aw/viewform?embedded=true"
            width="100%" height="1500" frameBorder={0} marginHeight={0} marginWidth={0}>Loading…</iframe>
          </Box>
          
        </Flex>
      </Box>
    </Container>
  );
}
