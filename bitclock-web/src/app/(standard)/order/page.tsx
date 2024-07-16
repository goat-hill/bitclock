"use client";

import React, { useCallback, useState } from "react";
import { loadStripe } from "@stripe/stripe-js";
import {
  EmbeddedCheckoutProvider,
  EmbeddedCheckout,
} from "@stripe/react-stripe-js";
import products, { ProductInfo } from "./products";
import {
  Flex,
  Button,
  Image,
  SimpleGrid,
  Title,
  Text,
  Box,
  rem,
} from "@mantine/core";
import NextImage from "next/image";

const stripePromise = loadStripe(
  process.env.NEXT_PUBLIC_STRIPE_PUBLISHABLE_KEY,
);

export default function OrderPage() {
  const [selectedProductInfo, setSelectedProductInfo] =
    useState<ProductInfo | null>(null);

  const fetchClientSecret = useCallback(async () => {
    if (!selectedProductInfo) {
      return null;
    }

    const checkout_session = await fetch("/api/checkout_sessions", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: `{"productId": "${selectedProductInfo.id}"}`,
    });
    const res = await checkout_session.json();
    return res.clientSecret;
  }, [selectedProductInfo]);

  const options = { fetchClientSecret };
  return <>Coming soon...</>;
  return (
    <>
      <Text ta="center" size="sm" mt="sm" mb="lg">
        Thanks for supporting this indie hardware project!
        <br />
        📦 Please allow up to 2 weeks for PCB assembly, 3D printing, and
        packaging.
      </Text>
      {!selectedProductInfo ? (
        <SimpleGrid
          cols={{ base: 1, sm: 2 }}
          spacing="xl"
          verticalSpacing={rem(80)}
        >
          {products.map((product) => (
            <div key={product.id}>
              <Image
                component={NextImage}
                src={product.image}
                w="100%"
                width={928}
                h="auto"
                alt="Prusa slicer screenshot of enclosure 3D print"
                radius="md"
                fit="contain"
                bd="3px solid gray.4"
                priority
              />
              <Title order={2} mt="md">
                {product.name}
              </Title>
              <Text size="sm">{product.description}</Text>
              <Flex direction="row" align="center" mt="md">
                <Box>
                  <Text size={rem(28)} fw="bold">
                    ${product.price}
                  </Text>
                  <Text c="dimmed" fw={500}>
                    + $6 shipping
                  </Text>
                </Box>
                <Box flex={1} />
                <Button
                  variant="gradient"
                  gradient={{ deg: 133, from: "blue", to: "cyan" }}
                  size="md"
                  radius="md"
                  mt="md"
                  onClick={() => setSelectedProductInfo(product)}
                >
                  Buy
                </Button>
              </Flex>
            </div>
          ))}
        </SimpleGrid>
      ) : (
        <>
          <div id="checkout">
            <EmbeddedCheckoutProvider stripe={stripePromise} options={options}>
              <EmbeddedCheckout />
            </EmbeddedCheckoutProvider>
          </div>
        </>
      )}
    </>
  );
}
