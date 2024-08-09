import React from "react";

import { Box, Flex, Text, rem } from "@mantine/core";

interface InlineVideoProps {
  src: string;
  autoplay?: boolean;
  loop?: boolean;
  label?: string;
}

export default function InlineVideo(props: InlineVideoProps) {
  return (
    <Flex direction="column" justify="center" align="center" gap="sm" my="xl">
      <Box w={{ base: "100%", sm: 600, md: 800 }}>
        <video
          width="100%"
          preload="auto"
          playsInline
          muted
          autoPlay={props.autoplay}
          loop={props.loop}
          style={{ borderRadius: 5 }}
        >
          <source src={props.src} type="video/mp4" />
          Your browser does not support the video tag.
        </video>
      </Box>
      {props.label && (
        <Text size="sm" c="dimmed" fw="bold">
          {props.label}
        </Text>
      )}
    </Flex>
  );
}
