import { Box, Container } from "@mantine/core";
import React from "react";
import Logo from "./logo";

export default function Home() {
  return (
    <Container py="md">
      <Box style={{ width: 400, marginLeft: "auto", marginRight: "auto" }}>
        <Logo />
      </Box>
    </Container>
  );
}
