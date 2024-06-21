"use client";

import React, { useEffect, useState } from "react";
import { redirect } from "next/navigation";
import { Title, Box, Center, Loader, Image, Button, Flex } from "@mantine/core";
import BitclockImage from "@/app/img/bitclock.jpg";
import NextImage from "next/image";
import { IconBrandGithub, IconCube } from "@tabler/icons-react";
import Link from "next/link";

export default function Return() {
  const [status, setStatus] = useState<string | null>(null);
  const [customerEmail, setCustomerEmail] = useState("");

  useEffect(() => {
    async function loadSession() {
      const queryString = window.location.search;
      const urlParams = new URLSearchParams(queryString);
      const sessionId = urlParams.get("session_id");

      const res = await fetch(
        `/api/checkout_sessions?session_id=${sessionId}`,
        {
          method: "GET",
        },
      );
      const data = await res.json(); // TODO: Use typed parsing
      setStatus(data.status);
      setCustomerEmail(data.customer_email);
    }
    loadSession();
  }, []);

  if (status === "open") {
    return redirect("/");
  }

  if (status === "expired") {
    return <>Failed to complete order.</>;
  }

  return (
    <>
      {status === "complete" && (
        <Box mt="xl">
          <Title order={2}>Order complete</Title>
          <p>
            Thank you for your order! A confirmation email will be sent to{" "}
            {customerEmail}.
          </p>
          <p>
            📦 Please allow up to 2 weeks for PCB assembly, 3D printing, and
            packaging. You will be notified when it is shipped.
          </p>

          <Image
            component={NextImage}
            src={BitclockImage}
            w="100%"
            width={600}
            h="auto"
            alt="Two Bitclock devices on a desk"
            radius="md"
            fit="contain"
            maw={600}
          />
          <p>
            If you have any questions, please email{" "}
            <a href="mailto:info@bitclock.io">info@bitclock.io</a>.
          </p>
          <Flex direction="row" gap="md">
            <Button
              component={Link}
              size="md"
              href="https://github.com/goat-hill/bitclock"
              target="_blank"
              leftSection={<IconBrandGithub size={14} />}
              variant="outline"
            >
              Code on GitHub
            </Button>
            <Button
              component={Link}
              size="md"
              leftSection={<IconCube size={14} />}
              href="https://github.com/goat-hill/bitclock/tree/master/bitclock-enclosure"
              variant="outline"
              target="_blank"
            >
              STL files on GitHub
            </Button>
          </Flex>
        </Box>
      )}
      {status === "expired" && (
        <>
          <Title order={2}>Session expired</Title>
        </>
      )}
      {!status && (
        <Center>
          <Loader />
        </Center>
      )}
    </>
  );
}
