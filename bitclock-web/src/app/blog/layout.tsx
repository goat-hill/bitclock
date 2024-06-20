import React from "react";

import { Container, Box, Paper } from "@mantine/core";
import Logo from "@/app/logo";

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <Container
      py="md"
      w={{ base: "100%", md: 1000 }}
      style={{ lineHeight: 1.8 }}
    >
      <Box style={{ width: 200, marginLeft: "auto", marginRight: "auto" }}>
        <Logo />
      </Box>
      <Paper shadow="xs" p="md" mt="xl">
        {children}
        <p>by Brady Law</p>
      </Paper>
    </Container>
  );
}
