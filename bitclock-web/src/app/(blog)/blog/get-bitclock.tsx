import React from "react";

import { Button, Center } from "@mantine/core";
import Link from "next/link";

export default function GetBitclock() {
  return (
    <Center>
      <Button
        variant="gradient"
        gradient={{ deg: 133, from: "blue", to: "cyan" }}
        size="lg"
        radius="md"
        component={Link}
        target="_blank"
        href="/"
        my="md"
      >
        Learn more about Bitclock
      </Button>
    </Center>
  );
}
