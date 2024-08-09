import React from "react";
import NextImage from "next/image";

import { Box, Flex, Text, Image, rem } from "@mantine/core";

interface InlineImageProps {
  src: typeof NextImage;
  alt: string;
  w?: typeof Box.w;
  label?: string;
  priority?: boolean;
}

export default function InlineImage(props: InlineImageProps) {
  return (
    <Flex
      direction="column"
      justify="center"
      align="center"
      gap="sm"
      my={rem(50)}
    >
      <Box w={props.w ?? { base: "100%", sm: 600, md: 800 }}>
        <Image
          component={NextImage}
          src={props.src}
          w="100%"
          h="auto"
          width={800}
          alt={props.alt}
          radius="md"
          fit="cover"
          mt="md"
          priority={props.priority}
        />
      </Box>
      {props.label && (
        <Text size="sm" c="dimmed" fw="bold">
          {props.label}
        </Text>
      )}
    </Flex>
  );
}
