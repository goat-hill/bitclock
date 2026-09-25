import products from "./products";
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

export default function OrderPage() {
  return (
    <>
      <Text ta="center" size="sm" mt="sm" mb="lg">
        Thanks for supporting this indie hardware project!
        <br />
        📦 Please allow up to 2 weeks for PCB assembly, 3D printing, and
        packaging.
      </Text>
      <SimpleGrid
        cols={{ base: 1, sm: 2 }}
        spacing="xl"
        verticalSpacing={rem(80)}
      >
        {products.map((product) => (
          <div
            key={product.id}
            style={{ opacity: 0.5, filter: "grayscale(100%)" }}
          >
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
              <Button variant="default" size="md" radius="md" mt="md" disabled>
                Out of stock
              </Button>
            </Flex>
          </div>
        ))}
      </SimpleGrid>
    </>
  );
}
